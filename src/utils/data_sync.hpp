#pragma once

#include <mutex>
#include <vector>
#include "utils/macros.hpp"
#include "utils/thread/annotations.hpp"

template <typename T, class Mutex, class LockGaurd>
class DataSync {
 public:
  DataSync() = default;

  void set(const T& data) {
    LockGaurd lck(mtx_);
    data_ = data;
  }

  T get() {
    LockGaurd lck(mtx_);
    return data_;
  }

  T take() {
    T ret;
    LockGaurd lck(mtx_);
    ret = data_;
    data_ = T();
    return ret;
  }

  void direct_set(const T& data) { data_ = data; }

  T direct_get() { return data_; }

  void lock() { mtx_.lock(); }

  void unlock() { mtx_.unlock(); }

 private:
  Mutex mtx_;
  T data_ GAURDED_BY(mtx_);
};

template <typename T, class Mutex, class LockGaurd>
class DataSync<std::vector<T>, Mutex, LockGaurd> {
 public:
  DataSync() = default;

  void set(const std::vector<T>& data) {
    LockGaurd lck(mtx_);
    data_ = data;
  }

  std::vector<T> get() {
    LockGaurd lck(mtx_);
    return data_;
  }

  void clear() {
    LockGaurd lck(mtx_);
    data_.clear();
  }

  std::vector<T> take() {
    LockGaurd lck(mtx_);
    auto ret = data_;
    data_.clear();
    return ret;
  }

  void push_back(const T& data) {
    LockGaurd lck(mtx_);
    data_.emplace_back(data);
  }

  std::size_t size() {
    LockGaurd lck(mtx_);
    return data_.size();
  }

  DataSync& operator=(const std::vector<T>& data) {
    set(data);
    return *this;
  }

  void lock() { mtx_.lock(); }

  void unlock() { mtx_.unlock(); }

 private:
  Mutex mtx_;
  std::vector<T> data_ GAURDED_BY(mtx_);
};

template <typename T>
using MutexData = DataSync<T, std::mutex, std::scoped_lock<std::mutex>>;