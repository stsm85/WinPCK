#pragma once

#include "Logger.h"

#ifndef _COMPILE_AS_LIB
#ifdef _MAKE_DLL	//.dll
#define LOG_EXTERN_DEF _declspec(dllexport) 
#elif defined(_MAKE_EXE)	//.exe
#define LOG_EXTERN_DEF _declspec(dllimport) 
#else	//other
#define LOG_EXTERN_DEF 
#endif
#else
#define LOG_EXTERN_DEF 
#endif

LOG_EXTERN_DEF extern CLogger& Logger;

#define ENABLE_DETAILLOG 1

#include "DetailLogger.h"

