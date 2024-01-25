#pragma once

/**
 * @file node.h
 * @author sqi
 * @brief
 * @version 0.1
 * @date 2024-01-24
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <assert.h>

#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "utils/meta/define.h"
#include "utils/type_traits.hpp"

namespace phoenix {

template <typename T, typename U = std::decay_t<T>>
using type_check_t =
    std::enable_if_t<std::is_integral_v<U> || std::is_floating_point_v<U> ||
                         std::is_same_v<std::string, U> ||
                         std::is_same_v<const char*, U>,
                     bool>;

template <typename T>
using sign_t = std::conditional_t<std::is_signed_v<T>, int64_t, uint64_t>;

struct Elem {
  virtual ~Elem() = default;
  int type_;
};

class Node {
 private:
  enum class ElemType {
    Value,
    Vector,
    Map,
  };

 public:
  Node();

  template <typename T, type_check_t<T> = true>
  Node(T val);

  bool isValue() const;
  bool isMap() const;
  bool isVector() const;

  bool isValid() const;

  template <typename T, type_check_t<T> = true>
  Node& operator=(T val);

  template <
      typename T,
      std::enable_if_t<is_map_v<T> &&
                           std::is_same_v<typename T::key_type, std::string> &&
                           std::is_same_v<typename T::value_type, Node>,
                       bool> = true>
  Node& operator=(const T& vals);

  template <
      typename T,
      std::enable_if_t<is_map_v<T> &&
                           std::is_same_v<typename T::key_type, std::string> &&
                           !std::is_same_v<typename T::value_type, Node>,
                       bool> = true>
  Node& operator=(const T& vals);

  template <typename T,
            std::enable_if_t<is_vector_v<T> &&
                                 !std::is_same_v<typename T::value_type, Node>,
                             bool> = true>
  Node& operator=(const T& vals);

  template <typename T,
            std::enable_if_t<is_vector_v<T> &&
                                 std::is_same_v<typename T::value_type, Node>,
                             bool> = true>
  Node& operator=(const T& vals);

  Node& operator[](const std::string& key);
  Node& operator[](const std::size_t idx);

  template <typename T>
  T as() const;

  std::string name_;

 private:
  bool delete_if(const ElemType type);

  template <typename T>
  bool check_type(const Meta::Type type) {
    using check_type_t = std::decay_t<T>;
    bool ret = false;
    switch (type) {
      case Meta::Type::Bool:
        ret = is_bool_v<check_type_t>;
        break;
      case Meta::Type::Float:
        ret = std::is_floating_point_v<check_type_t>;
        break;
      case Meta::Type::Integer:
        ret = is_integer_v<check_type_t>;
        break;
      case Meta::Type::String:
        ret = is_string_v<check_type_t>;
        break;

      default:
        break;
    }

    return false;
  }

  std::shared_ptr<Elem> elem_;
};

struct ElemValue : public Elem {
  ElemValue() : meta_(nullptr) {}
  ~ElemValue() override {
    delete meta_;
    meta_ = nullptr;
  }

  Meta* meta_;
};

struct ElemMap : public Elem {
  std::map<std::string, Node, std::less<>> nodes_;
};

struct ElemVec : public Elem {
  std::vector<Node> nodes_;
};

template <typename T, type_check_t<T> = true>
Node::Node(T val) {
  using MetaType = MetaImpl<typename type_cvt<std::decay_t<T>>::dst_t>;

  elem_ = std::make_shared<ElemValue>();
  elem_->type_ = static_cast<int>(ElemType::Value);
  auto elem = std::dynamic_pointer_cast<ElemValue>(elem_);
  elem->meta_ = new MetaType(val);
}

template <typename T, type_check_t<T> = true>
Node& Node::operator=(T val) {
  using MetaType = MetaImpl<typename type_cvt<std::decay_t<T>>::dst_t>;

  if (delete_if(ElemType::Value)) {
    elem_ = std::make_shared<ElemValue>();
    elem_->type_ = static_cast<int>(ElemType::Value);
  }
  auto& meta = std::dynamic_pointer_cast<ElemValue>(elem_)->meta_;
  if (!meta) {
    std::cout << val << std::endl;
    meta = new MetaType(val);
  } else if (!check_type<T>(meta->type_)) {
    delete meta;
    meta = new MetaType(val);
  } else {
    ((MetaType*)meta)->val_ = val;
  }
  return *this;
}

template <
    typename T,
    std::enable_if_t<is_map_v<T> &&
                         std::is_same_v<typename T::key_type, std::string> &&
                         std::is_same_v<typename T::value_type, Node>,
                     bool> = true>
Node& Node::operator=(const T& vals) {
  if (delete_if(ElemType::Map)) {
    elem_ = std::make_shared<ElemMap>();
    elem_->type_ = static_cast<int>(ElemType::Map);
  }
  auto elem = std::dynamic_pointer_cast<ElemMap>(elem_);
  elem->nodes_.clear();
  for (auto iter = std::begin(vals); iter != std::end(vals); ++iter) {
    elem->nodes_.emplace(iter->first, iter->second);
  }
  return *this;
}

template <
    typename T,
    std::enable_if_t<is_map_v<T> &&
                         std::is_same_v<typename T::key_type, std::string> &&
                         !std::is_same_v<typename T::value_type, Node>,
                     bool> = true>
Node& Node::operator=(const T& vals) {
  if (delete_if(ElemType::Map)) {
    elem_ = std::make_shared<ElemMap>();
    elem_->type_ = static_cast<int>(ElemType::Map);
  }
  auto elem = std::dynamic_pointer_cast<ElemMap>(elem_);
  elem->nodes_.clear();
  for (auto iter = std::begin(vals); iter != std::end(vals); ++iter) {
    elem->nodes_.emplace(iter->first, Node(iter->second));
  }
  return *this;
}

template <typename T,
          std::enable_if_t<is_vector_v<T> &&
                               !std::is_same_v<typename T::value_type, Node>,
                           bool> = true>
Node& Node::operator=(const T& vals) {
  if (delete_if(ElemType::Vector)) {
    elem_ = std::make_shared<ElemVec>();
    elem_->type_ = static_cast<int>(ElemType::Vector);
  }
  auto elem = std::dynamic_pointer_cast<ElemVec>(elem_);
  elem->nodes_.clear();
  for (auto iter = std::begin(vals); iter != std::end(vals); ++iter) {
    elem->nodes_.emplace_back(Node(*iter));
  }
  return *this;
}

template <typename T,
          std::enable_if_t<is_vector_v<T> &&
                               std::is_same_v<typename T::value_type, Node>,
                           bool> = true>
Node& Node::operator=(const T& vals) {
  if (delete_if(ElemType::Vector)) {
    elem_ = std::make_shared<ElemVec>();
    elem_->type_ = static_cast<int>(ElemType::Vector);
  }
  auto elem = std::dynamic_pointer_cast<ElemVec>(elem_);
  elem->nodes_.clear();
  for (auto iter = std::begin(vals); iter != std::end(vals); ++iter) {
    elem->nodes_.emplace_back(*iter);
  }
  return *this;
}

template <typename T>
T Node::as() const {
  using MetaType = MetaImpl<typename type_cvt<std::decay_t<T>>::dst_t>;

  assert(isValid());
  auto elem = std::dynamic_pointer_cast<ElemValue>(elem_);
  return ((MetaType*)(elem->meta_))->val_;
}

}  // namespace phoenix