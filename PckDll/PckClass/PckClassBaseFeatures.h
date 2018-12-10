#pragma once
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include "PckHeader.h"
#include "PckClassLog.h"

#include <vector>
using namespace std;

class CPckClassBaseFeatures 
{
public:
	CPckClassBaseFeatures();
	~CPckClassBaseFeatures();

	virtual CONST	LPPCKINDEXTABLE		GetPckIndexTable();
	virtual CONST	LPPCK_PATH_NODE		GetPckPathNode();

	//是否已经加载文件
	BOOL	isFileLoaded();

	//文件大小
	QWORD	GetPckSize();

	//获取文件数
	DWORD	GetPckFileCount();

	//数据区大小
	QWORD	GetPckDataAreaSize();

	//数据区冗余数据大小
	QWORD	GetPckRedundancyDataSize();

protected:
	//重置信息
	void	ResetPckInfos();

	//设置进度
	void	SetParams_ProgressInc();
	void	SetParams_Progress(DWORD dwUIProgres);

	//设置进度信息中的最大值
	void	SetParams_ProgressUpper(DWORD dwUIProgressUpper, BOOL bReset = TRUE);
	void	AddParams_ProgressUpper(DWORD dwUIProgressUpperAdd);

	//多线程处理进程
	void	SetThreadFlag(BOOL isThreadWorking);
	//BOOL	CheckIfNeedForcedStopWorking();
	void	SetErrMsgFlag(int errMsg);

#if _DEBUG
#define _USELOGFILE 1
	static SRWLOCK	m_LockLogFile;
	static int logOutput(const char *file, const char *format, ...);
#else
#define _USELOGFILE 0
	static void logOutput(...){}
#endif

	//打印日志
	CPckClassLog			m_PckLog;
	PCK_ALL_INFOS			m_PckAllInfo;

	//运行时变量 
	LPPCK_RUNTIME_PARAMS	m_lpPckParams;

};
