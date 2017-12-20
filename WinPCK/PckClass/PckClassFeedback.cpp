//////////////////////////////////////////////////////////////////////
// PckClassFeedback.cpp: 信息回馈、信息输出、调试等 
//
// 此程序由 李秋枫/stsm/liqf 编写，pck结构参考若水的pck结构.txt，并
// 参考了其易语言代码中并于读pck文件列表的部分
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2015.5.19
//////////////////////////////////////////////////////////////////////

#include "PckClass.h"
#include "PckControlCenter.h"

#define define_one_PrintLog(_loglevel)	\
void CPckClass::PrintLog##_loglevel(const char *_text)\
{\
	lpPckParams->lpPckControlCenter->PrintLog##_loglevel(_text);\
}\
void CPckClass::PrintLog##_loglevel(const wchar_t *_text)\
{\
	lpPckParams->lpPckControlCenter->PrintLog##_loglevel(_text);\
}

define_one_PrintLog(I);
define_one_PrintLog(W);
define_one_PrintLog(E);
define_one_PrintLog(D);
define_one_PrintLog(N);

void CPckClass::PrintLogE(const char *_maintext, const char *_file, const char *_func, const long _line)
{
	lpPckParams->lpPckControlCenter->PrintLogE(_maintext, _file, _func, _line);
}

void CPckClass::PrintLogE(const wchar_t *_maintext, const char *_file, const char *_func, const long _line)
{
	lpPckParams->lpPckControlCenter->PrintLogE(_maintext, _file, _func, _line);
}

void CPckClass::PrintLogE(const char *_fmt, const char *_maintext, const char *_file, const char *_func, const long _line)
{
	lpPckParams->lpPckControlCenter->PrintLogE(_fmt, _maintext, _file, _func, _line);
}

void CPckClass::PrintLogE(const char *_fmt, const wchar_t *_maintext, const char *_file, const char *_func, const long _line)
{
	lpPckParams->lpPckControlCenter->PrintLogE(_fmt, _maintext, _file, _func, _line);
}

void CPckClass::PrintLog(const char chLevel, const char *_maintext)
{
	lpPckParams->lpPckControlCenter->PrintLog(chLevel, _maintext);
}

void CPckClass::PrintLog(const char chLevel, const wchar_t *_maintext)
{
	lpPckParams->lpPckControlCenter->PrintLog(chLevel, _maintext);
}

void CPckClass::PrintLog(const char chLevel, const char *_fmt, const char *_maintext)
{
	lpPckParams->lpPckControlCenter->PrintLog(chLevel, _fmt, _maintext);
}

void CPckClass::PrintLog(const char chLevel, const char *_fmt, const wchar_t *_maintext)
{
	lpPckParams->lpPckControlCenter->PrintLog(chLevel, _fmt, _maintext);
}


