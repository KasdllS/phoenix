#pragma once

#include <functional>
#include "utils/factory/class_register.hpp"
#include "utils/macros.hpp"
#include "utils/meta.hpp"
#include "utils/noncopyable.hpp"
#include "utils/nonmovable.hpp"
#include "utils/single_ton.hpp"

namespace parser {

class ParserImpl : public noncopyable, public nonmovable {
 public:
  using SerializeFunc = std::function<std::string(const utils::Node&)>;
  using DeserializeFunc = std::function<utils::Node(const std::string&)>;
  ParserImpl() = default;

  void registFuncs(const std::string& key,
                   const SerializeFunc& ser,
                   const DeserializeFunc& deser);

  utils::Node deserialize(const std::string& key, const std::string& bytes);
  std::string serialize(const std::string& key, const utils::Node& node);

 private:
  std::unordered_map<std::string, SerializeFunc> ser_funcs_;
  std::unordered_map<std::string, DeserializeFunc> deser_funcs_;
};

using ParserInst = SingleTon<ParserImpl>;

template <typename T>
class RegisterImpl {
 public:
  RegisterImpl() {
    ParserInst::instance().registFuncs(T::key, T::serialize, T::deserialize);
  }
};

#define REGIST_PARSER(cls)                                         \
  class CONCAT(cls, Register) {                                    \
   public:                                                         \
    CONCAT(cls, Register)() {                                      \
      ParserInst::instance().registFuncs(cls::key, cls::serialize, \
                                         cls::deserialize);        \
    }                                                              \
  };                                                               \
  static CONCAT(cls, Register) CONCAT(s_reg_, __COUNTER__);

}  // namespace parser