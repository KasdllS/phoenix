/*
 * @Author: sqi
 * @Date: 2022-03-24 19:33:25
 * @LastEditors: sqi
 * @LastEditTime: 2022-11-14 15:10:52
 * @Description:
 */
#ifndef UTILS_TIME_HPP
#define UTILS_TIME_HPP

#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

#include <chrono>

#include "utils/macros.hpp"
#include "utils/single_ton.hpp"

namespace utils {

class time {
 protected:
  time() {
    pthread_key_create(&tic_key, [](void* tm) { delete (uint64_t*)(tm); });
  }
  time(time&&) = delete;
  ~time() { pthread_key_delete(tic_key); }

  pthread_key_t tic_key;

 public:
  enum class TIME_TYPE {
    SECONDS,
    MILLISECONDS,
    MICROSECONDS,
    NANOSECONDS,
  };

  /**
   * @brief: 获取毫秒级时间戳
   * @return 毫秒级时间戳
   */
  static uint64_t get_cur_ts() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
  }

  /**
   * @brief
   * 一组基于TLS的tic、toc函数
   * 仅限于同一线程内的计时
   */
  static void tic() { SingleTon<time>::instance().tic_with_tls(); }
  static uint64_t toc(TIME_TYPE type = TIME_TYPE::MILLISECONDS) {
    return SingleTon<time>::instance().toc_with_tls(type);
  }

 private:
  void tic_with_tls() const {
    if (UNLIKELY(pthread_getspecific(tic_key) == nullptr)) {
      pthread_setspecific(tic_key, new uint64_t(-1));
    }

    *((uint64_t*)pthread_getspecific(tic_key)) =
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch())
            .count();
  }

  uint64_t toc_with_tls(TIME_TYPE type) const {
    uint64_t const* t = (uint64_t*)pthread_getspecific(tic_key);
    assert(t != nullptr && *t != uint64_t(-1));
    uint64_t cnt = std::chrono::duration_cast<std::chrono::nanoseconds>(
                       std::chrono::steady_clock::now().time_since_epoch())
                       .count() -
                   *t;
    int step;
    switch (type) {
      case TIME_TYPE::SECONDS:
        step = 1000000000;
        break;
      case TIME_TYPE::MILLISECONDS:
        step = 1000000;
        break;
      case TIME_TYPE::MICROSECONDS:
        step = 1000;
        break;
      case TIME_TYPE::NANOSECONDS:
        step = 1;
        break;

      default:
        abort();
    }
    return cnt / step;
  }
};

}  // namespace utils

#endif