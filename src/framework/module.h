#pragma once

#include "core/object.h"

namespace core {

class Module : public Object {
 public:
  enum class Status { OK = 0, Loading, Unready, Offline, Warnning, Error };

  Module() = default;

  virtual void initialize() = 0;

 protected:
  void setStatus(Status status);

 private:
  Status status_;
};

}  // namespace core