#pragma once

/**
 * @file list.hpp
 * @author sqi
 * @brief
 * @version 0.1
 * @date 2024-01-25
 *
 * @copyright Copyright (c) 2024
 *
 * TODO: 集成一些无锁结构
 */

#include <algorithm>
#include <list>
#include <shared_mutex>

namespace phoenix::thread_safe {

template <typename T>
class list {
 public:
  using iterator = typename std::list<T>::iterator;
  list() = default;

  void push_back(const T& v) {
    std::unique_lock<std::shared_mutex> lck(mtx_list_);
    list_.push_back(v);
  }

  bool erase(const T& v) {
    std::unique_lock<std::shared_mutex> lck(mtx_list_);
    auto iter = std::find(list_.begin(), list_.end(), v);
    if (iter == list_.end()) {
      return false;
    }

    list_.erase(iter);
    return true;
  }

  std::list<T> to_list() {
    std::shared_lock<std::shared_mutex> lck(mtx_list_);
    return list_;
  }

 private:
  std::shared_mutex mtx_list_;
  std::list<T> list_;
};

}  // namespace phoenix::thread_safe
