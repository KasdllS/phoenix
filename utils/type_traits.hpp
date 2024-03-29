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

#include <map>
#include <vector>

namespace phoenix {

template <typename T>
constexpr bool is_vector_v = false;

template <typename... T>
constexpr bool is_vector_v<std::vector<T...>> = true;

template <typename T>
constexpr bool is_map_v = false;

template <typename... T>
constexpr bool is_map_v<std::map<T...>> = true;

template <typename T>
constexpr bool is_bool_v = std::is_same_v<bool, T>;

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

}  // namespace phoenix