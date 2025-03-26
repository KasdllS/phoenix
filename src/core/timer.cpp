#include "core/timer.h"

#include "core/event.h"
#include "core/thread.h"

#include <sys/timerfd.h>
#include <unistd.h>

#include "utils/assert.h"

namespace core {

Timer::Timer(const Event::Handler& handler, Object* parent)
    : Object(parent), timer_id_(-1), handler_(handler) {}

Timer::~Timer() {
  stop();
}

void Timer::start(int64_t microseconds) {
  if (isRunning()) {
    return;
  }
  interval_ = microseconds;
  timer_id_ = thread()->addTimer(interval_, handler_, false);
}

void Timer::stop() {
  if (timer_id_ > -1) {
    thread()->removeTimer(timer_id_);
    timer_id_ = -1;
  }
}

void Timer::singleshot(int64_t microseconds) const {
  thread()->addTimer(microseconds, handler_, true);
}

bool Timer::isRunning() const {
  return timer_id_ > -1;
}

int64_t Timer::interval() const {
  return interval_;
}

void Timer::moveToThread(Thread* thd) {
  if (timer_id_ > -1) {
    thread()->removeTimer(timer_id_);
    timer_id_ = thd->addTimer(interval_, handler_, false);
  }
  Object::moveToThread(thd);
}

}  // namespace core