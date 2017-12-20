
#include "base64.h"

unsigned int __cdecl stringlen (
        const char * str
        )
{
        const char *eos = str;

        while( *eos++ ) ;

        return( eos - str - 1 );
}

BEXTERN unsigned long BEXPORT decodeBound(const void* pdata,const unsigned long data_size)
{
	const unsigned char* input=(const unsigned char*)pdata;
	unsigned long		size;
	unsigned long		dstsize;

	if(0 == data_size){
		size = stringlen(input);
	}else if(0 == (data_size & 0x3)){
		size = data_size;
	}else{
		return 0;
	}

	input += size;
	input -= 2;

	dstsize = (size >>2) * 3;

	if('=' == *input++)
		dstsize -= 2;
	else if('=' == *input)
		dstsize -= 1;

	return dstsize;
}

BEXTERN unsigned long BEXPORT encodeBound(const void* pdata,const unsigned long data_size)
{
	
	const unsigned char* input=(const unsigned char*)pdata;

	if(0 == data_size)
		return 0;

	return (((data_size + 2) / 3)<<2) + 1;

}

//
//#ifdef _WINDLL
//
//	#include <windows.h>
//
//	#ifdef _MANAGED
//	#pragma managed(push, off)
//	#endif
//
//	BOOL APIENTRY DllMain( HMODULE hModule,
//						   DWORD  ul_reason_for_call,
//						   LPVOID lpReserved
//						 )
//	{
//		switch (ul_reason_for_call)
//		{
//		case DLL_PROCESS_ATTACH:
//		case DLL_THREAD_ATTACH:
//		case DLL_THREAD_DETACH:
//		case DLL_PROCESS_DETACH:
//			break;
//		}
//		return TRUE;
//	}
//
//	#ifdef _MANAGED
//	#pragma managed(pop)
//	#endif
//
//#endif //_WINDLL
//