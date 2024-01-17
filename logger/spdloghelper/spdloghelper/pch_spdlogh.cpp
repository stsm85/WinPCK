// pch.cpp: 与预编译标头对应的源文件

#include "pch_spdlogh.h"

// 当使用预编译的头时，需要使用此源文件，编译才能成功。

//#ifndef _IS_CONSOLE_EXE_
//#error "please define _IS_CONSOLE_EXE_" 
//#else
//#if _IS_CONSOLE_EXE_
//CLogger& Logger = CLogger::GetInstance(std::bind(&init_logger_console, std::placeholders::_1));
//#else
//CLogger& Logger = CLogger::GetInstance(std::bind(&init_logger_memfile, std::placeholders::_1));
//#endif
//#endif