
#ifndef _BASE64_H_
#define _BASE64_H_
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//#define BASE64_DLLEXP
//#define BASE64_DLLIMP
#define BASE64_STATIC

#ifdef BASE64_DLLEXP
#	define BEXTERN extern __declspec(dllexport)
#	define BEXPORTVA __cdecl
#	define BEXPORT __stdcall
#elif defined BASE64_STATIC
#	define BEXTERN 
#	define BEXPORTVA __cdecl
#	define BEXPORT __stdcall
#else
#	define BEXTERN extern __declspec(dllimport)
#	define BEXPORTVA __cdecl
#	define BEXPORT __stdcall
#endif

#define BASE64_ZUP 0

BEXTERN uint32_t BEXPORT decodeBound(const char* pdata,const uint32_t data_size);
BEXTERN uint32_t BEXPORT encodeBound(const uint32_t data_size);

BEXTERN void BEXPORT base64_encode(const void* pdata,const uint32_t data_size,void* out_pcode);
BEXTERN void BEXPORT base64_decode(const void* pdata,const uint32_t data_size,void* out_pcode);

#ifdef __cplusplus
}
#endif

#endif //_BASE64_H_