#pragma once

#include <memory>
#include "core/event.h"

namespace core {

class Poller {
 public:
  Poller() = default;
  virtual ~Poller() = default;

  virtual EventPtr addEvent(int fd,
                            Events events,
                            const Event::Handler& handler) = 0;
  virtual EventPtr addEvent(Events events, const Event::Handler& handler) = 0;
  virtual void rmEvent(int fd) = 0;

  virtual void wakeup() const = 0;
  virtual void run(int timeout = -1) = 0;

  virtual int64_t addTimer(int64_t microseconds,
                           const Event::Handler& handler,
                           bool single_shot) = 0;
  virtual void rmTimer(int64_t timer_id) = 0;
};

std::shared_ptr<Poller> makePoller();

}  // namespace core