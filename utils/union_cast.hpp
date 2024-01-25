#pragma once

/**
 * @brief
 *
 * 为了做一些不被允许的危险操作，很少会用到
 * 大多数情况可以被 `reinterpret_cast` 替代
 */

namespace phoenix {

template <typename TO, typename FROM>
inline TO union_cast(FROM value) {
  union {
    FROM from;
    TO to;
  } convert;
  convert.from = value;
  return convert.to;
}

}  // namespace phoenix