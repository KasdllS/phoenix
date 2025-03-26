#pragma once

#include <assert.h>
#include <iostream>
#include "utils/macros.hpp"

#ifndef fassert
#define fassert(expr)                                                     \
  do {                                                                    \
    if (UNLIKELY(!(expr))) {                                              \
      std::cerr << __FILE__ << ":" << __LINE__ << "(" << __func__ << ") " \
                << STR(expr) << std::endl;                                \
      abort();                                                            \
    }                                                                     \
  } while (0)
#endif
