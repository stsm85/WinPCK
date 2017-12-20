
#ifndef _BASE64_H_
#define _BASE64_H_


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


BEXTERN unsigned long BEXPORT decodeBound(const void* pdata,const unsigned long data_size);
BEXTERN unsigned long BEXPORT encodeBound(const void* pdata,const unsigned long data_size);

BEXTERN void BEXPORT base64_encode(const void* pdata,const unsigned long data_size,void* out_pcode);
BEXTERN void BEXPORT base64_decode(const void* pdata,const unsigned long data_size,void* out_pcode);

#ifdef __cplusplus
}
#endif

#endif //_BASE64_H_