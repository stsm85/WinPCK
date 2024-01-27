#pragma once

#include "Logger.h"

#define LOGGER_INIT_IN_DLL 1


#ifdef _MAKE_DLL	//.dll
#define LOG_EXTERN_DEF _declspec(dllexport)
#elif defined(_DLL)	//.exe
#define LOG_EXTERN_DEF _declspec(dllimport)
#else	//other
#define LOG_EXTERN_DEF extern
#endif

#if ! LOGGER_INIT_IN_DLL
#undef LOG_EXTERN_DEF
#define LOG_EXTERN_DEF extern
#endif

LOG_EXTERN_DEF CLogger& Logger;

#define ENABLE_DETAILLOG 1

#include "DetailLogger.h"

