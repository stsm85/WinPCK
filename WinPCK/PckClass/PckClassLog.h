#pragma once

//日志
#define	LOG_BUFFER						8192

#if 0
#define	LOG_FLAG_ERROR					'E'
#define	LOG_FLAG_WARNING				'W'
#define	LOG_FLAG_INFO					'I'
#define	LOG_FLAG_DEBUG					'D'
#define	LOG_FLAG_NOTICE					'N'
#endif 

#define LOG_IMAGE_NOTICE				0
#define LOG_IMAGE_INFO					1
#define LOG_IMAGE_WARNING				2
#define LOG_IMAGE_ERROR					3
#define LOG_IMAGE_DEBUG					4
#define LOG_IMAGE_EMPTY					5
#define LOG_IMAGE_COUNT					6

typedef void(*ShowLogA)(const int log_level, const char *str);
typedef void(*ShowLogW)(const int log_level, const wchar_t *str);

class CPckClassLog
{
public:
	CPckClassLog();
	~CPckClassLog();

	//注册LOG显示方式
	void PckClassLog_func_register(ShowLogW _ShowLogW);

	//日志显示函数
#define define_define_one_PrintLog(_loglvchar)	\
	void PrintLog##_loglvchar(const char *_text, ...);\
	void PrintLog##_loglvchar(const wchar_t *_text, ...);\
	void PrintLog##_loglvchar(const char *_text, va_list ap);\
	void PrintLog##_loglvchar(const wchar_t *_text, va_list ap);

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


	void	PrintLog(const char chLevel, const char *_fmt, va_list ap);
	void	PrintLog(const char chLevel, const wchar_t *_fmt, va_list ap);
	void	PrintLog(const char chLevel, const char *_fmt, ...);
	void	PrintLog(const char chLevel, const wchar_t *_fmt, ...);

	static const char	GetLogLevelPrefix(int _loglevel);
private:

	static const char	m_szLogPrefix[LOG_IMAGE_COUNT];

	//日志显示方式，默认显示到控制台
	void		PrintLogToConsole(const int log_level, const wchar_t *str);

	static ShowLogW	ShowLogExtern;
	void		ShowLog(const int log_level, const char *str);
	void		ShowLog(const int log_level, const wchar_t *str);

	//错误信息
	DWORD	m_dwLastError;
	//返回具体错误信息
	wchar_t *GetErrorMsg(CONST DWORD dwError, wchar_t *szMessage);

};

