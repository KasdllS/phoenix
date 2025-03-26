#pragma once

#include <memory>
#include <string>
#include <typeinfo>

#include "core/event.h"
#include "utils/type_traits.hpp"

namespace thread {
class Thread;
}

namespace core {

class Object : public std::enable_shared_from_this<Object> {
 public:
  using Ptr = std::shared_ptr<Object>;
  using WPtr = std::weak_ptr<Object>;

  explicit Object(Object* parent = nullptr);
  virtual ~Object();

  const std::type_info& type_info() {
    // 获取对象的虚表指针，进而获取虚表中的typeinfo指针
    /**
     * @brief
     * class A
     *                      |   0(64 bits)  |
     *                      |   typeinfo    |
     * |    vtable+16   |-> |       ~A      |
     * |        mem     |   |     vfuncs    |
     */
    void** vtable = *reinterpret_cast<void***>(this);
    const std::type_info* type =
        reinterpret_cast<const std::type_info*>(vtable[-1]);
    return *type;
  }

  std::string type() { return type_traits::demangle(type_info().name()); }

  Thread const* thread() const { return thd_; }

  void setParent(Object*);

  virtual void moveToThread(Thread*);

 protected:
  void addChild(Object*);
  void removeChild(Object*);

 private:
  Object* parent_;
  std::list<Object*> children_;

  Thread const* thd_;
};

}  // namespace core