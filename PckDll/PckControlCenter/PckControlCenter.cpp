//////////////////////////////////////////////////////////////////////
// PckControlCenter.cpp: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 头文件,界面与PCK类的数据交互，控制中心
//
// 此程序由 李秋枫/stsm/liqf 编写，pck结构参考若水的pck结构.txt，并
// 参考了其易语言代码中并于读pck文件列表的部分
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.10.10
//////////////////////////////////////////////////////////////////////

#include "PckControlCenter.h"
#include "PckClass.h"
#include "PckClassLog.h"
#include <thread>
	
const DWORD	CPckControlCenter::m_dwNumberOfProcessors = thread::hardware_concurrency();

CPckControlCenter::CPckControlCenter():
	m_lpClassPck(NULL),
	m_lpPckRootNode(NULL),
	m_emunFileFormat(FMTPCK_UNKNOWN),
	cParams({ 0 })
{
	init();
	New();
}

CPckControlCenter::~CPckControlCenter()
{
	regMsgFeedback(NULL, DefaultFeedbackCallback);
	Close();
	uninit();
}

void CPckControlCenter::init()
{
	cParams.lpPckControlCenter = this;
	cParams.code_page = 936;
	cParams.dwCompressLevel = getDefaultCompressLevel();
	cParams.dwMTThread = m_dwNumberOfProcessors;
	cParams.dwMTMaxMemory = getMaxMemoryAllowed();

	m_lpPckLog = new CPckClassLog();
}

void CPckControlCenter::uninit()
{
	lpszFilePathToAdd.clear();
	delete m_lpPckLog;
}

void CPckControlCenter::Reset(DWORD dwUIProgressUpper)
{
	memset(&cParams.cVarParams, 0, sizeof(PCK_VARIETY_PARAMS));
	cParams.cVarParams.dwUIProgressUpper = dwUIProgressUpper;
}

#pragma region 日志相关功能
//日志
void	CPckControlCenter::regShowFunc(ShowLogW _ShowLogW)
{
	CPckClassLog::PckClassLog_func_register(_ShowLogW);
}

const char CPckControlCenter::getLogLevelPrefix(int _loglevel)
{
	return CPckClassLog::GetLogLevelPrefix(_loglevel);
}

void CPckControlCenter::Print(const char chLevel, const char *_fmt, ...)
{
	va_list	ap;
	va_start(ap, _fmt);
	CPckClassLog::PrintLog(chLevel, _fmt, ap);
	va_end(ap);
}

void CPckControlCenter::Print(const char chLevel, const wchar_t *_fmt, ...)
{
	va_list	ap;
	va_start(ap, _fmt);
	CPckClassLog::PrintLog(chLevel, _fmt, ap);
	va_end(ap);
}

void CPckControlCenter::Print(const char chLevel, const char *_fmt, va_list ap)
{
	CPckClassLog::PrintLog(chLevel, _fmt, ap);
}

void CPckControlCenter::Print(const char chLevel, const wchar_t *_fmt, va_list ap)
{
	CPckClassLog::PrintLog(chLevel, _fmt, ap);
}
#pragma endregion