#pragma once

/**
 * @file type_traits.hpp
 * @author sqi
 * @brief
 * @version 0.1
 * @date 2024-01-25
 *
 * @copyright Copyright (c) 2024
 *
 * 一些C++17以上标准可能已经有了的元编程所需的组件
 */

#include <list>
#include <map>
#include <unordered_map>
#include <vector>

#include <cxxabi.h>
#include <type_traits>

template <typename T>
constexpr bool is_vector_v = false;

template <typename... T>
constexpr bool is_vector_v<std::vector<T...>> = true;

template <typename... T>
constexpr bool is_vector_v<std::list<T...>> = true;

template <typename T>
constexpr bool is_map_v = false;

template <typename... T>
constexpr bool is_map_v<std::map<T...>> = true;

template <typename... T>
constexpr bool is_map_v<std::unordered_map<T...>> = true;

template <typename T>
constexpr bool is_bool_v = std::is_same_v<bool, std::decay_t<T>>;

template <typename T>
constexpr bool is_integer_v = std::is_integral_v<T> && !std::is_same_v<T, bool>;

template <typename T>
constexpr bool is_numeric_v = is_integer_v<T> || std::is_floating_point_v<T>;

template <typename T>
constexpr bool is_string_v =
    std::is_same_v<const char*, T> || std::is_same_v<std::string, T>;

template <typename First, typename... T>
struct type_expansion {
  using first_t = First;
  using left_expansion = type_expansion<T...>;
};

template <typename First>
struct type_expansion<First> {
  using first_t = First;
};

template <typename T, typename Enable = void>
struct type_cvt {};

template <typename T>
struct type_cvt<T, std::enable_if_t<is_bool_v<T>>> {
  using src_t = T;
  using dst_t = bool;
};

template <typename T>
struct type_cvt<T, std::enable_if_t<is_integer_v<T>>> {
  using src_t = T;
  using dst_t = std::conditional_t<std::is_signed_v<T>, int64_t, uint64_t>;
};

template <typename T>
struct type_cvt<T, std::enable_if_t<std::is_floating_point_v<T>>> {
  using src_t = T;
  using dst_t = double;
};

template <typename T>
struct type_cvt<T, std::enable_if_t<is_string_v<T>>> {
  using src_t = T;
  using dst_t = std::string;
};

#define HAS_MEMBER(member)                                                    \
  template <typename T, typename = void>                                      \
  struct has_member_##member : std::false_type {};                            \
                                                                              \
  template <typename T>                                                       \
  struct has_member_##member<T,                                               \
                             std::void_t<decltype(std::declval<T>().member)>> \
      : std::true_type {};

#define HAS_FUNCTION(func)                                           \
  template <typename T>                                              \
  struct has_func_##func {                                           \
   private:                                                          \
    template <typename U>                                            \
    static auto test(int) -> decltype(&U::func(), std::true_type{}); \
                                                                     \
    template <typename>                                              \
    static std::false_type test(...);                                \
                                                                     \
   public:                                                           \
    static constexpr bool value = decltype(test<T>(0))::value;       \
  };

HAS_FUNCTION(operator)
// 判断是否是 lambda 表达式
template <typename T>
struct is_lambda
    : std::integral_constant<bool,
                             !std::is_function<T>::value &&
                                 !std::is_pointer<T>::value &&
                                 !std::is_member_pointer<T>::value &&
                                 has_func_operator<T>::value> {};

namespace type_traits {

inline std::string demangle(const std::string& mangled_name) {
  int status;
  return abi::__cxa_demangle(mangled_name.c_str(), NULL, NULL, &status);
}

}  // namespace type_traits