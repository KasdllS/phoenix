#pragma once

#include <list>
#include <memory>
#include <unordered_map>
#include <utility>

#include "utils/noncopyable.hpp"
#include "utils/nonmovable.hpp"
#include "utils/single_ton.hpp"

template <typename Key,
          class Prod,
          template <typename Tp> class Ptr = std::shared_ptr,
          class ProdCreator = Ptr<Prod> (*)()>
class ClassFactory : public noncopyable, public nonmovable {
 public:
  template <typename T>
  using PtrT = Ptr<T>;

  using BaseT = Prod;

  bool RegisterCreator(Key k, ProdCreator creator) {
    return creators_.insert(std::make_pair(k, creator)).second;
  }

  bool Contains(Key k) { return creators_.count(k) == 1; }

  void Clear() { creators_.clear(); }

  bool Empty() { return creators_.empty(); }

  template <typename... Args>
  Ptr<Prod> Create(Key k, Args&&... args) {
    typename std::unordered_map<Key, ProdCreator>::const_iterator cit =
        creators_.find(k);
    if (cit == creators_.end())
      return nullptr;
    return (cit->second)(std::forward<Args>(args)...);
  }

  std::vector<Key> keys() const {
    std::vector<Key> ret;
    ret.reserve(creators_.size());
    for (auto const& [key, value] : creators_) {
      ret.emplace_back(key);
    }
    return ret;
  }

 private:
  std::unordered_map<Key, ProdCreator> creators_;
};

template <typename Key,
          class Prod,
          template <typename Tp> class Ptr = std::shared_ptr,
          class ProdCreator = Ptr<Prod> (*)()>
using FactorySingleTon = SingleTon<ClassFactory<Key, Prod, Ptr, ProdCreator>>;
