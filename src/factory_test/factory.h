#pragma once

#include "core/object.h"
#include "utils/factory/class_factory.hpp"
#include "utils/factory/class_register.hpp"

class Base : public core::Object {
 protected:
  Base() = default;
  virtual ~Base() = default;
};

using BaseFactory = FactorySingleTon<std::string, Base>;

template <class Derived>
using BaseRegister =
    ClassRegister<Base, Derived, Register<BaseFactory, Derived>>;