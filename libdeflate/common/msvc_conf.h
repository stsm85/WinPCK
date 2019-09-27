#include <stdlib.h>
#define COMPILER_SUPPORTS_TARGET_FUNCTION_ATTRIBUTE 1
#define __SSE2__ 1

#ifdef _M_IX86
#define __i386__
#elif defined _M_X64
#define __x86_64__
#endif

//#define COMPILER_SUPPORTS_AVX_TARGET 1
//#define COMPILER_SUPPORTS_AVX2_TARGET 1
//#define COMPILER_SUPPORTS_TARGET_INTRINSICS 1
