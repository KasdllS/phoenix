#include "core/poller/epoller.h"

#include <iostream>

#include <sys/eventfd.h>
#include <sys/timerfd.h>

#include <string.h>
#include <unistd.h>

#include "utils/assert.h"
#include "utils/macros.hpp"

namespace core {

Epoller::Epoller()
    : fd_(epoll_create1(EPOLL_CLOEXEC)),
      wake_fd_(eventfd(0, EFD_CLOEXEC)),
      timer_fd_(timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)) {
  if (fd_ == -1) {
    std::cerr << "Error creating epoll instance: " << strerror(errno)
              << std::endl;
    abort();
  }
  fassert(wake_fd_ != -1);
  fassert(timer_fd_ != -1);

  Operation op{
      Operation::Type::ADD,
      create(wake_fd_, Events::Execute, [this](const Event*) { handle(); })};
  list_.push_back(op);
  op.event_ = create(timer_fd_, Events::Execute,
                     [this](const Event*) { handleTimer(); });
  list_.push_back(op);
  handle();
}

EventPtr Epoller::addEvent(int fd,
                           Events events,
                           const Event::Handler& handler) {
  auto ev = create(fd, events, handler);
  Operation op{Operation::Type::ADD, ev};
  list_.push_back(op);
  wakeup();
  return ev;
}

EventPtr Epoller::addEvent(Events events, const Event::Handler& handler) {
  return addEvent(eventfd(0, EFD_CLOEXEC), events, handler);
}

void Epoller::rmEvent(int ev_fd) {
  Operation op{Operation::Type::DEL,
               create(ev_fd, Events::Undefined, [](const Event*) {})};
  list_.push_back(op);
  wakeup();
}

void Epoller::run(int timeout) {
  int nfds = epoll_wait(fd_, events_.data(), static_cast<int>(events_.size()),
                        timeout);
  for (int i = 0; i < nfds; ++i) {
    int fd = events_[i].data.fd;
    auto ev = maps_.at(fd);
    ev->handler_(ev.get());
  }
}

int64_t Epoller::addTimer(int64_t microseconds,
                          const Event::Handler& handler,
                          bool single_shot) {
  auto p = std::make_shared<TimerEvent>();
  p->id_ = timer_counter_.fetch_add(1);
  p->single_shot_ = single_shot;
  p->timeout_ = microseconds;
  p->handler_ = handler;
  p->expire_ = std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::steady_clock::now().time_since_epoch())
                   .count();
  p->type_ = static_cast<int>(EventType::Timer);

  Operation op{Operation::Type::ADD, p};
  list_.push_back(op);
  wakeup();
  return p->id_;
}

void Epoller::rmTimer(int64_t timer_id) {
  auto p = std::make_shared<TimerEvent>();
  p->id_ = timer_id;
  Operation op{Operation::Type::DEL, p};
  list_.push_back(op);
  wakeup();
}

void Epoller::wakeup() const {
  uint64_t one = 1;
  auto size = ::write(wake_fd_, &one, sizeof(one));
  UNUSED(size);
}

EventPtr Epoller::create(int fd, Events events, const Event::Handler& handler) {
  auto ret = std::make_shared<IOEvent>();
  ret->fd_ = fd;
  ret->event_ = events;
  ret->status_ = EventStatus::NotReady;
  ret->type_ = static_cast<int>(EventType::IO);
  if (static_cast<int>(ret->event_) & 0x04) {
    std::weak_ptr<IOEvent> ev = ret;
    ret->handler_ = [ev, handler](const Event*) {
      auto p = ev.lock();
      if (p) {
        Epoller::consume(p->fd_);
      }
      handler(p.get());
    };
  } else {
    ret->handler_ = handler;
  }

  return ret;
}

void Epoller::consume(int fd) {
  uint64_t one;
  auto size = ::read(fd, &one, sizeof(one));
  UNUSED(size);
}

void Epoller::handle() {
  std::list<Operation> list = list_.take();

  for (auto& l : list) {
    switch (l.type_) {
      case Operation::Type::ADD: {
        switch (static_cast<EventType>(l.event_->type_)) {
          case EventType::IO:
            addIO(std::static_pointer_cast<IOEvent>(l.event_));
            break;
          case EventType::Timer:
            addTimer(std::static_pointer_cast<TimerEvent>(l.event_));
            break;
          default:
            break;
        }
        l.event_->status_ = EventStatus::Listen;
      } break;

      case Operation::Type::DEL: {
        switch (static_cast<EventType>(l.event_->type_)) {
          case EventType::IO:
            rmIO(std::static_pointer_cast<IOEvent>(l.event_));
            break;
          case EventType::Timer:
            rmTimer(std::static_pointer_cast<TimerEvent>(l.event_));
            break;
          default:
            break;
        }
        l.event_->status_ = EventStatus::NotReady;
      } break;

      default:
        abort();
    }
  }

  std::size_t new_size = std::max(maps_.size(), events_.size());
  events_.resize(new_size);

  resetTimer();
}

void Epoller::addIO(const std::shared_ptr<IOEvent>& io) {
  struct epoll_event ev;
  memset(&ev, 0, sizeof(struct epoll_event));
  auto events = static_cast<int>(io->event_);
  if (events & 0x01) {
    ev.events |= EPOLLIN;
  }
  if (events & 0x02) {
    ev.events |= EPOLLOUT;
  }
  ev.data.fd = io->fd_;
  int type = maps_.count(io->fd_) == 1 ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
  maps_.erase(io->fd_);
  maps_.emplace(io->fd_, io);
  fassert(-1 != epoll_ctl(fd_, type, ev.data.fd, &ev));
}

void Epoller::rmIO(const std::shared_ptr<IOEvent>& io) {
  struct epoll_event ev;
  memset(&ev, 0, sizeof(struct epoll_event));
  auto events = static_cast<int>(io->event_);
  if (events & 0x01) {
    ev.events |= EPOLLIN;
  }
  if (events & 0x02) {
    ev.events |= EPOLLOUT;
  }
  ev.data.fd = io->fd_;
  if (maps_.count(io->fd_) == 0) {
    fassert(-1 != epoll_ctl(fd_, EPOLL_CTL_DEL, io->fd_, &ev));
    maps_.erase(io->fd_);
  }
  std::cout << "rm : " << io->fd_ << std::endl;
}

void Epoller::addTimer(const std::shared_ptr<TimerEvent>& timer) {
  sortTimer(timer);
}

void Epoller::rmTimer(const std::shared_ptr<TimerEvent>& timer) {
  auto iter = std::find_if(timer_sequence_.begin(), timer_sequence_.end(),
                           [timer](const std::shared_ptr<TimerEvent>& e) {
                             return e->id_ == timer->id_;
                           });
  if (iter != timer_sequence_.end()) {
    timer_sequence_.erase(iter);
  }
}

void Epoller::handleTimer() {
  auto cur_ts = std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::steady_clock::now().time_since_epoch())
                    .count();

  std::list<std::shared_ptr<TimerEvent>> timers;

  for (auto iter = timer_sequence_.begin(); iter != timer_sequence_.end();
       iter = timer_sequence_.erase(iter)) {
    auto& p = *iter;
    if (p->expire_ > cur_ts) {
      break;
    }
    timers.emplace_back(p);
  }

  for (auto const& p : timers) {
    p->handler_(p.get());
    if (!p->single_shot_) {
      sortTimer(p);
    }
  }

  resetTimer();
}

void Epoller::sortTimer(const std::shared_ptr<TimerEvent>& timer) {
  timer->expire_ += timer->timeout_;
  auto iter = timer_sequence_.begin();
  for (; iter != timer_sequence_.end(); ++iter) {
    if ((*iter)->expire_ > timer->expire_) {
      break;
    }
  }
  timer_sequence_.insert(iter, timer);
}

void Epoller::resetTimer() {
  struct itimerspec spec;
  memset(&spec, 0, sizeof(struct itimerspec));

  if (!timer_sequence_.empty()) {
    auto cost = timer_sequence_.front()->expire_ -
                std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::steady_clock::now().time_since_epoch())
                    .count();
    int64_t nanoseconds = cost > 0 ? cost * 1000 : 1;
    spec.it_value.tv_sec = nanoseconds / 1000000000;
    spec.it_value.tv_nsec = nanoseconds % 1000000000;
    fassert(-1 != timerfd_settime(timer_fd_, 0, &spec, nullptr));
  }
}

std::shared_ptr<Poller> makePoller() {
  return std::make_shared<Epoller>();
}

}  // namespace core