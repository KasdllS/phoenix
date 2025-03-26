#include "core/object.h"

#include "core/thread.h"

namespace core {

Object::Object(Object* parent) : parent_(nullptr), thd_(Thread::this_thread()) {
  setParent(parent);
}

Object::~Object() {
  if (parent_) {
    parent_->removeChild(this);
  }
}

void Object::setParent(Object* parent) {
  if (parent_) {
    parent_->removeChild(this);
    parent_->addChild(this);
  }

  parent_ = parent;
}

void Object::moveToThread(Thread* thd) {
  for (auto& child : children_) {
    child->moveToThread(thd);
  }
  thd_ = thd;
}

void Object::addChild(Object* child) {
  if (std::find(children_.begin(), children_.end(), child) == children_.end()) {
    children_.emplace_back(child);
  }
}

void Object::removeChild(Object* child) {
  if (auto iter = std::find(children_.begin(), children_.end(), child);
      iter != children_.end()) {
    children_.erase(iter);
  }
}

}  // namespace core