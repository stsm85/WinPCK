//#include <Windows.h>
#include "PckClassBaseFeatures.h"
//#include <tchar.h>

CPckClassBaseFeatures::CPckClassBaseFeatures():
	m_PckAllInfo({ 0 }),
	m_lpPckParams(NULL),
	m_NodeMemPool(10 * 1024 * 1024)
{
	m_PckAllInfo.cRootNode.entryType = PCK_ENTRY_TYPE_NODE | PCK_ENTRY_TYPE_FOLDER | PCK_ENTRY_TYPE_ROOT;
}

CPckClassBaseFeatures::~CPckClassBaseFeatures()
{
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

void CPckClassBaseFeatures::ResetPckInfos()
{
	memset(&m_PckAllInfo, 0, sizeof(PCK_ALL_INFOS));
	m_PckAllInfo.dwAddressOfFileEntry = PCK_DATA_START_AT;
}

CONST	LPPCKINDEXTABLE CPckClassBaseFeatures::GetPckIndexTable()
{
	return m_PckAllInfo.lpPckIndexTable;
}

CONST	LPPCK_PATH_NODE CPckClassBaseFeatures::GetPckPathNode()
{
	return &m_PckAllInfo.cRootNode;
}

BOOL CPckClassBaseFeatures::isFileLoaded()
{
	return m_PckAllInfo.isPckFileLoaded;
}

uint64_t CPckClassBaseFeatures::GetPckSize()
{
	return m_PckAllInfo.qwPckSize;
}

uint32_t CPckClassBaseFeatures::GetPckFileCount()
{
	return m_PckAllInfo.dwFileCount;
}

uint64_t CPckClassBaseFeatures::GetPckDataAreaSize()
{
	return m_PckAllInfo.dwAddressOfFileEntry - PCK_DATA_START_AT;
}

uint64_t CPckClassBaseFeatures::GetPckRedundancyDataSize()
{
	return m_PckAllInfo.dwAddressOfFileEntry - PCK_DATA_START_AT - m_PckAllInfo.cRootNode.child->qdwDirCipherTextSize;
}


#pragma region UI_Params
void CPckClassBaseFeatures::SetParams_ProgressInc()
{
	++m_lpPckParams->cVarParams.dwUIProgress;
}
void CPckClassBaseFeatures::SetParams_Progress(DWORD dwUIProgress)
{
	m_lpPckParams->cVarParams.dwUIProgress = dwUIProgress;
}

void CPckClassBaseFeatures::SetParams_ProgressUpper(DWORD dwUIProgressUpper, BOOL bReset)
{
	if(bReset)
		m_lpPckParams->cVarParams.dwUIProgress = 0;
	m_lpPckParams->cVarParams.dwUIProgressUpper = dwUIProgressUpper;
}

void CPckClassBaseFeatures::AddParams_ProgressUpper(DWORD dwUIProgressUpperAdd)
{
	m_lpPckParams->cVarParams.dwUIProgressUpper += dwUIProgressUpperAdd;
}
#pragma endregion

#pragma region Thread Params
void CPckClassBaseFeatures::SetThreadFlag(BOOL isThreadWorking)
{
	//if (FALSE == isThreadWorking)
	//在开始和结束任务后，把强制退出标记置0
	//m_lpPckParams->cVarParams.bForcedStopWorking = FALSE;
	SetErrMsgFlag(PCK_OK);
	m_lpPckParams->cVarParams.bThreadRunning = isThreadWorking;
}

BOOL CPckClassBaseFeatures::CheckIfNeedForcedStopWorking()
{
	if (m_lpPckParams->cVarParams.bForcedStopWorking)
		SetErrMsgFlag(PCK_MSG_USERCANCELED);

	return m_lpPckParams->cVarParams.bForcedStopWorking;
}

void CPckClassBaseFeatures::SetErrMsgFlag(int errMsg)
{
	m_lpPckParams->cVarParams.bForcedStopWorking = (PCK_OK != errMsg);
	m_lpPckParams->cVarParams.errMessageNo = errMsg;
}
#pragma endregion




