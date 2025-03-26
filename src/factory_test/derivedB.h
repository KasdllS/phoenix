#pragma once

#include "factory.h"

class DerivedB : public Base, public BaseRegister<DerivedB> {
 public:
  constexpr static char type[] = "B";
  static std::shared_ptr<Base> create() { return std::make_shared<DerivedB>(); }
  DerivedB() = default;
};