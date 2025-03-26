#include "core/event.h"

#include "utils/assert.h"
#include "utils/macros.hpp"

#include <sys/eventfd.h>
#include <unistd.h>

#include "core/thread.h"

namespace core {

Trigger::Trigger(const EventPtr& ev)
    : pimpl_(std::static_pointer_cast<IOEvent>(ev)) {}

Trigger::~Trigger() {
  std::cout << "~Trigger" << std::endl;
  auto p = pimpl_.lock();
  if (p) {
    p->thd_->removeEvent(p->fd_);
  }
}

bool Trigger::isValid() const {
  auto p = pimpl_.lock();
  return p && p->fd_ != -1;
}

int Trigger::fd() const {
  auto p = pimpl_.lock();
  return p ? p->fd_ : -1;
}

void Trigger::trigger() const {
  //
  auto cur_thread = Thread::this_thread();
  std::shared_ptr<IOEvent> p;
  EventStatus status = EventStatus::Listen;
  while (p = pimpl_.lock()) {
    if (p->status_.compare_exchange_weak(status, EventStatus::Triggering)) {
      break;
    }
    p = nullptr;
    status = EventStatus::Listen;
    // 在事件没有被添加到监听队列中时，循环等待其被加入队列。
    // 等待期间为保证不阻塞当前线程，执行事件循环
    cur_thread->processEvents(1);
  }

  uint64_t one = 1;
  auto size = ::write(p->fd_, &one, sizeof(one));
  UNUSED(size);
  p->status_ = status;
}

void Trigger::moveToThread(Thread const* thd) {
  auto p = pimpl_.lock();
  if (!p || p->thd_ == thd) {
    return;
  }

  auto cur_thd = Thread::this_thread();
  p->thd_->removeEvent(p->fd_);
  auto status = EventStatus::NotReady;
  while (!p->status_.compare_exchange_weak(status, EventStatus::Moving)) {
    cur_thd->processEvents(1);
  }

  pimpl_ = thd->addEvent(p->fd_, p->event_, p->handler_).pimpl_;
}

}  // namespace core