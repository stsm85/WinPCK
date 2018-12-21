
#include "PckClassThread.h"

CPckClassThreadWorker::CPckClassThreadWorker()
{
	DWORD	dwCurrentPID = GetCurrentProcessId();

	sprintf_s(m_szEventAllWriteFinish, 16, TEXT_EVENT_WRITE_PCK_DATA_FINISH, dwCurrentPID);

}

CPckClassThreadWorker::~CPckClassThreadWorker()
{}

BOOL CPckClassThreadWorker::CheckIfNeedForcedStopWorking()
{
	if (m_lpPckParams->cVarParams.bForcedStopWorking)
		SetErrMsgFlag(PCK_MSG_USERCANCELED);

	return m_lpPckParams->cVarParams.bForcedStopWorking;
}

BOOL CPckClassThreadWorker::initMultiThreadVars(CMapViewFileMultiPckWrite *lpFileWriter)
{
	//多线程使用参数初始化
	SetThreadFlag(TRUE);
	mt_lpFileWrite = lpFileWriter;
	mt_evtAllWriteFinish = CreateEventA(NULL, FALSE, FALSE, m_szEventAllWriteFinish);
	mt_threadID = 0;
	m_memoryNotEnoughBlocked = FALSE;

	return TRUE;
}

void CPckClassThreadWorker::ExecuteMainThreadGroup(PCK_ALL_INFOS &pckAllInfo, int threadnum, void* threadparams)
{

	InitializeSRWLock(&g_mt_LockCompressedflag);
	InitializeSRWLock(&g_mt_LockThreadID);
	InitializeSRWLock(&g_mt_LockReadFileMap);

	InitializeSRWLock(&m_LockMaxMemory);
	InitializeSRWLock(&m_LockQueue);
	InitializeConditionVariable(&m_cvReadyToPut);
	InitializeConditionVariable(&m_cvReadThreadComplete);
	InitializeConditionVariable(&m_cvMemoryNotEnough);

	for(int i = 0; i < threadnum; i++) {
		_beginthread(CompressThread, 0, threadparams);
	}
	_beginthread(WriteThread, 0, this);

	::Sleep(100);
	
	//待写线程完成
	AcquireSRWLockShared(&g_mt_LockThreadID);
	//是否有可能mt_threadID还没开始加已经到这里了
	while(0 != mt_threadID) {
		logOutput(__FUNCTION__ "_Sleep", "SleepConditionVariableSRW, %d\r\n", mt_threadID);
		SleepConditionVariableSRW(&m_cvReadThreadComplete, &g_mt_LockThreadID, INFINITE, CONDITION_VARIABLE_LOCKMODE_SHARED);
	}
	ReleaseSRWLockShared(&g_mt_LockThreadID);

	logOutput(__FUNCTION__ "_Sleep", "Awake\r\n");

	m_PckLog.PrintLogN(TEXT_LOG_FLUSH_CACHE);

	WaitForSingleObject(mt_evtAllWriteFinish, INFINITE);
	CloseHandle(mt_evtAllWriteFinish);

	pckAllInfo.lpPckIndexTableToAdd = &m_Index_Compress;
	pckAllInfo.dwFileCountToAdd = mt_dwFileCountOfWriteTarget;
	pckAllInfo.dwAddressOfFilenameIndex = mt_dwAddressQueue;

}


VOID CPckClassThreadWorker::WriteThread(VOID* pParam)
{
	CPckClassThreadWorker *pThis = (CPckClassThreadWorker*)pParam;

	LPBYTE		dataToWrite = NULL;
	QWORD		dwTotalFileSizeAfterCompress = mt_CompressTotalFileSize;

	PCKINDEXTABLE_COMPRESS	lpPckIndexTableComp;

	QWORD						dwAddressDataAreaEndAt = mt_dwAddressNameQueue;
	DWORD						nWrite = 0;
	BOOL						result = TRUE;

	for(nWrite = 0;nWrite<mt_dwFileCountOfWriteTarget;nWrite++){

		if(!pThis->getCompressedDataQueue(dataToWrite, lpPckIndexTableComp)) {
			//user cacle
			result = FALSE;
			break;
		}

		QWORD dwAddress = lpPckIndexTableComp.dwAddressFileDataToWrite;

		//处理lpPckFileIndex->dwAddressOffset
		if(0 != lpPckIndexTableComp.dwCompressedFilesize) {

			if (!mt_lpFileWrite->Write2(dwAddress, dataToWrite, lpPckIndexTableComp.dwCompressedFilesize)) {
				pThis->SetErrMsgFlag(PCK_ERR_VIEW);
				result = FALSE;
				break;
			}

			dwAddressDataAreaEndAt += lpPckIndexTableComp.dwCompressedFilesize;
			pThis->freeMaxAndSubtractMemory(dataToWrite, lpPckIndexTableComp.dwMallocSize);
		}

	}

	AcquireSRWLockShared(&g_mt_LockThreadID);
	while(0 != mt_threadID) {
		logOutput(__FUNCTION__, "SleepConditionVariableSRW, %d\r\n", mt_threadID);
		SleepConditionVariableSRW(&pThis->m_cvReadThreadComplete, &g_mt_LockThreadID, INFINITE, CONDITION_VARIABLE_LOCKMODE_SHARED);
	}
	ReleaseSRWLockShared(&g_mt_LockThreadID);

	mt_dwAddressQueue = dwAddressDataAreaEndAt;


#pragma region 写线程失败处理

	if(!result) {

		logOutput(__FUNCTION__, "Finished with errs\r\n");

		mt_dwFileCountOfWriteTarget = nWrite;

		if(NULL != dataToWrite)
			free(dataToWrite);

		size_t nQueueLen = pThis->m_QueueContent.size();
		logOutput(__FUNCTION__ "_free", "m_QueueContent.size() = %d\r\n", nQueueLen);

		for(size_t i = 0;i < nQueueLen;i++) {

			PCKINDEXTABLE *lpPckIndex = &pThis->m_QueueContent[i];
			if(MALLOCED_EMPTY_DATA != (int)lpPckIndex->compressed_file_data) {
				logOutput(__FUNCTION__ "_free", "free buffer(0x%08x)\r\n", (int)lpPckIndex->compressed_file_data);
				free(lpPckIndex->compressed_file_data);
			}
		}

		pThis->m_QueueContent.clear();
		pThis->m_lpPckParams->cVarParams.dwMTMemoryUsed = 0;
		WakeAllConditionVariable(&pThis->m_cvMemoryNotEnough);

		pThis->m_Index_Compress.pop_back();

	}
#pragma endregion
	logOutput(__FUNCTION__, "Finished\r\n");

	SetEvent(mt_evtAllWriteFinish);
	return;
}

VOID CPckClassThreadWorker::CompressThread(VOID* pParam)
{
	THREAD_PARAMS *lpThreadParams = (THREAD_PARAMS*)pParam;
	CPckClassThreadWorker *pThis = (CPckClassThreadWorker*)lpThreadParams->pThis;

	AcquireSRWLockExclusive(&g_mt_LockThreadID);
	mt_threadID++;
	logOutput(__FUNCTION__, "mt_threadID++, %d\r\n", mt_threadID);
	ReleaseSRWLockExclusive(&g_mt_LockThreadID);

	PCKINDEXTABLE		pckFileIndex = { 0 };
	//FETCHDATA_RET		rtn_GetCompressData;

	//取得压缩好的数据
	while(FD_OK == (lpThreadParams->GetUncompressedData)(pThis, &lpThreadParams->cDataFetchMethod, pckFileIndex)) {

		//窗口中以显示的文件进度
		pThis->SetParams_ProgressInc();

		//放入队列
		pThis->putCompressedDataQueue(pckFileIndex);

	}

	//if(FD_ERR == rtn_GetCompressData)
	//	pThis->SetThreadWorkerCanceled();

	AcquireSRWLockExclusive(&g_mt_LockThreadID);
	mt_threadID--;
	logOutput(__FUNCTION__, "mt_threadID--, %d\r\n", mt_threadID);
	ReleaseSRWLockExclusive(&g_mt_LockThreadID);

	WakeConditionVariable(&pThis->m_cvReadyToPut);
	logOutput(__FUNCTION__, "WakeConditionVariable(&pThis->m_cvReadyToPut);\r\n");
	WakeAllConditionVariable(&pThis->m_cvReadThreadComplete);
	logOutput(__FUNCTION__, "WakeAllConditionVariable(&pThis->m_cvReadThreadComplete);\r\n");

	return;
}
