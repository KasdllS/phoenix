#pragma once

#include "utils/meta/define.h"

namespace phoenix {

class Node {
 public:
  Node();
  ~Node();

  bool isValid() const;

  Node& operator=(const int val);
  Node& operator=(const std::string& val);

  std::string name_;

 private:
  Meta* meta_;
  Node* child_;
};

}  // namespace phoenix