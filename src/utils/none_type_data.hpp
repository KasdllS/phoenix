#pragma once

#include <assert.h>
#include <cxxabi.h>
#include <any>
#include <string>
#include <typeinfo>

class Any {
 public:
  Any() = default;

  template <typename T>
  Any(const T& data) : data_(data) {}

  Any(const Any& data) : data_(data) {}
  Any(Any&& data) noexcept { data_ = std::move(data.data_); }

  bool isValid() const { return data_.has_value(); }

  const std::type_info& type() const { return data_.type(); }

  std::string typeName() const {
    return abi::__cxa_demangle(type().name(), NULL, NULL, NULL);
  }

  template <typename T>
  bool canConvert() const {
    return typeid(T) == data_.type();
  }

  template <typename T>
  T cast() const {
    assert(data_.type() == data_.type());
    return std::any_cast<T>(data_);
  }

  template <typename T>
  Any& operator=(const T& data) {
    data_ = data;
    return *this;
  }

  Any& operator=(const Any& other) = default;

  Any& operator=(Any&& other) noexcept {
    data_ = std::move(other.data_);
    return *this;
  }

  template <typename T>
  void set(const T& data) {
    *this = data;
  }

  void clear() { data_.reset(); }

 private:
  std::any data_;
};