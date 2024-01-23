#pragma once

#include <assert.h>

#include <any>
#include <functional>
#include <future>
#include <memory>

namespace phoenix {

class Functor {
 protected:
  std::string void_type_;
  std::string type_;
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
  explicit FunctorImpl(const function& func) : func_(func) {
    // TODO : 类型存储
  }

  RetType invoke(ArgTypes&&... args) {
    // TODO : 类型验证
    return func_(std::forward<ArgTypes>(args)...);
  }

 private:
  function func_;
};

template <typename RetType, typename... ArgTypes>
FunctorImpl(RetType (*)(ArgTypes...)) -> FunctorImpl<RetType(ArgTypes...)>;

template <typename RetType, typename... ArgTypes>
using FunctorImplPtr = std::shared_ptr<FunctorImpl<RetType(ArgTypes...)>>;

template <typename RetType, typename... ArgTypes>
FunctorImplPtr<RetType, ArgTypes...> makeFunctor(RetType (*func)(ArgTypes...)) {
  return std::make_shared<FunctorImpl<RetType(ArgTypes...)>>(func);
}

}  // namespace phoenix
