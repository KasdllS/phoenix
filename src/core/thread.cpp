#include "core/thread.h"

#include <sys/eventfd.h>
#include <sys/prctl.h>

#include "core/poller.h"

#include "utils/assert.h"
#include "utils/thread/thread_local_storage.hpp"
#include "utils/time.hpp"

namespace core {

const static utils::thread::ThreadLocalStorage<Thread*> current_thd =
    utils::thread::ThreadLocalStorage<Thread*>([]() {
      auto p = new (Thread*)(nullptr);
      return p;
    });

Thread* Thread::this_thread() {
  auto thd = (*current_thd);
  return thd == nullptr ? Application::thread() : thd;
}

Thread::Thread(const std::string& name /* = "" */)
    : poller_(makePoller()), status_(Status::Exit), thd_name_(name) {}

Thread::~Thread() {
  std::scoped_lock lck(mtx_);
  if (!run_) {
    return;
  }
  stop();
  if (thd_.joinable()) {
    thd_.join();
  }
}

void Thread::start() {
  std::scoped_lock lck(mtx_);
  if (run_) {
    return;
  }

  run_ = true;
  std::thread thd([this]() { threadMain(); });
  thd_.swap(thd);
}

void Thread::stop() {
  std::scoped_lock lck(mtx_);
  if (!run_) {
    return;
  }

  run_ = false;
  poller_->wakeup();
}

Trigger Thread::addEvent(const int fd,
                         const Events event,
                         const Event::Handler& handler) const {
  auto p = poller_->addEvent(fd, event, handler);
  p->thd_ = this;
  Trigger ret(p);
  return ret;
}

Trigger Thread::addEvent(const Events event,
                         const Event::Handler& handler) const {
  auto p = poller_->addEvent(event, handler);
  p->thd_ = this;
  Trigger ret(p);
  return ret;
}

void Thread::removeEvent(const int ev_fd) const {
  poller_->rmEvent(ev_fd);
}

int Thread::addTimer(int64_t microseconds,
                     const Event::Handler& handler,
                     bool single_shot) const {
  return poller_->addTimer(microseconds, handler, single_shot);
}

void Thread::removeTimer(int timer_id) const {
  poller_->rmTimer(timer_id);
}

void Thread::processEvents(int max_time) const {
  if (this_thread() != this) {
    return;
  }

  do {
    auto start = utils::time::get_cur_ts();
    poller_->run(max_time);
    auto cost = utils::time::get_cur_ts() - start;
    max_time -= cost;
  } while (max_time > 0);
}

void Thread::threadMain() {
  status_ = Status::Starting;
  *current_thd = this;
  if (!thd_name_.empty()) {
    prctl(PR_SET_NAME, thd_name_.c_str());
  }

  status_ = Status::Running;
  while (run_) {
    poller_->run();
  }
  *current_thd = nullptr;

  status_ = Status::Exit;
}

class MainThread : public Thread {
 public:
  MainThread() = default;

  int exec() {
    run_ = true;
    status_ = Status::Starting;

    status_ = Status::Running;
    while (run_) {
      poller_->run();
    }

    status_ = Status::Exit;

    return 0;
  }

  void exit() {
    run_ = false;
    while (status_ != Status::Exit) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  void start() override {}
  void stop() override {}
  void join() override {}
};

Application::Application() : thd_(new MainThread) {}
Application::~Application() {
  delete thd_;
  thd_ = nullptr;
}

int Application::exec() {
  return instance().thd_->exec();
}

void Application::exit() {
  instance().thd_->exit();
}

Thread* Application::thread() {
  return instance().thd_;
}

}  // namespace core