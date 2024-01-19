#pragma once

#include "Logger.h"

#ifdef _MAKE_DLL	//.dll
#define LOG_EXTERN_DEF _declspec(dllexport)
#elif defined(_DLL)	//.exe
#define LOG_EXTERN_DEF _declspec(dllimport)
#else	//other
#define LOG_EXTERN_DEF extern
#endif

LOG_EXTERN_DEF extern CLogger& Logger;

//template<typename FormatString, typename... Args>
//void client_log(int msglevel, spdlog::level::level_enum lvl, const FormatString& fmt, Args&&...args)
//{
//	Logger->log(lvl, fmt, std::forward<Args>(args)...);
//	send_log(msglevel, lvl, std::format(fmt, std::forward<Args>(args)...));
//}
