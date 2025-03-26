#pragma once

#include <algorithm>
#include <list>
#include <shared_mutex>

namespace utils::thread {

template <typename T>
class list {
 public:
  using iterator = typename std::list<T>::iterator;
  list() = default;

  void push_back(const T& v) {
    std::unique_lock lck(mtx_list_);
    list_.push_back(v);
  }

  bool erase(const T& v) {
    std::unique_lock lck(mtx_list_);
    auto iter = std::find(list_.begin(), list_.end(), v);
    if (iter == list_.end()) {
      return false;
    }

    list_.erase(iter);
    return true;
  }

  bool empty() {
    std::shared_lock lck(mtx_list_);
    return list_.empty();
  }

  T pop_front() {
    std::unique_lock lck(mtx_list_);
    auto t = list_.front();
    list_.pop_front();
    return t;
  }

  std::list<T> to_list() {
    std::list<T> ret;
    std::shared_lock lck(mtx_list_);
    ret = list_;
    return ret;
  }

  std::list<T> take() {
    std::list<T> ret;
    std::unique_lock lck(mtx_list_);
    ret = std::move(list_);
    return ret;
  }

 private:
  std::shared_mutex mtx_list_;
  std::list<T> list_;
};

}  // namespace utils::thread