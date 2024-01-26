//////////////////////////////////////////////////////////////////////
// StopWatch.cpp
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////

#include "StopWatch.h"
#include <stdio.h>

#define TIME_ELAPSED_STR	"用时 %.2fms"
#define TIME_ELAPSED_STRW	L"用时 %.2fms"

CStopWatch::CStopWatch()
{
	m_start.QuadPart = 0;
	m_stop.QuadPart = 0;
	QueryPerformanceFrequency(&m_frequency);
}

void CStopWatch::start()
{
	QueryPerformanceCounter(&m_start);
}

void CStopWatch::stop()
{
	QueryPerformanceCounter(&m_stop);
}

double CStopWatch::getElapsedTime()
{
	LARGE_INTEGER time;
	time.QuadPart = m_stop.QuadPart - m_start.QuadPart;
	return (double)time.QuadPart / (double)m_frequency.QuadPart * 1000;
}

#if 0
const char *  CStopWatch::getElapsedTimeStringA()
{
	static char szTime[100];
	sprintf_s(szTime, TIME_ELAPSED_STR, getElapsedTime());
	return szTime;
}

const wchar_t *  CStopWatch::getElapsedTimeStringW()
{
	static wchar_t szTime[100];
	swprintf_s(szTime, TIME_ELAPSED_STRW, getElapsedTime());
	return szTime;
}
#endif