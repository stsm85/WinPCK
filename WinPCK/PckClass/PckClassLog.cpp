//////////////////////////////////////////////////////////////////////
// PckClassLog.cpp: 信息回馈、信息输出、调试等 
//
// 此程序由 李秋枫/stsm/liqf 编写，pck结构参考若水的pck结构.txt，并
// 参考了其易语言代码中并于读pck文件列表的部分
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2015.5.19
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4996 )

#include <Windows.h>
#include "PckClassLog.h"
#include "CharsCodeConv.h"
#include <assert.h>
#include <stdio.h>

const char	CPckClassLog::m_szLogPrefix[LOG_IMAGE_COUNT] = { 'N', 'I', 'W', 'E', 'D', ' ' };
ShowLogW	CPckClassLog::ShowLogExtern = NULL;

CPckClassLog::CPckClassLog() : m_dwLastError(0)
{
}

CPckClassLog::~CPckClassLog()
{}

void CPckClassLog::PckClass_log_register(ShowLogW _ShowLogW)
{
	ShowLogExtern = _ShowLogW;
}

wchar_t* CPckClassLog::GetErrorMsg(CONST DWORD dwError, wchar_t *szMessage)
{
	//static WCHAR szMessage[1024] = L"错误原因：";
	wcscpy(szMessage, L"错误原因：");

	size_t len = wcslen(szMessage);
	WCHAR *lpszMessage = szMessage + len;
	// retrieve a message from the system message table
	if(!FormatMessageW(
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		lpszMessage,
		LOG_BUFFER - len,
		NULL)) {
		wcscpy(lpszMessage, L"未知错误");
	}
	SetLastError(0);
	return szMessage;
}

#pragma region PrintLogToDst

void CPckClassLog::ShowLog(const int _loglevel, const char *_logtext)
{
	CAnsi2Ucs cA2U;
	ShowLog(_loglevel, cA2U.GetString(_logtext));
}

void CPckClassLog::ShowLog(const int _loglevel, const wchar_t *_logtext)
{
	if(NULL != ShowLogExtern) {
		ShowLogExtern(_loglevel, _logtext);
	} else {
		PrintLogToConsole(_loglevel, _logtext);
	}
}

const char	CPckClassLog::GetLogLevelPrefix(int _loglevel)
{
	if((LOG_IMAGE_COUNT > _loglevel) && (0 < _loglevel))
		return  m_szLogPrefix[_loglevel];
	else
		return ' ';
}

void CPckClassLog::PrintLogToConsole(const int log_level, const wchar_t *str)
{
	
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	printf("%02d:%02d:%02d :%c ", systime.wHour, systime.wMinute, systime.wSecond, m_szLogPrefix[log_level]);

	wprintf(str);
}

#pragma endregion
#pragma region PrintLog[IWEDN](_text, ...)

#define define_one_PrintLog(_loglvchar, _loglevel)	\
void CPckClassLog::PrintLog##_loglvchar(const char *_text, ...)\
{\
	va_list	ap;\
	va_start(ap, _text);\
	PrintLog##_loglvchar(_text, ap);\
	va_end(ap);\
}\
void CPckClassLog::PrintLog##_loglvchar(const wchar_t *_text, ...)\
{\
	va_list	ap;\
	va_start(ap, _text);\
	PrintLog##_loglvchar(_text, ap);\
	va_end(ap);\
}\
void CPckClassLog::PrintLog##_loglvchar(const char *_text, va_list ap)\
{\
	char _maintext[LOG_BUFFER];\
	_vsnprintf(_maintext, sizeof(_maintext), _text, ap);\
	ShowLog(_loglevel, _maintext);\
}\
void CPckClassLog::PrintLog##_loglvchar(const wchar_t *_text, va_list ap)\
{\
	wchar_t _maintext[LOG_BUFFER];\
	_vsnwprintf(_maintext, sizeof(_maintext), _text, ap);\
	ShowLog(_loglevel, _maintext);\
}

define_one_PrintLog(I, LOG_IMAGE_INFO)
define_one_PrintLog(W, LOG_IMAGE_WARNING)
define_one_PrintLog(E, LOG_IMAGE_ERROR)
define_one_PrintLog(D, LOG_IMAGE_DEBUG)
define_one_PrintLog(N, LOG_IMAGE_NOTICE)

#undef define_one_PrintLog
#pragma endregion
#pragma region PrintLogEL

void CPckClassLog::PrintLogEL(const char *_maintext, const char *_file, const char *_func, const long _line)
{
	CAnsi2Ucs cA2U;
	m_dwLastError = GetLastError();
	PrintLogEL(cA2U.GetString(_maintext), _file, _func, _line);
}

void CPckClassLog::PrintLogEL(const wchar_t *_maintext, const char *_file, const char *_func, const long _line)
{
	wchar_t szMessage[LOG_BUFFER];

	PrintLog(LOG_IMAGE_ERROR, L"%s (%S 发生错误在 %S 行数:%d)", _maintext, _file, _func, _line);

	if(0 == m_dwLastError) {
		m_dwLastError = GetLastError();
		if(0 != m_dwLastError)
			PrintLog(LOG_IMAGE_ERROR, GetErrorMsg(GetLastError(), szMessage));
	} else {
		PrintLog(LOG_IMAGE_ERROR, GetErrorMsg(m_dwLastError, szMessage));
		m_dwLastError = 0;
	}
	assert(FALSE);

}

void CPckClassLog::PrintLogEL(const char *_fmt, const char *_maintext, const char *_file, const char *_func, const long _line)
{
	char szPrintf[LOG_BUFFER];
	sprintf_s(szPrintf, _fmt, _maintext);
	PrintLogEL(szPrintf, _file, _func, _line);

}
void CPckClassLog::PrintLogEL(const char *_fmt, const wchar_t *_maintext, const char *_file, const char *_func, const long _line)
{
	CUcs2Ansi cU2A;
	PrintLogEL(_fmt, cU2A.GetString(_maintext), _file, _func, _line);
}
#pragma endregion
#pragma region PrintLog
void CPckClassLog::PrintLog(const char chLevel, const char *_fmt, va_list ap)
{
	char _maintext[LOG_BUFFER];
	_vsnprintf(_maintext, sizeof(_maintext), _fmt, ap);

	ShowLog(chLevel, _maintext);
}

void CPckClassLog::PrintLog(const char chLevel, const wchar_t *_fmt, va_list ap)
{
	wchar_t _maintext[LOG_BUFFER];
	_vsnwprintf(_maintext, sizeof(_maintext), _fmt, ap);

	ShowLog(chLevel, _maintext);
}

void CPckClassLog::PrintLog(const char chLevel, const char *_fmt, ...)
{
	va_list	ap;
	va_start(ap, _fmt);
	PrintLog(chLevel, _fmt, ap);
	va_end(ap);
}

void CPckClassLog::PrintLog(const char chLevel, const wchar_t *_fmt, ...)
{
	va_list	ap;
	va_start(ap, _fmt);
	PrintLog(chLevel, _fmt, ap);
	va_end(ap);
}

#pragma endregion
