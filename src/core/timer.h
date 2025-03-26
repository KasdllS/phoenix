#pragma once

#include <functional>
#include <memory>

#include "core/event.h"
#include "core/object.h"

namespace core {

class Timer : public core::Object {
 public:
  Timer(const Event::Handler& handler, Object* parent = nullptr);
  ~Timer() override;

  void start(int64_t microseconds);
  void stop();
  void singleshot(int64_t microseconds) const;

  bool isRunning() const;
  int64_t interval() const;

  void moveToThread(Thread*) override;

 private:
  int64_t timer_id_ = -1;
  Event::Handler handler_;
  int64_t interval_;
};

}  // namespace core