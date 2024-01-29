#pragma once

/**
 * @file meta.h
 * @author sqi
 * @brief
 * @version 0.1
 * @date 2024-01-24
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <iostream>
#include <limits>
#include <map>
#include <string>

#include "utils/type_traits.hpp"

namespace phoenix {

template <typename T, typename Enable = void>
struct TypeCheck {};

struct Meta {
  enum class Type {
    Bool,
    Enum,
    Integer,
    Float,
    String,
  };

  Meta(const Type type, bool editable)
      : type_(type), editable_(editable), is_ref_(false) {}

  Type type_;
  bool editable_;
  bool is_ref_;
};

template <typename T, typename Enable = void>
struct MetaImpl {};

constexpr static bool default_editable = true;

template <typename T>
struct MetaImpl<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, bool>>>
    : public Meta {
  MetaImpl(T val, bool editable = default_editable)
      : Meta(Meta::Type::Bool, editable), val_(val) {
    is_ref_ = std::is_reference_v<T>;
  }

  T val_;
};

template <typename T>
struct MetaImpl<T, std::enable_if_t<std::is_enum_v<std::decay_t<T>>>>
    : public Meta {
  using Type =
      std::conditional_t<std::is_lvalue_reference_v<T>, int64_t&, int64_t>;
  MetaImpl(Type val, const std::map<int64_t, std::string>& combo, bool editable)
      : Meta(Meta::Type::Enum, editable), combo_(combo), val_(val) {
    is_ref_ = std::is_reference_v<Type>;
  }

  Type val_;
  const std::map<int64_t, std::string> combo_;
};

template <typename T>
struct MetaImpl<T, std::enable_if_t<is_numeric_v<std::decay_t<T>>>>
    : public Meta {
  using IntType = std::conditional_t<std::is_signed_v<T>, int64_t, uint64_t>;
  using FloatType = double;
  using ParamType =
      std::conditional_t<is_integer_v<std::decay_t<T>>, IntType, FloatType>;
  using ValueType =
      std::conditional_t<std::is_lvalue_reference_v<T>, T, ParamType>;

  constexpr static Meta::Type type =
      is_integer_v<std::decay_t<T>> ? Meta::Type::Integer : Meta::Type::Float;
  constexpr static ParamType default_step =
      std::is_integral_v<std::decay_t<T>> ? 1 : 0.1;

  MetaImpl(ValueType val, ParamType min = std::numeric_limits<ParamType>::min(),
           ParamType max = std::numeric_limits<ParamType>::max(),
           ParamType step = default_step, bool editable = default_editable)
      : Meta(type, editable), val_(val), min_(min), max_(max), step_(step) {
    is_ref_ = std::is_reference_v<ValueType>;
  }

  ValueType val_;
  ParamType min_, max_, step_;
};

template <typename T>
struct MetaImpl<T, std::enable_if_t<is_string_v<std::decay_t<T>>>>
    : public Meta {
  using TarType = typename type_cvt<std::decay_t<T>>::dst_t;
  using ParamType = std::conditional_t<
      std::is_lvalue_reference_v<T>, T,
      std::add_lvalue_reference_t<std::add_const_t<TarType>>>;
  using ValueType =
      std::conditional_t<std::is_lvalue_reference_v<T>,
                         std::add_lvalue_reference_t<TarType>, TarType>;

  MetaImpl(ParamType val, bool editable = default_editable)
      : Meta(Meta::Type::String, editable), val_(val) {
    is_ref_ = std::is_reference_v<ValueType>;
  }

  ValueType val_;
};

}  // namespace phoenix