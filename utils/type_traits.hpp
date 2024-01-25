#pragma once

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

// template <typename T>
// struct map_type {};

// template <typename... T>
// struct map_type<std::map<T...>> {
//   using key_t = typename type_expansion<T...>::first_t;
//   using value_t = typename type_expansion<T...>::left_expansion::first_t;
// };

// template<typename T>
// struct vector_type {};

}  // namespace phoenix