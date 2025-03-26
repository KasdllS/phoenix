#pragma once

#include <assert.h>

#include <type_traits>

template <typename T>
class SingleTon : private T {
 public:
  using InstT = T;

  static T& instance() {
    static_assert(std::is_copy_constructible_v<T> == false &&
                      std::is_move_constructible_v<T> == false &&
                      std::is_copy_assignable_v<T> == false &&
                      std::is_move_assignable_v<T> == false,
                  "single ton must be uncopiable and unmoveable");

    static SingleTon<T> s_inst;
    return s_inst;
  }

 private:
  SingleTon() = default;
};