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

	//文件大小
	QWORD	GetPckSize();

	//获取文件数
	DWORD	GetPckFileCount();

	//数据区大小
	QWORD	GetPckDataAreaSize();

	//数据区冗余数据大小
	QWORD	GetPckRedundancyDataSize();

	//获取基本信息，用于写入文件后造成的文件损坏的恢复
	BOOL GetPckBasicInfo(LPRESTORE_INFOS lpRestoreInfo);
	BOOL SetPckBasicInfo(LPRESTORE_INFOS lpRestoreInfo);

protected:
	//重置信息
	void	ResetPckInfos();

	//设置进度
	void	SetParams_ProgressInc();
	void	SetParams_Progress(DWORD dwUIProgres);

	//设置进度信息中的最大值
	void	SetParams_ProgressUpper(DWORD dwUIProgressUpper);

#ifdef _DEBUG
	static SRWLOCK	m_LockLogFile;
	static int logOutput(const char *file, const char *format, ...);
#else
	static void logOutput(...){}
#endif
	//添加待压缩文件时，遍历文件时使用的文件列表的第一个文件
	//LPFILES_TO_COMPRESS		m_firstFile;

	//打印日志
	CPckClassLog			m_PckLog;
	PCK_ALL_INFOS			m_PckAllInfo;

	//运行时变量 
	LPPCK_RUNTIME_PARAMS	m_lpPckParams;

};
