#pragma once

#include <tuple>

namespace utils {

template <typename R, typename... Args>
struct function_traits_helper {
  static constexpr auto param_count = sizeof...(Args);
  using return_type = R;

  template <std::size_t N>
  using param_type = std::tuple_element_t<N, std::tuple<Args...>>;
};

template <typename T>
struct function_traits;

template <typename R, typename... Args>
struct function_traits<R (*)(Args...)>
    : public function_traits_helper<R, Args...> {};

template <typename R, typename... Args>
struct function_traits<R (&)(Args...)>
    : public function_traits_helper<R, Args...> {};

template <typename R, typename... Args>
struct function_traits<R(Args...)> : public function_traits_helper<R, Args...> {
};

}  // namespace utils