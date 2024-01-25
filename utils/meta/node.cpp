#include "utils/meta/node.h"

#include <iostream>

namespace phoenix {

Node::Node() : name_("") {}

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

bool Node::delete_if(const ElemType type) {
  if (elem_ && elem_->type_ != static_cast<int>(type)) {
    elem_ = nullptr;
  }

  return elem_ == nullptr;
}

}  // namespace phoenix