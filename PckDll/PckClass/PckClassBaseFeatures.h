#pragma once
#include <stdio.h>
#include "PckHeader.h"
#include "PckClassLog.h"

#include "AllocMemPool.h"
#include "PckClassZlib.h"

#include <vector>

class CPckClassBaseFeatures 
{
public:
	CPckClassBaseFeatures();
	~CPckClassBaseFeatures();

	virtual const	LPPCKINDEXTABLE		GetPckIndexTable();
	virtual const	LPPCK_PATH_NODE		GetPckPathNode();

	//是否已经加载文件
	BOOL	isFileLoaded();

	//文件大小
	uint64_t	GetPckSize();

	//获取文件数
	uint32_t	GetPckFileCount();

	//数据区大小
	uint64_t	GetPckDataAreaSize();

	//数据区冗余数据大小
	uint64_t	GetPckRedundancyDataSize();

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
	BOOL	CheckIfNeedForcedStopWorking();
	void	SetErrMsgFlag(int errMsg);

	PCK_ALL_INFOS			m_PckAllInfo;

	CAllocMemPool			m_NodeMemPool;

	CPckClassZlib			m_zlib;

	//运行时变量 
	LPPCK_RUNTIME_PARAMS	m_lpPckParams;

};
