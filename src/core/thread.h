#pragma once

#include <thread>

#include <functional>

#include "core/event.h"
#include "core/timer.h"

#include <mutex>
#include "utils/thread/annotations.hpp"

namespace core {

class Poller;

class Thread {
 public:
  static Thread* this_thread();

  enum class Status {
    Starting,
    Running,
    Exit,
  };

  explicit Thread(const std::string& name = "");
  virtual ~Thread();

  virtual void start();
  virtual void stop();
  virtual void join() {
    if (thd_.joinable()) {
      thd_.join();
    }
  }

  Trigger addEvent(const int fd,
                   const Events event,
                   const Event::Handler& handler) const;
  Trigger addEvent(const Events event, const Event::Handler& handler) const;
  void removeEvent(const int fd) const;

  int addTimer(int64_t microseconds,
               const Event::Handler& handler,
               bool single_shot) const;
  void removeTimer(int timer_id) const;

  std::string name() const { return thd_name_; }

  void processEvents(int max_time) const;

 protected:
  void threadMain();

  std::mutex mtx_;
  bool run_ = false GAURDED_BY(mtx_);
  std::shared_ptr<Poller> poller_ GAURDED_BY(mtx_);

  Status status_;

  std::thread thd_;
  std::string thd_name_;
};

class MainThread;
class Application {
 public:
  static int exec();
  static void exit();
  static Thread* thread();

 private:
  static Application& instance() {
    static Application inst;
    return inst;
  }

  Application();
  ~Application();

  MainThread* thd_;
};

}  // namespace core