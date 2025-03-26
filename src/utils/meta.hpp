#pragma once

/**
 * @file meta.hpp
 * @author sqi (kasdll@outlook.com)
 * @brief
 * 提供一个树形通用存储结构，节点可用setExtra、getExtra获取额外参数
 *
 * @version 0.1
 * @date 2024-10-31
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <float.h>
#include <any>
#include <limits>
#include <memory>
#include <ostream>
#include <vector>
#include "utils/assert.h"
#include "utils/string.h"
#include "utils/type_traits.hpp"

namespace utils {

struct Meta {
  using Ptr = std::shared_ptr<Meta>;
};

struct BoolMeta : public Meta {
  BoolMeta(bool val) : val_(val) {}

  bool val_;
};

template <typename T, typename Enable = void>
struct MetaImpl {};

template <typename T>
struct MetaImpl<T, std::enable_if_t<is_numeric_v<T>>> : public Meta {
  MetaImpl(T val) : val_(val) {}

  T val_;
};

using IntegerMeta = MetaImpl<int64_t>;
using FPointMeta = MetaImpl<double>;

struct StringMeta : public Meta {
  StringMeta(const std::string& val) : val_(val) {}

  std::string val_;
};

class Node {
 public:
  enum class Type {
    Unset = 0,
    Bool,
    Integer,
    Double,
    String,
    Array,
    Map,
  };

 private:
  using Ptr = std::shared_ptr<Node>;

  struct Memory {
    using Ptr = std::shared_ptr<Memory>;

    Type type_ = Type::Unset;
    Node::Ptr node_;
    std::string tag_;
    std::any extra_;
  };

  template <class T, class U>
  std::shared_ptr<T> Cast(const std::shared_ptr<U>& ptr) const {
    return std::static_pointer_cast<T>(ptr);
  }

  template <class T>
  std::shared_ptr<T> child() const {
    return std::static_pointer_cast<T>(child_->node_);
  }

 public:
  class Iterator {
    enum class Type {
      Array,
      Map,
    };

   public:
    Iterator() = default;

    Node& operator*();
    std::map<std::string, Node>::iterator& operator->();
    bool operator==(const Iterator&) const;
    bool operator!=(const Iterator&) const;
    Iterator& operator++();
    Iterator operator++(int);
    Iterator& operator=(const std::vector<Node>::iterator& it);
    Iterator& operator=(const std::map<std::string, Node>::iterator& it);

   private:
    Type type_;
    std::vector<Node>::iterator arr_iter_;
    std::map<std::string, Node>::iterator map_iter_;

    friend class Node;
  };

  Node();

  template <typename T,
            std::enable_if_t<is_numeric_v<T> || is_bool_v<T>, bool> = true>
  Node(T val);
  template <typename T, std::enable_if_t<is_string_v<T>, bool> = true>
  Node(const T& val);
  template <std::size_t N>
  Node(const char (&val)[N]);

  void merge(utils::Node& other, const std::string& tar = "/");

  bool isValid() const;

  Type type() const { return child_->type_; }

  bool isBool() const;
  bool isInteger() const;
  bool isDouble() const;
  bool isString() const;
  bool isArray() const;
  bool isMap() const;

  template <typename T,
            std::enable_if_t<is_numeric_v<T> || is_bool_v<T>, bool> = true>
  T as() const;
  template <typename T, std::enable_if_t<is_string_v<T>, bool> = true>
  T as() const;
  template <typename T, std::enable_if_t<is_vector_v<T>, bool> = true>
  T as() const;
  template <typename T,
            std::enable_if_t<is_map_v<T> && std::is_same_v<typename T::key_type,
                                                           std::string>,
                             bool> = true>
  T as() const;

  Iterator begin() const;
  Iterator end() const;

  Node& operator[](const std::string& name);
  const Node& operator[](const std::string& name) const;
  const Node& at(const std::string& name) const;
  bool hasMember(const std::string& name) const;
  void erase(const std::string& name);

  Node& operator[](const int idx);
  inline const Node& operator[](const int idx) const;
  const Node& at(const int idx) const;

  template <typename T, std::enable_if_t<is_bool_v<T>, bool> = true>
  Node& operator=(const T val);

  template <typename T, std::enable_if_t<is_integer_v<T>, bool> = true>
  Node& operator=(const T val);

  template <typename T,
            std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
  Node& operator=(const T val);

  template <typename T, std::enable_if_t<is_string_v<T>, bool> = true>
  Node& operator=(const T& val);

  template <std::size_t N>
  Node& operator=(const char (&arr)[N]);

  void setTag(const std::string& tag);
  std::string tag() const;

  template <
      typename T,
      std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>,
                       bool> = true>
  void push_back(const T val);

  template <typename T, std::enable_if_t<is_string_v<T>, bool> = true>
  void push_back(const T& val);

  template <std::size_t N>
  void push_back(const char (&arr)[N]);

  void push_back(const Node& n);

  template <
      typename T,
      std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>,
                       bool> = true>
  Node insert(const std::string& key, const T val);

  template <typename T, std::enable_if_t<is_string_v<T>, bool> = true>
  Node insert(const std::string& key, const T& val);

  template <std::size_t N>
  Node insert(const std::string& key, const char (&arr)[N]);

  Node insert(const std::string& key, const Node& n);

  std::size_t size() const;

  void clear();

  Node clone() const;

  Memory& operator*() { return *child_; }

  template <typename T>
  T extra() {
    fassert(typeid(T) == child_->extra_.type());
    return std::any_cast<T>(child_->extra_);
  }

  template <typename T>
  void setExtra(const T& data) {
    child_->extra_ = data;
  }

  template <typename T>
  bool getExtra(T& data) const {
    auto& extra = child_->extra_;
    if (extra.has_value() && typeid(T) == extra.type()) {
      data = std::any_cast<T>(extra);
      return true;
    }

    return false;
  }

 private:
  std::map<std::string, Node>::iterator mapBegin() const;
  std::map<std::string, Node>::iterator mapEnd() const;
  std::vector<Node>::iterator arrayBegin() const;
  std::vector<Node>::iterator arrayEnd() const;

  Memory::Ptr child_;
};

class ValueNode : public Node {
 public:
  ValueNode() = default;

  Meta::Ptr meta_;
};

class ArrayNode : public Node {
 public:
  ArrayNode() = default;

  std::vector<Node> nodes_;
};

class MapNode : public Node {
 public:
  MapNode() = default;

  std::map<std::string, Node> nodes_;
};

inline Node::Node() : child_(std::make_shared<Memory>()) {}

template <typename T, std::enable_if_t<is_numeric_v<T> || is_bool_v<T>, bool>>
Node::Node(T val) : child_(std::make_shared<Memory>()) {
  this->operator=(val);
}

template <typename T, std::enable_if_t<is_string_v<T>, bool>>
Node::Node(const T& val) : child_(std::make_shared<Memory>()) {
  this->operator=(val);
}

template <std::size_t N>
Node::Node(const char (&val)[N]) : child_(std::make_shared<Memory>()) {
  this->operator=(val);
}

static utils::Node find(const std::vector<std::string_view>& levels,
                        utils::Node& node) {
  //
  utils::Node ret = node;

  for (auto const& level : levels) {
    std::string tar(level);
    if (!ret.hasMember(tar)) {
      ret = utils::Node();
      break;
    }
    ret = ret[tar];
  }

  return ret;
}

static void merge(utils::Node& tar, utils::Node& src) {
  if (!src.isValid()) {
    return;
  }

  if (src.isMap()) {
    for (auto iter = src.begin(); iter != src.end(); ++iter) {
      merge(tar[iter->first], src[iter->first]);
    }
  } else {
    tar = src.clone();
  }
}

inline void Node::merge(utils::Node& other, const std::string& tar) {
  //
  auto levels = utils::strings::split(tar, "/");
  utils::Node root = find(levels, *this);
  if (!root.isValid()) {
    return;
  }
  ::utils::merge(root, other);
}

inline bool Node::isValid() const {
  return child_->type_ != Node::Type::Unset;
}

inline bool Node::isBool() const {
  return child_->type_ == Node::Type::Bool;
}

inline bool Node::isInteger() const {
  return child_->type_ == Node::Type::Integer;
}

inline bool Node::isDouble() const {
  return child_->type_ == Node::Type::Double;
}

inline bool Node::isString() const {
  return child_->type_ == Node::Type::String;
}

inline bool Node::isArray() const {
  return child_->type_ == Node::Type::Array;
}

inline bool Node::isMap() const {
  return child_->type_ == Node::Type::Map;
}

inline Node::Iterator Node::begin() const {
  Node::Iterator it;
  switch (child_->type_) {
    case Type::Array:
      it.type_ = Iterator::Type::Array;
      it = arrayBegin();
      break;

    case Type::Map:
      it.type_ = Iterator::Type::Map;
      it = mapBegin();
      break;

    default:
      it.type_ = Iterator::Type::Array;
      it.arr_iter_ = arrayEnd();
      break;
  }
  return it;
}

inline Node::Iterator Node::end() const {
  Node::Iterator it;
  switch (child_->type_) {
    case Type::Map:
      it.type_ = Iterator::Type::Map;
      it = mapEnd();
      break;

    default:
      it.type_ = Iterator::Type::Array;
      it.arr_iter_ = arrayEnd();
      break;
  }
  return it;
}

inline std::map<std::string, Node>::iterator Node::mapBegin() const {
  fassert(LIKELY(isMap()));
  return child<MapNode>()->nodes_.begin();
}

inline std::map<std::string, Node>::iterator Node::mapEnd() const {
  fassert(LIKELY(isMap()));
  return child<MapNode>()->nodes_.end();
}

inline Node& Node::operator[](const std::string& name) {
  if (child_->type_ != Type::Map) {
    child_->node_ = Node::Cast<Node>(std::make_shared<MapNode>());
    child_->type_ = Type::Map;
  }
  return child<MapNode>()->nodes_[name];
}

inline const Node& Node::operator[](const std::string& name) const {
  return at(name);
}

inline const Node& Node::at(const std::string& name) const {
  if (child_->type_ != Type::Map) {
    throw std::bad_cast();
  }
  return child<MapNode>()->nodes_.at(name);
}

inline bool Node::hasMember(const std::string& name) const {
  if (!isMap()) {
    return false;
  }
  return child<MapNode>()->nodes_.count(name) == 1;
}

inline void Node::erase(const std::string& name) {
  if (!isMap()) {
    return;
  }
  child<MapNode>()->nodes_.erase(name);
}

inline Node& Node::operator[](const int idx) {
  if (child_->type_ != Type::Array) {
    child_->node_ = Node::Cast<Node>(std::make_shared<ArrayNode>());
    child_->type_ = Type::Array;
  }
  auto n = child<ArrayNode>();
  fassert(idx < n->nodes_.size());
  return n->nodes_[idx];
}

inline const Node& Node::operator[](const int idx) const {
  return at(idx);
}

inline const Node& Node::at(const int idx) const {
  if (child_->type_ != Type::Array) {
    throw std::bad_cast();
  }
  auto n = child<ArrayNode>();
  fassert(idx < n->nodes_.size());
  return n->nodes_.at(idx);
}

inline std::vector<Node>::iterator Node::arrayBegin() const {
  fassert(LIKELY(isArray()));
  return child<ArrayNode>()->nodes_.begin();
}

inline std::vector<Node>::iterator Node::arrayEnd() const {
  fassert(LIKELY(isArray()));
  return child<ArrayNode>()->nodes_.end();
}

template <typename T,
          std::enable_if_t<is_numeric_v<T> || is_bool_v<T>, bool> = true>
T Node::as() const {
  switch (child_->type_) {
    case Node::Type::Bool:
      return std::static_pointer_cast<BoolMeta>(child<ValueNode>()->meta_)
          ->val_;

    case Node::Type::Integer:
      return std::static_pointer_cast<IntegerMeta>(child<ValueNode>()->meta_)
          ->val_;

    case Node::Type::Double:
      return std::static_pointer_cast<FPointMeta>(child<ValueNode>()->meta_)
          ->val_;

    case Node::Type::String: {
      auto str =
          std::static_pointer_cast<StringMeta>(child<ValueNode>()->meta_)->val_;
      if constexpr (std::is_floating_point_v<T>) {
        if constexpr (std::is_same_v<double, T>) {
          return std::stod(str);
        } else if constexpr (std::is_same_v<float, T>) {
          return std::stof(str);
        }
      } else if constexpr (std::is_integral_v<T>) {
        if constexpr (sizeof(T) == 8) {
          return std::stoll(str);
        } else {
          return std::stoi(str);
        }
      }

      throw std::invalid_argument("Cannot convert String to " +
                                  type_traits::demangle(typeid(T).name()));

    } break;

    default:
      break;
  }

  throw std::invalid_argument("Cannot convert Type " +
                              std::to_string(static_cast<int>(child_->type_)) +
                              " to " + type_traits::demangle(typeid(T).name()));
}

template <typename T, std::enable_if_t<is_string_v<T>, bool> = true>
T Node::as() const {
  switch (child_->type_) {
    case Node::Type::Bool:
      return std::to_string(
          std::static_pointer_cast<BoolMeta>(child<ValueNode>()->meta_)->val_);

    case Node::Type::Integer:
      return std::to_string(
          std::static_pointer_cast<IntegerMeta>(child<ValueNode>()->meta_)
              ->val_);

    case Node::Type::Double:
      return std::to_string(
          std::static_pointer_cast<FPointMeta>(child<ValueNode>()->meta_)
              ->val_);

    case Node::Type::String:
      return std::static_pointer_cast<StringMeta>(child<ValueNode>()->meta_)
          ->val_;

    default:
      break;
  }

  throw std::invalid_argument("Cannot convert Type " +
                              std::to_string(static_cast<int>(child_->type_)) +
                              " to " + type_traits::demangle(typeid(T).name()));
}

template <typename T, std::enable_if_t<is_vector_v<T>, bool> = true>
T Node::as() const {
  T ret;
  if (LIKELY(child_->type_ == Node::Type::Array)) {
    auto& nodes = child<ArrayNode>()->nodes_;
    for (auto iter = nodes.begin(); iter != nodes.end(); ++iter) {
      try {
        ret.emplace_back((*iter).as<typename T::value_type>());
      } catch (...) {
      }
    }
  }

  return ret;
}

template <typename T,
          std::enable_if_t<
              is_map_v<T> && std::is_same_v<typename T::key_type, std::string>,
              bool> = true>
T Node::as() const {
  T ret;
  if (LIKELY(child_->type_ == Node::Type::Map)) {
    auto& nodes = child<MapNode>()->nodes_;
    for (auto iter = nodes.begin(); iter != nodes.end(); ++iter) {
      try {
        ret.emplace(iter->first, iter->second.as<typename T::mapped_type>());
      } catch (...) {
      }
    }
  }

  return ret;
}

template <typename T, std::enable_if_t<is_bool_v<T>, bool> = true>
Node& Node::operator=(const T val) {
  if (child_->type_ != Type::Bool) {
    auto p = std::make_shared<ValueNode>();
    p->meta_ = std::make_shared<BoolMeta>(false);
    child_->node_ = Node::Cast<Node>(p);
    child_->type_ = Type::Bool;
  }
  std::static_pointer_cast<BoolMeta>(child<ValueNode>()->meta_)->val_ = val;
  return *this;
}

template <typename T, std::enable_if_t<is_integer_v<T>, bool> = true>
Node& Node::operator=(const T val) {
  if (child_->type_ != Type::Integer) {
    auto p = std::make_shared<ValueNode>();
    p->meta_ = std::make_shared<IntegerMeta>(0);
    child_->node_ = Node::Cast<Node>(p);
    child_->type_ = Type::Integer;
  }
  std::static_pointer_cast<IntegerMeta>(child<ValueNode>()->meta_)->val_ = val;
  return *this;
}

template <typename T,
          std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
Node& Node::operator=(const T val) {
  if (child_->type_ != Type::Double) {
    auto p = std::make_shared<ValueNode>();
    p->meta_ = std::make_shared<FPointMeta>(0.0);
    child_->node_ = Node::Cast<Node>(p);
    child_->type_ = Type::Double;
  }
  std::static_pointer_cast<FPointMeta>(child<ValueNode>()->meta_)->val_ = val;
  return *this;
}

template <typename T, std::enable_if_t<is_string_v<T>, bool> = true>
Node& Node::operator=(const T& val) {
  if (child_->type_ != Type::String) {
    auto p = std::make_shared<ValueNode>();
    p->meta_ = std::make_shared<StringMeta>("");
    child_->node_ = Node::Cast<Node>(p);
    child_->type_ = Type::String;
  }
  std::static_pointer_cast<StringMeta>(child<ValueNode>()->meta_)->val_ = val;
  return *this;
}

template <std::size_t N>
Node& Node::operator=(const char (&arr)[N]) {
  this->operator=(std::string(arr));
  return *this;
}

inline void Node::setTag(const std::string& tag) {
  if (isValid()) {
    child_->tag_ = tag;
  }
}

inline std::string Node::tag() const {
  return isValid() ? child_->tag_ : "";
}

template <typename T,
          std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>,
                           bool> = true>
void Node::push_back(const T val) {
  if (child_->type_ != Type::Array) {
    auto p = std::make_shared<ArrayNode>();
    child_->node_ = Node::Cast<Node>(p);
    child_->type_ = Type::Array;
  }
  Node n = val;
  child<ArrayNode>()->nodes_.emplace_back(n);
}

template <typename T, std::enable_if_t<is_string_v<T>, bool> = true>
void Node::push_back(const T& val) {
  if (child_->type_ != Type::Array) {
    auto p = std::make_shared<ArrayNode>();
    child_->node_ = Node::Cast<Node>(p);
    child_->type_ = Type::Array;
  }
  Node n = val;
  child<ArrayNode>()->nodes_.emplace_back(n);
}

template <std::size_t N>
void Node::push_back(const char (&val)[N]) {
  if (child_->type_ != Type::Array) {
    auto p = std::make_shared<ArrayNode>();
    child_->node_ = Node::Cast<Node>(p);
    child_->type_ = Type::Array;
  }
  Node n = val;
  child<ArrayNode>()->nodes_.emplace_back(n);
}

inline void Node::push_back(const Node& n) {
  if (child_->type_ != Type::Array) {
    auto p = std::make_shared<ArrayNode>();
    child_->node_ = Node::Cast<Node>(p);
    child_->type_ = Type::Array;
  }

  child<ArrayNode>()->nodes_.emplace_back(n);
}

template <typename T,
          std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>,
                           bool> = true>
Node Node::insert(const std::string& key, const T val) {
  if (child_->type_ != Type::Map) {
    auto p = std::make_shared<MapNode>();
    child_->node_ = Node::Cast<Node>(p);
    child_->type_ = Type::Map;
  }

  Node ret;
  auto p = child<MapNode>();
  if (p->nodes_.count(key) == 1) {
    ret = p->nodes_[key];
  }
  p->nodes_[key] = val;
  return ret;
}

template <typename T, std::enable_if_t<is_string_v<T>, bool> = true>
Node Node::insert(const std::string& key, const T& val) {
  if (child_->type_ != Type::Map) {
    auto p = std::make_shared<MapNode>();
    child_->node_ = Node::Cast<Node>(p);
    child_->type_ = Type::Map;
  }

  Node ret;
  auto p = child<MapNode>();
  if (p->nodes_.count(key) == 1) {
    ret = p->nodes_[key];
  }
  p->nodes_[key] = val;
  return ret;
}

template <std::size_t N>
Node Node::insert(const std::string& key, const char (&val)[N]) {
  if (child_->type_ != Type::Map) {
    auto p = std::make_shared<MapNode>();
    child_->node_ = Node::Cast<Node>(p);
    child_->type_ = Type::Map;
  }

  Node ret;
  auto p = child<MapNode>();
  if (p->nodes_.count(key) == 1) {
    ret = p->nodes_[key];
  }
  p->nodes_[key] = val;
  return ret;
}

inline Node Node::insert(const std::string& key, const Node& n) {
  if (child_->type_ != Type::Map) {
    auto p = std::make_shared<MapNode>();
    child_->node_ = Node::Cast<Node>(p);
    child_->type_ = Type::Map;
  }

  Node ret;
  auto p = child<MapNode>();
  if (p->nodes_.count(key) == 1) {
    ret = p->nodes_[key];
  }
  p->nodes_[key] = n;
  return ret;
}

inline std::size_t Node::size() const {
  switch (child_->type_) {
    case Type::Array:
      return child<ArrayNode>()->nodes_.size();

    case Type::Map:
      return child<MapNode>()->nodes_.size();

    default:
      throw std::bad_cast();
      break;
  }

  return std::size_t(-1);
}

inline void Node::clear() {
  switch (child_->type_) {
    case Type::Array:
      child<ArrayNode>()->nodes_.clear();
      break;

    case Type::Map:
      child<MapNode>()->nodes_.clear();
      break;

    default:
      throw std::bad_cast();
      break;
  }
}

inline Node Node::clone() const {
  if (!child_) {
    return Node();
  }

  Node n;
  n.child_->type_ = child_->type_;
  n.child_->tag_ = child_->tag_;
  n.child_->extra_ = child_->extra_;
  switch (child_->type_) {
    case Node::Type::Bool: {
      auto src = std::static_pointer_cast<BoolMeta>(child<ValueNode>()->meta_);
      auto tar = std::make_shared<ValueNode>();
      tar->meta_ = std::make_shared<BoolMeta>(src->val_);
      n.child_->node_ = Node::Cast<Node>(tar);
    } break;
    case Node::Type::Integer: {
      auto src =
          std::static_pointer_cast<IntegerMeta>(child<ValueNode>()->meta_);
      auto tar = std::make_shared<ValueNode>();
      tar->meta_ = std::make_shared<IntegerMeta>(src->val_);
      n.child_->node_ = Node::Cast<Node>(tar);
    } break;
    case Node::Type::Double: {
      auto src =
          std::static_pointer_cast<FPointMeta>(child<ValueNode>()->meta_);
      auto tar = std::make_shared<ValueNode>();
      tar->meta_ = std::make_shared<FPointMeta>(src->val_);
      n.child_->node_ = Node::Cast<Node>(tar);
    } break;
    case Node::Type::String: {
      auto src =
          std::static_pointer_cast<StringMeta>(child<ValueNode>()->meta_);
      auto tar = std::make_shared<ValueNode>();
      tar->meta_ = std::make_shared<StringMeta>(src->val_);
      n.child_->node_ = Node::Cast<Node>(tar);
    } break;
    case Node::Type::Array: {
      auto src = child<ArrayNode>();
      auto tar = std::make_shared<ArrayNode>();
      tar->nodes_.resize(src->nodes_.size());
      for (int i = 0; i < tar->nodes_.size(); ++i) {
        tar->nodes_[i] = src->nodes_[i].clone();
      }
      n.child_->node_ = Node::Cast<Node>(tar);
    } break;
    case Node::Type::Map: {
      auto src = child<MapNode>();
      auto tar = std::make_shared<MapNode>();
      for (auto const& [key, node] : src->nodes_) {
        tar->nodes_.emplace(key, node.clone());
      }
      n.child_->node_ = Node::Cast<Node>(tar);
    } break;
    default:
      break;
  }

  return n;
}

inline std::ostream& operator<<(std::ostream& oss, Node& n) {
  if (n.isBool()) {
    oss << n.as<bool>();
  } else if (n.isInteger()) {
    oss << n.as<int64_t>();
  } else if (n.isDouble()) {
    oss << n.as<double>();
  } else if (n.isString()) {
    oss << n.as<std::string>();
  } else if (n.isArray()) {
    oss << "[ ";
    for (auto iter = n.begin(); iter != n.end(); ++iter) {
      oss << (*iter) << " ";
    }
    oss << "]";
  } else if (n.isMap()) {
    for (auto iter = n.begin(); iter != n.end(); ++iter) {
      oss << iter->first << ":";
      if (iter->second.isMap()) {
        oss << std::endl;
      }
      oss << iter->second;
      if (!iter->second.isMap()) {
        oss << std::endl;
      }
    }
  }

  return oss;
}

inline Node& Node::Iterator::operator*() {
  return *arr_iter_;
}

inline std::map<std::string, Node>::iterator& Node::Iterator::operator->() {
  return map_iter_;
}

inline bool Node::Iterator::operator==(const Node::Iterator& other) const {
  switch (type_) {
    case Type::Array:
      return arr_iter_ == other.arr_iter_;

    case Type::Map:
      return map_iter_ == other.map_iter_;

    default:
      break;
  }
  return false;
}

inline bool Node::Iterator::operator!=(const Node::Iterator& other) const {
  switch (type_) {
    case Type::Array:
      return arr_iter_ != other.arr_iter_;

    case Type::Map:
      return map_iter_ != other.map_iter_;

    default:
      break;
  }

  return false;
}

inline Node::Iterator& Node::Iterator::operator++() {
  switch (type_) {
    case Type::Array:
      ++arr_iter_;
      break;

    case Type::Map:
      ++map_iter_;
      break;

    default:
      break;
  }

  return *this;
}

inline Node::Iterator Node::Iterator::operator++(int) {
  Node::Iterator ret = *this;
  this->operator++();
  return ret;
}

inline Node::Iterator& Node::Iterator::operator=(
    const std::vector<Node>::iterator& it) {
  type_ = Type::Array;
  arr_iter_ = it;
  return *this;
}

inline Node::Iterator& Node::Iterator::operator=(
    const std::map<std::string, Node>::iterator& it) {
  type_ = Type::Map;
  map_iter_ = it;
  return *this;
}

}  // namespace utils