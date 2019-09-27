#ifndef _COMPILER_H_
#define _COMPILER_H_

#ifdef __GNUC__
#  include "compiler_gcc.h"
#elif defined(_MSC_VER)
#  include "compiler_msc.h"
#else
#  pragma message("Unrecognized compiler.  Please add a header file for your compiler.  Compilation will proceed, but performance may suffer!")
#endif

#include <stddef.h> /* size_t */

#ifndef __bool_true_false_are_defined
#  include <stdbool.h> /* bool */
#endif

/* Fixed-width integer types */
#ifndef PRIu32
#  include <inttypes.h>
#endif

//#include <stdint.h>
#include <limits.h>

#ifdef _WIN32
#include <windows.h>
#define PATH_SEPERATOR	"\\"
#define MAX_PATH_LEN	MAX_PATH
#else
//#include <limits.h>
#define PATH_SEPERATOR	"/"
#define MAX_PATH_LEN	PATH_MAX
#endif


//typedef unsigned __int64	QWORD, *LPQWORD;
typedef unsigned long       ulong_t;
typedef int                 BOOL;
//typedef unsigned char       BYTE;
//typedef unsigned short      WORD;

typedef const wchar_t * 	LPCWSTR;
typedef wchar_t *			LPWSTR;
typedef const char * 		LPCSTR;
typedef char *				LPSTR;

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif


#if defined(_MSC_VER)
#pragma warning(disable:4996)
#pragma warning ( disable : 4005 )
#pragma warning ( disable : 4995 )

#else

#define strcmpi			strcasecmp
#define stricmp			strcasecmp
#define strcpy_s		strcpy
#define strcat_s		strcat
//#if 0
//#define sprintf_s(x,args...)		snprintf((x), sizeof((x)),##args)
//extern "C" char * strlwr(char *s);
//#endif
#endif


#endif //_COMPILER_H_