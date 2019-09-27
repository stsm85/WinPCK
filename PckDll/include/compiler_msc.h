/*
 * compiler_msc.h - definitions for the Microsoft C Compiler
 */

#define LIBEXPORT	__declspec(dllexport)

/*
 * Old versions (e.g. VS2010) of MSC don't have the C99 header stdbool.h.
 * Beware: the below replacement isn't fully standard, since normally any value
 * != 0 should be implicitly cast to a bool with value 1... but that doesn't
 * happen if bool is really just an 'int'.
 */
//typedef int bool;
//#define true 1
//#define false 0
//#define __bool_true_false_are_defined 1

/* Define ssize_t */
#ifdef _WIN64
typedef long long ssize_t;
#else
typedef int ssize_t;
#endif

/*
 * Old versions (e.g. VS2010) of MSC have stdint.h but not the C99 header
 * inttypes.h.  Work around this by defining the PRI* macros ourselves.
 */
//#include <inttypes.h>
#include <stdint.h>

//add by stsm
//#include "msvc_conf.h"
//#include "fix_clang_emmintrin.h"

#define PRIu8  "hhu"
#define PRIu16 "hu"
#define PRIu32 "u"
#define PRIu64 "llu"
#define PRIi8  "hhi"
#define PRIi16 "hi"
#define PRIi32 "i"
#define PRIi64 "lli"
#define PRIx8  "hhx"
#define PRIx16 "hx"
#define PRIx32 "x"
#define PRIx64 "llx"

/* Assume a little endian architecture with fast unaligned access */
#define CPU_IS_LITTLE_ENDIAN()		1
#define UNALIGNED_ACCESS_IS_FAST	1

/* __restrict has nonstandard behavior; don't use it */
#define restrict

/* ... but we can use __inline and __forceinline */
#define inline		__inline
#define forceinline	__forceinline

/* Byte swap functions */
#define bswap16	_byteswap_ushort
#define bswap32	_byteswap_ulong
#define bswap64	_byteswap_uint64
