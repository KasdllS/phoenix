#pragma once

#define LIKELY(exp) __builtin_expect(!!(exp), 1)
#define UNLIKELY(exp) __builtin_expect(!!(exp), 0)

#define UNUSED(x) (void)x

#ifndef CONCAT_1
#define CONCAT_1(x, y) x##y
#endif

#ifndef CONCAT
#define CONCAT(x, y) CONCAT_1(x, y)
#endif

#ifndef STR_1
#define STR_1(x) #x
#endif

#ifndef STR
#define STR(x) STR_1(x)
#endif