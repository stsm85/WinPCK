#pragma once
//#include <Windows.h>
#include "PckDefines.h"
#if PCK_DEBUG_OUTPUT_FILE
#include <mutex>
#endif

//日志
#define	LOG_BUFFER						8192

class CPckClassLog
{
private:
	CPckClassLog();
	CPckClassLog(const CPckClassLog&) = delete;
	~CPckClassLog();

	const CPckClassLog& operator=(const CPckClassLog&) = delete;

public:

	static CPckClassLog& GetInstance() {
		static CPckClassLog onlyInstance;
		return onlyInstance;
	}
	//注册LOG显示方式
	void PckClassLog_func_register(ShowLogW _ShowLogW);

	void e(const char *_text, ...);
	void w(const char *_text, ...);
	void i(const char *_text, ...);
	void d(const char *_text, ...);
	void n(const char *_text, ...);

	void e(const wchar_t *_text, ...);
	void w(const wchar_t *_text, ...);
	void i(const wchar_t *_text, ...);
	void d(const wchar_t *_text, ...);
	void n(const wchar_t *_text, ...);

	void	PrintErrorLog();
	void	PrintLog(const char chLevel, const char *_fmt, va_list ap);
	void	PrintLog(const char chLevel, const wchar_t *_fmt, va_list ap);

#if PCK_DEBUG_OUTPUT
	void OutputVsIde(const char *_text, ...);
#else
	void OutputVsIde(...) {};
#endif

#if PCK_DEBUG_OUTPUT_FILE
	std::mutex	m_LockLogFile;
	int logOutput(const char *file, const char *format, ...);
#else
	void logOutput(...) {}
#endif

private:

	//日志显示方式，默认显示到控制台
	static void		PrintLogToConsole(const char log_level, const wchar_t *str);

	static ShowLogW	ShowLogExtern;

	void		ShowLog(const char log_level, const char *str);
	void		ShowLog(const char log_level, const wchar_t *str);

	//错误信息
	DWORD	m_dwLastError = 0;
	//返回具体错误信息
	wchar_t *GetErrorMsg(CONST DWORD dwError, wchar_t *szMessage);

};

extern CPckClassLog& Logger;

#define LoggerExInternal2(msg, file, func, line, ...) Logger.e(##msg " at: "##file ", function: " func ", line: "#line, ##__VA_ARGS__)
#define LoggerExInternal(msg, file, func, line, ...) LoggerExInternal2(msg, file, func, line, ##__VA_ARGS__)
#define Logger_el(msg, ...) LoggerExInternal(msg, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
