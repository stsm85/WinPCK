//////////////////////////////////////////////////////////////////////
// PckClass.cpp: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 有关类的初始化等
//
// 此程序由 李秋枫/stsm/liqf 编写，pck结构参考若水的pck结构.txt，并
// 参考了其易语言代码中并于读pck文件列表的部分
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#include "PckClass.h"

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4146 )

void CPckClass::CPckClassInit()
{
	m_ReadCompleted = FALSE;

	m_lpPckIndexTable = NULL;
	m_firstFile = NULL;

	m_PckAllInfo.dwFileCount = 0;

	memset(&m_RootNode, 0, sizeof(PCK_PATH_NODE));

	*m_PckAllInfo.szAdditionalInfo = 0;
	*m_PckAllInfo.szFilename = 0;

	DWORD	dwCurrentPID = GetCurrentProcessId();

	sprintf_s(m_szEventAllWriteFinish, 16, TEXT_EVENT_WRITE_PCK_DATA_FINISH, dwCurrentPID);
	sprintf_s(m_szEventAllCompressFinish, 16, TEXT_EVENT_COMPRESS_PCK_DATA_FINISH, dwCurrentPID);
	sprintf_s(m_szEventMaxMemory, 16, TEXT_EVENT_PCK_MAX_MEMORY, dwCurrentPID);

	sprintf_s(m_szMapNameRead, 16, TEXT_MAP_NAME_READ, dwCurrentPID);
	sprintf_s(m_szMapNameWrite, 16, TEXT_MAP_NAME_WRITE, dwCurrentPID);

	BuildSaveDlgFilterString();

	init_compressor();

}

CPckClass::CPckClass(LPPCK_RUNTIME_PARAMS inout)
{
	lpPckParams = inout;
	CPckClassInit();
}

CPckClass::~CPckClass()
{
	DeAllocMultiNodes(m_RootNode.child);

	if(NULL != m_lpPckIndexTable)
		free(m_lpPckIndexTable);	
}

BOOL CPckClass::Init(LPCTSTR	szFile)
{
	////test();

	lstrcpy(m_PckAllInfo.szFilename, szFile);

	if(MountPckFile(m_PckAllInfo.szFilename))
	{
		BuildDirTree();
		return m_ReadCompleted = TRUE;
	}else{
		return m_ReadCompleted = FALSE;
	}
}

CONST	LPPCKINDEXTABLE CPckClass::GetPckIndexTable()
{
	return m_lpPckIndexTable;
}

CONST	LPPCK_PATH_NODE CPckClass::GetPckPathNode()
{
	return &m_RootNode;
}

QWORD CPckClass::GetPckSize()
{
	return m_PckAllInfo.qwPckSize;
}

DWORD CPckClass::GetPckFileCount()
{
	return m_PckAllInfo.dwFileCount;
}

QWORD CPckClass::GetPckDataAreaSize()
{
	return m_PckAllInfo.dwAddressName - PCK_DATA_START_AT;
}

QWORD CPckClass::GetPckRedundancyDataSize()
{
	return m_PckAllInfo.dwAddressName - PCK_DATA_START_AT - m_RootNode.child->qdwDirCipherTextSize;
}

char * CPckClass::GetAdditionalInfo()
{
	return m_PckAllInfo.szAdditionalInfo;
}

BOOL CPckClass::SetAdditionalInfo()
{
	if(0 == *m_PckAllInfo.szFilename)return FALSE;

	CMapViewFileWrite	*lpcWritefile = new CMapViewFileWrite(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->dwMaxSinglePckSize);

	if(NULL == strstr(m_PckAllInfo.szAdditionalInfo, PCK_ADDITIONAL_INFO))
	{
		strcpy(m_PckAllInfo.szAdditionalInfo,	PCK_ADDITIONAL_INFO
															PCK_ADDITIONAL_INFO_STSM);
	}

	if(!lpcWritefile->OpenPck(m_PckAllInfo.szFilename, OPEN_EXISTING))
	{
		PrintLogE(TEXT_OPENWRITENAME_FAIL, m_PckAllInfo.szFilename, __FILE__, __FUNCTION__, __LINE__);
		delete lpcWritefile;
		return FALSE;
	}

	lpcWritefile->SetFilePointer(-((QWORD)(m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize)), FILE_END);

	if(!lpcWritefile->Write(m_PckAllInfo.lpSaveAsPckVerFunc->FillTailData(&m_PckAllInfo), \
								m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize))
	{
		PrintLogE(TEXT_WRITEFILE_FAIL, __FILE__, __FUNCTION__, __LINE__);

		delete lpcWritefile;
		return FALSE;
	}

	delete lpcWritefile;
	return TRUE;

}
