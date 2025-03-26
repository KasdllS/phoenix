#pragma once

#include <any>
#include <functional>
#include <future>
#include <memory>
#include "utils/assert.h"
#include "utils/macros.hpp"
#include "utils/none_type_data.hpp"
#include "utils/type_traits.hpp"

namespace utils {

class Functor {
 public:
  Functor() = default;
  virtual ~Functor() = default;

  virtual bool isValid() const = 0;
};
using FunctorPtr = std::shared_ptr<Functor>;

template <typename Signature>
class FunctorImpl;

template <typename RetType, typename... ArgTypes>
class FunctorImpl<RetType(ArgTypes...)> : public Functor {
  using function = std::function<RetType(ArgTypes...)>;

  template <typename T>
  using decay_t = std::decay_t<T>;

 public:
  constexpr static std::size_t arg_count = sizeof...(ArgTypes);
  explicit FunctorImpl(const function& func)
      : func_(func),
        type_(typeid(
            std::function<std::decay_t<RetType>(std::decay_t<ArgTypes>...)>)) {}

  RetType invoke(ArgTypes&&... args) {
    if (typeid(
            std::function<std::decay_t<RetType>(std::decay_t<ArgTypes>...)>) !=
        type_) {
      throw std::bad_any_cast();
    }
    return func_(std::forward<ArgTypes>(args)...);
  }

  bool isValid() const override { return !func_; }

 private:
  function func_;
  const std::type_info& type_;
};

template <typename RetType, typename... ArgTypes>
FunctorImpl(RetType (*)(ArgTypes...)) -> FunctorImpl<RetType(ArgTypes...)>;

template <typename RetType, typename... ArgTypes>
using FunctorImplPtr = std::shared_ptr<FunctorImpl<RetType(ArgTypes...)>>;

template <typename RetType, typename... ArgTypes>
FunctorImplPtr<RetType, ArgTypes...> makeFunctor(RetType (*func)(ArgTypes...)) {
  return std::make_shared<FunctorImpl<RetType(ArgTypes...)>>(func);
}

class Function {
 public:
  template <typename T>
  class Result {
   public:
    void wait() { future_->wait(); }

    T get() { return future_->get(); }

   private:
    explicit Result(const std::shared_ptr<std::future<T>>& future)
        : future_(future) {}

    std::shared_ptr<std::future<T>> future_;

    friend class Function;
  };

  Function() = default;
  ~Function() = default;

  template <typename R, typename... Args>
  static Function makeFunction(const std::function<R(Args...)>& func) {
    assert(func);
    using function_generator_t =
        std::conditional_t<std::is_same_v<R, void>, void_ret<Args...>,
                           other_ret<R, Args...>>;
    Function function;
    function_generator_t::generate_function(func, function.future_,
                                            function.functor_);
    return function;
  }

  bool isValid() const { return !functor_ && functor_->isValid(); }

  template <typename... Args>
  void invoke(Args&&... args) const {
    auto p = std::dynamic_pointer_cast<FunctorImpl<void(Args...)>>(functor_);
    p->invoke(std::forward<Args>(args)...);
  }

  template <typename T>
  Result<T> getResult() const {
    assert(typeid(std::shared_ptr<std::future<T>>) == future_.type());
    return Result<T>(future_.cast<std::shared_ptr<std::future<T>>>());
  }

 private:
  template <typename R, typename... Args>
  struct other_ret {
    static void generate_function(const std::function<R(Args...)>& func,
                                  Any& future,
                                  FunctorPtr& functor) {
      auto p = std::make_shared<std::promise<R>>();
      auto f = std::make_shared<::std::future<R>>(p->get_future());
      future = f;
      std::function<void(Args...)> invoke_func = [p, func](Args&&... args) {
        p->set_value(func(std::forward<Args>(args)...));
      };
      functor = std::make_shared<FunctorImpl<void(Args...)>>(invoke_func);
    }
  };

  template <typename... Args>
  struct void_ret {
    static void generate_function(const std::function<void(Args...)>& func,
                                  Any& future,
                                  FunctorPtr& functor) {
      auto p = std::make_shared<std::promise<void>>();
      auto f = std::make_shared<::std::future<void>>(p->get_future());
      future = f;
      std::function<void(Args...)> invoke_func = [p, func](Args&&... args) {
        func(std::forward<Args>(args)...);
        p->set_value();
      };
      functor = std::make_shared<FunctorImpl<void(Args...)>>(invoke_func);
    }
  };

  Any future_;
  FunctorPtr functor_;
};

}  // namespace utils
