#pragma once

#include <algorithm>
#include <list>
#include <map>
#include <string>

#include "utils/macros.hpp"

#ifndef DEFINE_ENUM
#define DEFINE_ENUM(EnumName, ...)                                         \
  enum class EnumName { __VA_ARGS__ };                                     \
                                                                           \
  class EnumName##Map {                                                    \
   public:                                                                 \
    EnumName##Map() { initMap(); }                                         \
    static void initMap() {                                                \
      std::string arg = #__VA_ARGS__;                                      \
      std::list<std::string> args = spilit(arg);                           \
      for (auto const& str : args) {                                       \
        assignValue(str);                                                  \
      }                                                                    \
    }                                                                      \
                                                                           \
    inline static std::map<int, std::string> getMap() { return enum_str; } \
                                                                           \
   private:                                                                \
    static void removeBlank(std::string& str) {                            \
      auto noSpaceEnd = std::remove(str.begin(), str.end(), ' ');          \
      str.erase(noSpaceEnd, str.end());                                    \
    }                                                                      \
                                                                           \
    static std::list<std::string> spilit(const std::string& str) {         \
      std::list<std::string> ret;                                          \
                                                                           \
      std::string source = str;                                            \
      removeBlank(source);                                                 \
      std::size_t pos = 0;                                                 \
      std::string_view view(source);                                       \
                                                                           \
      while (pos != std::string::npos) {                                   \
        pos = view.find(',');                                              \
        if (auto tar = view.substr(0, pos); !tar.empty()) {                \
          ret.emplace_back(tar);                                           \
        }                                                                  \
        view = view.substr(pos + 1);                                       \
      }                                                                    \
                                                                           \
      return ret;                                                          \
    }                                                                      \
                                                                           \
    static void assignValue(const std::string& str) {                      \
      std::string_view view(str);                                          \
      std::size_t pos = view.find('=');                                    \
      if (pos == std::string::npos) {                                      \
        ++value;                                                           \
        enum_str.emplace(value, view.substr(0, pos));                      \
      } else {                                                             \
        value = std::atoll(std::string(view.substr(pos + 1)).c_str());     \
        enum_str.emplace(value, view.substr(0, pos));                      \
      }                                                                    \
    }                                                                      \
                                                                           \
    inline static int64_t value = -1;                                      \
    inline static std::map<int, std::string> enum_str;                     \
  };                                                                       \
  inline const static EnumName##Map CONCAT(s_##EnumName, __COUNTER__);
#endif