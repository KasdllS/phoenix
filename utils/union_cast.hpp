#pragma once

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

}