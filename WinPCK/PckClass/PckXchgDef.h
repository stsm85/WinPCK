//////////////////////////////////////////////////////////////////////
// PckXchgDef.h: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 头文件
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2015.5.22
//////////////////////////////////////////////////////////////////////

#pragma once

class CPckControlCenter;

typedef struct _PCK_VARIETY_PARAMS {

	DWORD		dwOldFileCount;
	DWORD		dwPrepareToAddFileCount;
	DWORD		dwChangedFileCount;
	DWORD		dwDuplicateFileCount;
	DWORD		dwFinalFileCount;

	DWORD		dwUseNewDataAreaInDuplicateFileSize;
	DWORD		dwDataAreaSize;
	DWORD		dwRedundancyDataSize;

	LPCSTR		lpszAdditionalInfo;

	DWORD		dwUIProgress;
	DWORD		dwUIProgressUpper;

	DWORD		dwMTMemoryUsed;

	BOOL		bThreadRunning;

}PCK_VARIETY_PARAMS;


typedef struct _PCK_RUNTIME_PARAMS {
	
	PCK_VARIETY_PARAMS	cVarParams;

	DWORD		dwMTMaxMemory;

	DWORD		dwMTThread;
	DWORD		dwMTMaxThread;		//当前CPU的1.5倍

	DWORD		dwCompressLevel;

	CPckControlCenter	*lpPckControlCenter;

	int			iListHotItem;

}PCK_RUNTIME_PARAMS, *LPPCK_RUNTIME_PARAMS;


