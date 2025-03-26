#pragma once

#include <tuple>
#include <type_traits>

template <typename... Types>
class TupleData {
 public:
  TupleData() = default;

  template <int idx, typename T>
  void setData(const T& data) {
    std::get<idx>(data_) = data;
  }

  template <int idx>
  auto& getData() {
    return std::get<idx>(data_);
  }

  template <int idx>
  void reset() {
    auto& data = std::get<idx>(data_);
    data = std::remove_reference_t<decltype(data)>();
  }

 private:
  std::tuple<Types...> data_;
};