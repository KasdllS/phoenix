#pragma once

#include <atomic>
#include <functional>
#include <memory>

namespace core {

class Thread;

enum class Events : uint8_t {
  Undefined,
  ReadOnly = 0x01,
  WriteOnly = 0x02,
  ReadWrite = 0x03,
  Execute = 0x05,
};

enum class EventStatus : uint8_t {
  NotReady = 0,
  Listen = 1,
  Triggering = 2,
  Moving = 3,
};

enum class EventType : uint8_t {
  IO = 0,
  Timer = 1,
};

struct Event {
  using Handler = std::function<void(const Event*)>;
  Handler handler_;
  int type_;
  std::atomic<EventStatus> status_;
  Thread const* thd_;
};

struct TimerEvent : public Event {
  int id_;
  int64_t timeout_;
  bool single_shot_;
  int64_t expire_;
};

struct IOEvent : public Event {
  int fd_;
  Events event_;
};

using EventPtr = std::shared_ptr<Event>;

class Trigger {
 public:
  ~Trigger();

  bool isValid() const;

  int fd() const;

  void trigger() const;

  void moveToThread(Thread const* thd);

 private:
  explicit Trigger(const EventPtr& ev);

  std::weak_ptr<IOEvent> pimpl_;

  friend class Thread;
};

}  // namespace core