/**
 * @file factory_register.hpp
 * @author sqi(kasdll@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-03-22
 *
 * @copyright Copyright (c) 2023
 *
 * 提供一个利用静态变量特性实现的工厂注册器
 * e.g.
 *
 * struct UserDefineIface {};
 * using UserDefineIfaceFactory = ClassFactory<int,UserDefineIface>;
 *
 * template <class UserDefineDerived>
 * class Register {
 *  public:
 *    Register() {
 *      UserDefineIfaceFactory::instance().Register(UserDefineDerived::id,
 *        []() -> UserDefineIface* {
 *          return new UserDefineDerived;
 *        });
 *    }
 * };
 *
 * template <class UserDefineDerived>
 * using UserDefineRegister =
 *     ClassRegister<UserDefineIface, UserDefineDerived
 *                                  , Register<UserDefineDerived>>;
 *
 * class UserDefineA : public UserDefineIface,
 *                     public UserDefineRegister<UserDefineA> {
 *  public:
 *    constexpr static int type = 0;
 *    UserDefineA() = default;
 * };
 */
#pragma once

#include <type_traits>

#include "utils/macros.hpp"

template <class Base, class Derived, class Register>
class ClassRegister {
 public:
  struct RegisterT : public Register {
    void touch() {
      // 不用实现
    }
  };
  ClassRegister() {
    static_assert(std::is_base_of_v<Base, Derived>, "Invalid Inheritance");
    s_register.touch();
  }

  inline static RegisterT s_register;
};

template <class Factory, class Derived>
class Register {
 public:
  Register() {
    Factory::instance().RegisterCreator(Derived::type, Derived::create);
  }
};

template <class Factory, class Derived, typename... Args>
class RegisterWithArgs {
 public:
  using FactoryT = typename Factory::InstT;
  using BaseT = typename FactoryT::BaseT;

  using BasePtr = typename FactoryT::template PtrT<BaseT>;
  using DerivedPtr = typename FactoryT::template PtrT<Derived>;

  RegisterWithArgs() {
    Factory::instance().RegisterCreator(
        Derived::type, [](Args&&... args) -> BasePtr {
          return DerivedPtr(new Derived(std::forward<Args>(args)...));
        });
  }
};

template <class Register>
class FunctorRegister {
 public:
  struct RegisterT : public Register {
    void touch() const {
      //
    }
  };

  FunctorRegister() { s_register.touch(); }

  inline static RegisterT s_register;
};