#pragma once

#include "factory.h"

class DerivedA : public Base, public BaseRegister<DerivedA> {
 public:
  constexpr static char type[] = "A";
  static std::shared_ptr<Base> create() { return std::make_shared<DerivedA>(); }
  DerivedA() = default;
};