#pragma once

#include <limits>
#include <map>
#include <string>

#include "utils/type_traits.hpp"

namespace phoenix {

struct Meta {
  enum class Type {
    Bool,
    Enum,
    Integer,
    Float,
    String,
  };

  Meta(const Type type, bool editable) : type_(type), editable_(editable) {}

  Type type_;
  bool editable_;
};

template <typename T, typename Enable = void>
struct MetaImpl {};

constexpr static bool default_editable = true;

template <typename T>
struct MetaImpl<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, bool>>>
    : public Meta {
  MetaImpl(T val, bool editable = default_editable)
      : Meta(Meta::Type::Bool, editable), val_(val) {}

  T val_;
};

template <typename T>
struct MetaImpl<T, std::enable_if_t<std::is_enum_v<std::decay_t<T>>>>
    : public Meta {
 private:
  using Type = std::conditional_t<std::is_reference_v<T>, int64_t&, int64_t>;

 public:
  MetaImpl(Type val, const std::map<int64_t, std::string>& combo, bool editable)
      : Meta(Meta::Type::Enum, editable), combo_(combo), val_(val) {}

  Type val_;
  const std::map<int64_t, std::string> combo_;
};

template <class T>
struct is_numeric {
  constexpr static bool value =
      (std::is_integral_v<T> ||
       std::is_floating_point_v<T>)&&!std::is_same_v<std::decay_t<T>, bool>;
};
template <class T>
inline constexpr bool is_numeric_v = is_numeric<T>::value;

template <typename T>
struct MetaImpl<T, std::enable_if_t<is_numeric_v<std::decay_t<T>>>>
    : public Meta {
 private:
  constexpr static Meta::Type type = std::is_integral_v<std::decay_t<T>>
                                         ? Meta::Type::Integer
                                         : Meta::Type::Float;
  using Type =
      std::conditional_t<std::is_reference_v<T>, std::remove_reference_t<T>, T>;
  constexpr static Type default_step =
      std::is_integral_v<std::decay_t<T>> ? 1 : 0.1;

 public:
  MetaImpl(T val, Type min = std::numeric_limits<Type>::min(),
           Type max = std::numeric_limits<Type>::max(),
           Type step = default_step, bool editable = default_editable)
      : Meta(type, editable), val_(val), min_(min), max_(max), step_(step) {}

  T val_;
  Type min_, max_, step_;
};

template <typename T>
struct MetaImpl<T, std::enable_if_t<is_string_v<std::decay_t<T>>>>
    : public Meta {
 private:
  using TarType = typename type_cvt<std::decay_t<T>>::dst_t;
  using Type = std::conditional_t<
      std::is_lvalue_reference_v<T>, T,
      std::add_lvalue_reference_t<std::add_const_t<TarType>>>;

 public:
  MetaImpl(Type val, bool editable = default_editable)
      : Meta(Meta::Type::String, editable), val_(val) {}

  std::conditional_t<std::is_lvalue_reference_v<T>,
                     std::add_lvalue_reference_t<TarType>, TarType>
      val_;
};

}  // namespace phoenix