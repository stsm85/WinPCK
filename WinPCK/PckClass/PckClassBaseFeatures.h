#pragma once
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include "PckHeader.h"
#include "PckClassLog.h"

#include <vector>
using namespace std;

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

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

protected:
	//重置信息
	void	ResetPckInfos();

	//设置进度信息中的最大值
	void	SetParams_ProgressUpper(DWORD dwUIProgressUpper);


#ifdef _DEBUG
	static char *formatString(const char *format, ...);
	static int logOutput(const char *file, const char *text);
#endif
	//添加待压缩文件时，遍历文件时使用的文件列表的第一个文件
	LPFILES_TO_COMPRESS		m_firstFile;

	//打印日志
	CPckClassLog			m_PckLog;
	PCK_ALL_INFOS			m_PckAllInfo;
	//运行时变量 
	LPPCK_RUNTIME_PARAMS	m_lpPckParams;
};
