
#include "base64.h"
#include <string>

BEXTERN uint32_t BEXPORT decodeBound(const char* pdata,const uint32_t data_size)
{
	const char* input = pdata;
	uint32_t		size;
	uint32_t		dstsize;

	if(0 == data_size){
		size = strlen(input);
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

BEXTERN uint32_t BEXPORT encodeBound(const uint32_t data_size)
{

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