#pragma once
#include <Windows.h>
#include "PckDefines.h"

//日志
#define	LOG_BUFFER						8192

#if 0
#define	LOG_FLAG_ERROR					'E'
#define	LOG_FLAG_WARNING				'W'
#define	LOG_FLAG_INFO					'I'
#define	LOG_FLAG_DEBUG					'D'
#define	LOG_FLAG_NOTICE					'N'
#endif 

class CPckClassLog
{
public:
	CPckClassLog();
	~CPckClassLog();

	//注册LOG显示方式
	static void PckClassLog_func_register(ShowLogW _ShowLogW);

	//日志显示函数
#define define_define_one_PrintLog(_loglvchar)	\
	static void PrintLog##_loglvchar(const char *_text, ...);\
	static void PrintLog##_loglvchar(const wchar_t *_text, ...);\
	static void PrintLog##_loglvchar(const char *_text, va_list ap);\
	static void PrintLog##_loglvchar(const wchar_t *_text, va_list ap);

	//PrintLog[IWEDN]
	define_define_one_PrintLog(I);
	define_define_one_PrintLog(W);
	define_define_one_PrintLog(E);
	define_define_one_PrintLog(D);
	define_define_one_PrintLog(N);

#undef define_define_one_PrintLog

	//__FILE__, __FUNCTION__, __LINE__
	void	PrintLogEL(const char *_maintext, const char *_file, const char *_func, const long _line);
	void	PrintLogEL(const wchar_t *_maintext, const char *_file, const char *_func, const long _line);
	void	PrintLogEL(const char *_fmt, const char *_maintext, const char *_file, const char *_func, const long _line);
	void	PrintLogEL(const char *_fmt, const wchar_t *_maintext, const char *_file, const char *_func, const long _line);


	static void	PrintLog(const char chLevel, const char *_fmt, va_list ap);
	static void	PrintLog(const char chLevel, const wchar_t *_fmt, va_list ap);
	static void	PrintLog(const char chLevel, const char *_fmt, ...);
	static void	PrintLog(const char chLevel, const wchar_t *_fmt, ...);

	static const char	GetLogLevelPrefix(int _loglevel);
private:

	static const char	m_szLogPrefix[LOG_IMAGE_COUNT];

	//日志显示方式，默认显示到控制台
	static void		PrintLogToConsole(const int log_level, const wchar_t *str);

	static ShowLogW	ShowLogExtern;
	static void		ShowLog(const int log_level, const char *str);
	static void		ShowLog(const int log_level, const wchar_t *str);

	//错误信息
	DWORD	m_dwLastError;
	//返回具体错误信息
	wchar_t *GetErrorMsg(CONST DWORD dwError, wchar_t *szMessage);

};

