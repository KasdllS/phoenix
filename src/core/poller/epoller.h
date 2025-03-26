#pragma once

#include <atomic>
#include <functional>
#include <thread>

#include <sys/epoll.h>

#include "core/poller.h"

#include "utils/thread/annotations.hpp"
#include "utils/thread/list.hpp"
#include "utils/thread/thread_local_storage.hpp"

namespace core {

class Epoller : public Poller {
  struct Operation {
    enum class Type {
      DEL = -1,
      ADD = 1,
    };
    Type type_;
    EventPtr event_;
  };

 public:
  Epoller();

  EventPtr addEvent(int fd,
                    Events events,
                    const Event::Handler& handler) override;
  EventPtr addEvent(Events events, const Event::Handler& handler) override;
  void rmEvent(int ev_fd) override;
  void wakeup() const override;

  void run(int timeout = -1) override;

  int64_t addTimer(int64_t microseconds,
                   const Event::Handler& handler,
                   bool single_shot) override;
  void rmTimer(int64_t timer_id) override;

 private:
  void handle();

  void addIO(const std::shared_ptr<IOEvent>& io);
  void rmIO(const std::shared_ptr<IOEvent>& io);

  void addTimer(const std::shared_ptr<TimerEvent>& timer);
  void rmTimer(const std::shared_ptr<TimerEvent>& timer);

  void handleTimer();
  void sortTimer(const std::shared_ptr<TimerEvent>& timer);
  void resetTimer();

  static EventPtr create(int fd, Events events, const Event::Handler& handler);
  static void consume(int fd);

  int fd_ = -1;
  int wake_fd_;
  int timer_fd_;

  utils::thread::list<Operation> list_;

  std::unordered_map<int, std::shared_ptr<IOEvent>> maps_;
  std::vector<struct epoll_event> events_;

  std::list<std::shared_ptr<TimerEvent>> timer_sequence_;

  inline static std::atomic<int64_t> timer_counter_;
};

}  // namespace core