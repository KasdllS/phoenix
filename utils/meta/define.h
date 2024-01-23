#pragma once

#include <limits>
#include <map>
#include <string>

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

template <typename T>
struct MetaImpl<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, bool>>>
    : public Meta {
  MetaImpl(T val, bool editable)
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
  constexpr static Meta::Type type =
      std::is_integral_v<T> ? Meta::Type::Integer : Meta::Type::Float;
  using Type =
      std::conditional_t<std::is_reference_v<T>, std::remove_reference_t<T>, T>;

 public:
  MetaImpl(T val, Type min, Type max, Type step, bool editable)
      : Meta(type, editable), val_(val), min_(min), max_(max), step_(step) {}

  T val_;
  Type min_, max_, step_;
};

template <typename T>
struct MetaImpl<T,
                std::enable_if_t<std::is_same_v<std::decay_t<T>, std::string>>>
    : public Meta {
 private:
  using Type =
      std::conditional_t<std::is_reference_v<T>, T,
                         std::add_lvalue_reference_t<std::add_const_t<T>>>;

 public:
  MetaImpl(Type val, bool editable)
      : Meta(Meta::Type::String, editable), val_(val) {}

  T val_;
};

}  // namespace phoenix