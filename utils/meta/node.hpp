#pragma once

/**
 * @file node_define.h
 * @author sqi
 * @brief
 * @version 0.1
 * @date 2024-01-24
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <assert.h>

#include <map>
#include <memory>
#include <vector>

#include "utils/meta/meta_define.hpp"
#include "utils/type_traits.hpp"

namespace phoenix {

template <typename T, typename U = std::decay_t<T>>
using type_check_t =
    std::enable_if_t<std::is_integral_v<U> || std::is_floating_point_v<U> ||
                         std::is_same_v<std::string, U> ||
                         std::is_same_v<const char*, U>,
                     bool>;

struct Elem {
  using Ptr = std::shared_ptr<Elem>;
  virtual ~Elem() = default;
  virtual Elem::Ptr clone() const = 0;

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
  Node() = default;

  template <typename T, type_check_t<T> = true>
  explicit Node(T val);

  Node clone() const;

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

  template <typename T,
            std::enable_if_t<!std::is_base_of_v<Meta, T>, bool> = true>
  T as() const;

  template <typename T,
            std::enable_if_t<std::is_base_of_v<Meta, T>, bool> = true>
  T as() const;

  std::string name_;

 private:
  void assign(const std::shared_ptr<Meta>& meta);
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

  friend class Maker;
};

struct ElemValue : public Elem {
  Elem::Ptr clone() const override {
    auto ret = std::make_shared<ElemValue>();
    ret->meta_ = meta_->clone();
    return std::dynamic_pointer_cast<Elem>(ret);
  }

  std::shared_ptr<Meta> meta_;
};

struct ElemMap : public Elem {
  Elem::Ptr clone() const override {
    auto ret = std::make_shared<ElemMap>();
    for (auto& [name, node] : nodes_) {
      ret->nodes_.emplace(name, node.clone());
    }
    return std::dynamic_pointer_cast<Elem>(ret);
  }

  std::map<std::string, Node, std::less<>> nodes_;
};

struct ElemVec : public Elem {
  Elem::Ptr clone() const override {
    auto ret = std::make_shared<ElemVec>();
    ret->nodes_.reserve(nodes_.size());
    for (auto& node : nodes_) {
      ret->nodes_.emplace_back(node.clone());
    }
    return std::dynamic_pointer_cast<Elem>(ret);
  }
  std::vector<Node> nodes_;
};

template <typename T, type_check_t<T> = true>
Node::Node(T val) {
  using MetaType = MetaImpl<typename type_cvt<std::decay_t<T>>::dst_t>;

  elem_ = std::make_shared<ElemValue>();
  elem_->type_ = static_cast<int>(ElemType::Value);
  auto elem = std::dynamic_pointer_cast<ElemValue>(elem_);
  elem->meta_ = std::make_shared<MetaType>(val);
}

Node Node::clone() const {
  Node n;
  n.name_ = name_;
  n.elem_ = elem_->clone();
  return n;
}

bool Node::isValue() const {
  if (!elem_) {
    return false;
  }

  return elem_->type_ == static_cast<int>(ElemType::Value);
}

bool Node::isMap() const {
  if (!elem_) {
    return false;
  }

  return elem_->type_ == static_cast<int>(ElemType::Map);
}

bool Node::isVector() const {
  if (!elem_) {
    return false;
  }

  return elem_->type_ == static_cast<int>(ElemType::Vector);
}

bool Node::isValid() const { return elem_ != nullptr; }

template <typename T, type_check_t<T> = true>
Node& Node::operator=(T val) {
  using MetaType = MetaImpl<typename type_cvt<std::decay_t<T>>::dst_t>;

  if (delete_if(ElemType::Value)) {
    elem_ = std::make_shared<ElemValue>();
    elem_->type_ = static_cast<int>(ElemType::Value);
  }
  auto& meta = std::static_pointer_cast<ElemValue>(elem_)->meta_;
  if (!meta) {
    meta = std::make_shared<MetaType>(val);
  } else if (!check_type<T>(meta->type_)) {
    meta = std::make_shared<MetaType>(val);
  } else {
    std::dynamic_pointer_cast<MetaType>(meta)->val_ = val;
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

Node& Node::operator[](const std::string& key) {
  assert(isValid() &&
         static_cast<Node::ElemType>(elem_->type_) == Node::ElemType::Map);
  return std::dynamic_pointer_cast<ElemMap>(elem_)->nodes_[key];
}

Node& Node::operator[](const std::size_t idx) {
  assert(isValid() &&
         static_cast<Node::ElemType>(elem_->type_) == Node::ElemType::Vector);
  auto elem = std::dynamic_pointer_cast<ElemVec>(elem_);
  assert(idx < elem->nodes_.size());
  return elem->nodes_[idx];
}

template <typename T,
          std::enable_if_t<!std::is_base_of_v<Meta, T>, bool> = true>
T Node::as() const {
  using Type = typename type_cvt<std::decay_t<T>>::dst_t;
  using DstType = std::conditional_t<std::is_reference_v<T>,
                                     std::add_lvalue_reference_t<Type>, Type>;
  using MetaType = MetaImpl<DstType>;

  assert(isValid());
  auto elem = std::dynamic_pointer_cast<ElemValue>(elem_);
  auto meta = std::static_pointer_cast<MetaType>(elem->meta_);
  assert(std::is_reference_v<DstType> == meta->is_ref_);
  return meta->val_;
}

template <typename T, std::enable_if_t<std::is_base_of_v<Meta, T>, bool> = true>
T Node::as() const {
  if (!elem_ || elem_->type_ != static_cast<int>(ElemType::Value)) {
    abort();
  }

  return *std::static_pointer_cast<T>(
      std::dynamic_pointer_cast<ElemValue>(elem_)->meta_);
}

void Node::assign(const std::shared_ptr<Meta>& meta) {
  if (delete_if(ElemType::Value)) {
    elem_ = std::make_shared<ElemValue>();
  }

  auto elem = std::dynamic_pointer_cast<ElemValue>(elem_);
  elem->meta_ = meta;
}

bool Node::delete_if(const ElemType type) {
  if (elem_ && elem_->type_ != static_cast<int>(type)) {
    elem_ = nullptr;
  }

  return elem_ == nullptr;
}

struct Maker {
 protected:
  static Node make(const std::shared_ptr<Meta>& meta) {
    Node n;
    auto elem = std::make_shared<ElemValue>();
    elem->type_ = static_cast<int>(Node::ElemType::Value);
    elem->meta_ = meta;
    n.elem_ = elem;
    return n;
  }
};

template <typename T, typename Enable = void>
struct MakeNode {};

template <typename T>
struct MakeNode<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, bool>>>
    : public Maker {
  /**
   * @brief 可用于向check box传递初始数据
   *
   * @tparam T bool
   * @param val
   * @param editable 是否可进行编辑
   * @return Node
   */
  Node operator()(T val, bool editable = default_editable) const {
    return Maker::make(std::make_shared<MetaImpl<T>>(val, editable));
  }
};

template <typename T>
struct MakeNode<T, std::enable_if_t<std::is_enum_v<std::decay_t<T>>>>
    : public Maker {
  /**
   * @brief 可用于向combo box传递初始数据
   *
   * @tparam T enum class/enum
   * @param val
   * @param combo 枚举/字符描述对。
   * @param editable 是否可进行编辑
   * @return Node
   */
  Node operator()(typename MetaImpl<T>::Type val,
                  const std::map<int64_t, std::string>& combo,
                  bool editable = default_editable) const {
    return Maker::make(std::make_shared<MetaImpl<T>>(val, combo, editable));
  }
};

template <typename T>
struct MakeNode<T, std::enable_if_t<is_numeric_v<std::decay_t<T>>>>
    : public Maker {
  /**
   * @brief 可用于向spin box传递初始数据
   *
   * @tparam T 整数/浮点数等类型
   * @param val
   * @param min 下限
   * @param max 上限
   * @param step 调整步长
   * @param editable 是否可进行编辑
   * @return Node
   */
  Node operator()(
      typename MetaImpl<T>::ValueType val,
      typename MetaImpl<T>::ParamType min =
          std::numeric_limits<typename MetaImpl<T>::ParamType>::min(),
      typename MetaImpl<T>::ParamType max =
          std::numeric_limits<typename MetaImpl<T>::ParamType>::max(),
      typename MetaImpl<T>::ParamType step = MetaImpl<T>::default_step,
      bool editable = default_editable) const {
    return Maker::make(
        std::make_shared<MetaImpl<T>>(val, min, max, step, editable));
  }
};

template <typename T>
struct MakeNode<T, std::enable_if_t<is_string_v<std::decay_t<T>>>>
    : public Maker {
  /**
   * @brief 可用于标签、字符输入框等
   *
   * @tparam T 字符串类型(const char* / std::string)
   * @param val 字符
   * @param editable 是否可进行编辑
   * @return Node
   */
  Node operator()(typename MetaImpl<T>::ParamType val,
                  bool editable = default_editable) const {
    return Maker::make(std::make_shared<MetaImpl<T>>(val, editable));
  }
};

template <typename T>
inline const static MakeNode<T> makeNode;

}  // namespace phoenix