#pragma once
//////////////////////////////////////////////////////////////////////
// StopWatch.h
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////


#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
#include <Windows.h>

#ifdef UNICODE 
#define getElapsedTimeString getElapsedTimeStringW
#else
#define getElapsedTimeString getElapsedTimeStringA
#endif

class CStopWatch
{
public:
	CStopWatch();
	void start();
	void stop();
	double getElapsedTime(); //in s
	//const char * getElapsedTimeStringA();
	//const wchar_t * getElapsedTimeStringW();

private:
	LARGE_INTEGER m_start;
	LARGE_INTEGER m_stop;
	LARGE_INTEGER m_frequency;
};
