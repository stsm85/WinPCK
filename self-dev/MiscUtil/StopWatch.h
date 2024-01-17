#pragma once
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
