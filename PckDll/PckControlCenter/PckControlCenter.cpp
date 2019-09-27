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
	
//const DWORD	CPckControlCenter::m_dwNumberOfProcessors = thread::hardware_concurrency();

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
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

void CPckControlCenter::init()
{
	cParams.lpPckControlCenter = this;
	//cParams.code_page = 936;
	cParams.dwCompressLevel = getDefaultCompressLevel();
	cParams.dwMTThread = thread::hardware_concurrency();
	cParams.dwMTMaxMemory = getMaxMemoryAllowed();
}

void CPckControlCenter::uninit()
{
	lpszFilePathToAdd.clear();
}

void CPckControlCenter::Reset(uint32_t dwUIProgressUpper)
{
	memset(&cParams.cVarParams, 0, sizeof(PCK_VARIETY_PARAMS));
	cParams.cVarParams.dwUIProgressUpper = dwUIProgressUpper;
}
