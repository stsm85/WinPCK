#include <pch.h>
#include "PckThreadRunner.h"



CPckThreadRunner::CPckThreadRunner(LPTHREAD_PARAMS threadparams) :
	m_threadparams(threadparams)
{
	m_lpPckParams = threadparams->pckParams;
	m_lpPckClassBase = threadparams->lpPckClassThreadWorker;

	m_lpPckClassBase->SetThreadFlag(TRUE);
	mt_dwAddressNameQueue = mt_dwAddressQueue = m_threadparams->dwAddressStartAt;
}


CPckThreadRunner::~CPckThreadRunner()
{
	Logger->trace(std::source_location::current().function_name());
}

void CPckThreadRunner::start()
{

	try {

		startThread();

		m_lpPckParams->cVarParams.dwMTMemoryUsed = 0;
		m_threadparams->lpPckAllInfo->lpPckIndexTableToAdd = &m_Index_Compress;
		m_threadparams->lpPckAllInfo->dwFileCountToAdd = m_threadparams->dwFileCountOfWriteTarget;
		m_threadparams->lpPckAllInfo->dwAddressOfFileEntry = mt_dwAddressQueue;

		uint64_t qwAddress = m_threadparams->lpPckAllInfo->dwAddressOfFileEntry;

		m_lpPckClassBase->WriteAllIndex(m_threadparams->lpFileWrite, m_threadparams->lpPckAllInfo, qwAddress);
		m_lpPckClassBase->WriteHeadAndTail(m_threadparams->lpFileWrite, m_threadparams->lpPckAllInfo, qwAddress);

	}
	catch (MyException ex)
	{
		;
	}
	m_lpPckClassBase->SetThreadFlag(FALSE);
}

void CPckThreadRunner::startThread()
{

	FETCHDATA_FUNC pGetUncompressedData = nullptr;
	if (DATA_FROM_FILE == m_threadparams->pck_data_src)
		pGetUncompressedData = std::bind(&CPckThreadRunner::GetUncompressedDataFromFile, this, &(m_threadparams->cDataFetchMethod), std::placeholders::_1);
	else if (DATA_FROM_PCK == m_threadparams->pck_data_src)
		pGetUncompressedData = std::bind(&CPckThreadRunner::GetUncompressedDataFromPCK, this, &(m_threadparams->cDataFetchMethod), std::placeholders::_1);
	else
		throw MyExceptionEx("pck_data_src is invalid");

	std::vector<std::thread> threads;

	CompressThreadFunc pCompressThread = std::bind(&CPckThreadRunner::CompressThread, this, pGetUncompressedData);
	WriteThreadFunc pWriteThread = std::bind(&CPckThreadRunner::WriteThread, this, m_threadparams);

	int threadnum = m_threadparams->pckParams->dwMTThread;

	for (int i = 0; i < threadnum; i++) {
		threads.push_back(std::thread(pCompressThread));
	}
	std::thread t(pWriteThread);

	std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
	//should note the TEXT_LOG_FLUSH_CACHE message
	Logger.off(TEXT_LOG_FLUSH_CACHE);
	//Logger.info(TEXT_LOG_FLUSH_CACHE);
	if (t.joinable())
		t.join();
}

void CPckThreadRunner::WriteThread(LPTHREAD_PARAMS threadparams)
{
	uint8_t*		dataToWrite = NULL;
	uint64_t		dwTotalFileSizeAfterCompress = threadparams->qwCompressTotalFileSize;
	CMapViewFileMultiPckWrite*	lpFileWrite = threadparams->lpFileWrite;

	PCKINDEXTABLE_COMPRESS	lpPckIndexTableComp;

	uint64_t			dwAddressDataAreaEndAt = mt_dwAddressNameQueue;
	uint32_t			nWrite = 0;
	BOOL				result = TRUE;


	for (nWrite = 0; nWrite < threadparams->dwFileCountOfWriteTarget; nWrite++) {

		if (!getCompressedDataQueue(dataToWrite, lpPckIndexTableComp)) {
			//user cacle
			result = FALSE;
			break;
		}

		uint64_t dwAddress = lpPckIndexTableComp.dwAddressFileDataToWrite;

		//处理lpPckFileIndex->dwAddressOffset
		if (0 != lpPckIndexTableComp.dwCompressedFilesize) {

			if (!lpFileWrite->Write2(dwAddress, dataToWrite, lpPckIndexTableComp.dwCompressedFilesize)) {
				m_lpPckClassBase->SetErrMsgFlag(PCK_ERR_VIEW);
				result = FALSE;
				break;
			}

			dwAddressDataAreaEndAt += lpPckIndexTableComp.dwCompressedFilesize;
			freeMaxAndSubtractMemory(dataToWrite, lpPckIndexTableComp.dwMallocSize);
		}

	}

	mt_dwAddressQueue = dwAddressDataAreaEndAt;


#pragma region 写线程失败处理

	if (!result) {

		Logger->error("{} Finished with errors", std::source_location::current().function_name());

		threadparams->dwFileCountOfWriteTarget = nWrite;

		if (NULL != dataToWrite)
			free(dataToWrite);

		uint32_t nQueueLen = m_QueueContent.size();
		Logger->error("{} _free", "m_QueueContent.size() = {}",std::source_location::current().function_name(), nQueueLen);

		for (uint32_t i = 0; i < nQueueLen; i++) {

			PCKINDEXTABLE *lpPckIndex = &m_QueueContent[i];
			if (MALLOCED_EMPTY_DATA != (int)lpPckIndex->compressed_file_data) {
				Logger->error("{} _free, free buffer(0x{:016x})", std::source_location::current().function_name(), (LONG_PTR)lpPckIndex->compressed_file_data);
				free(lpPckIndex->compressed_file_data);
			}
		}

		m_QueueContent.clear();
		m_lpPckParams->cVarParams.dwMTMemoryUsed = 0;
		m_cvMemoryNotEnough.notify_all();

		m_Index_Compress.pop_back();

	}
#pragma endregion
	Logger->info("WriteThread Finished");

	return;
}

void CPckThreadRunner::CompressThread(FETCHDATA_FUNC GetUncompressedData)
{

#if PCK_DEBUG_OUTPUT
	{
		std::lock_guard<std::mutex> lckThreadID(m_LockThreadID);
		m_threadID++;
		Logger->debug("{} mt_threadID++, {}", std::source_location::current().function_name(), m_threadID);
	}
#endif

	PCKINDEXTABLE		pckFileIndex = { 0 };

	//取得压缩好的数据
	while (FD_OK == GetUncompressedData(pckFileIndex)) {

		//窗口中以显示的文件进度
		m_lpPckClassBase->SetParams_ProgressInc();

		//放入队列
		putCompressedDataQueue(pckFileIndex);

	}

#if PCK_DEBUG_OUTPUT
	{
		std::lock_guard<std::mutex> lckThreadID(m_LockThreadID);
		m_threadID--;
		Logger->debug("{} mt_threadID--, {}", std::source_location::current().function_name(), m_threadID);
	}
#endif

	m_cvReadyToPut.notify_one();
	Logger->trace("{} WakeConditionVariable(m_cvReadyToPut);", std::source_location::current().function_name());
	return;
}
