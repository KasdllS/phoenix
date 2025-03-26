#pragma once

#include "core/object.h"

namespace core {

class Library final : public Object {
 public:
  explicit Library(const std::string& path);
  ~Library() override;

  void setLibPath(const std::string& path);

  bool isLoaded() const;

  void load();
  void unload();

  void* resolve(const std::string& name);

  std::string error() const;

 private:
  std::string path_;
  void* handler_;

  std::string error_;
};

}  // namespace core