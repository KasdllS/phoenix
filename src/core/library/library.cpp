#include "core/library/library.h"

#include <dlfcn.h>
#include <error.h>
#include <string.h>

namespace core {

Library::Library(const std::string& path) : path_(path) {
  load();
}

Library::~Library() {
  if (isLoaded()) {
    unload();
  }
}

void Library::setLibPath(const std::string& name) {
  if (isLoaded()) {
    unload();
  }

  path_ = name;
}

bool Library::isLoaded() const {
  return handler_ != nullptr;
}

void Library::load() {
  if (isLoaded()) {
    return;
  }

  handler_ = dlopen(path_.c_str(), RTLD_LOCAL | RTLD_LAZY);
  if (!handler_) {
    error_ = strerror(errno);
  }
}

void Library::unload() {
  if (isLoaded()) {
    dlclose(handler_);
  }
}

void* Library::resolve(const std::string& name) {
  if (!isLoaded()) {
    error_ = "Please load the library first";
    return nullptr;
  }

  void* ret = dlsym(handler_, name.c_str());
  if (!ret) {
    error_ = strerror(errno);
  }

  return ret;
}

std::string Library::error() const {
  return error_;
}

}  // namespace core