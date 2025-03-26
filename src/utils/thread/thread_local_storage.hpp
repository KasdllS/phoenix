#pragma once

#include <assert.h>
#include <pthread.h>
#include <functional>
#include "utils/macros.hpp"

namespace utils::thread {

template <typename T>
void default_deletor(T* p) {
  delete p;
}

template <typename T>
class ThreadLocalStorage {
  using destrcutor_t = void (*)(void*);

 public:
  using Creator = std::function<T*()>;
  using Deletor = void (*)(T*);

  struct default_constrcutor {
    static Creator create() {
      Creator ret = []() { return new T; };
      return ret;
    }
  };

  struct assert_t {
    static Creator create() { abort(); }
  };

  ThreadLocalStorage(Creator creator = nullptr, Deletor deletor = nullptr)
      : creator_(creator) {
    if (!creator) {
      creator = std::conditional_t<std::is_default_constructible_v<T>,
                                   default_constrcutor, assert_t>::create();
    }
    if (!deletor) {
      deletor = default_deletor;
    }
    initTLS(deletor);
  }

  operator T*() const {
    T* p = reinterpret_cast<T*>(pthread_getspecific(thread_local_key_));
    if (UNLIKELY(p == nullptr)) {
      p = creator_();
      pthread_setspecific(thread_local_key_, (void*)p);
    }
    return p;
  }

  T& operator*() const {
    T* p = reinterpret_cast<T*>(pthread_getspecific(thread_local_key_));
    if (UNLIKELY(p == nullptr)) {
      p = creator_();
      pthread_setspecific(thread_local_key_, (void*)p);
    }
    return *p;
  }

  T* operator->() const {
    T* p = reinterpret_cast<T*>(pthread_getspecific(thread_local_key_));
    if (UNLIKELY(p == nullptr)) {
      p = creator_();
      pthread_setspecific(thread_local_key_, (void*)p);
    }
    return p;
  }

 private:
  void initTLS(Deletor deletor) {
    destrcutor_t des = nullptr;
    des = reinterpret_cast<destrcutor_t>(deletor);

    pthread_key_create(&thread_local_key_, des);
  }

  pthread_key_t thread_local_key_;
  Creator creator_;
};

}  // namespace utils::thread