#include "PckThreadRunner.h"


BOOL CPckThreadRunner::putCompressedDataQueue(PCKINDEXTABLE &cPckFileIndexToCompress)
{
#pragma region 减少内存消耗
	//用到的次数很少
	DWORD dwUnusedMemory = cPckFileIndexToCompress.dwMallocSize - cPckFileIndexToCompress.cFileIndex.dwFileCipherTextSize;
	if ((10 * 1024 * 1024) < dwUnusedMemory) {
		LPBYTE newbuf = (LPBYTE)realloc(cPckFileIndexToCompress.compressed_file_data, cPckFileIndexToCompress.cFileIndex.dwFileCipherTextSize);
		if (NULL != newbuf) {
			cPckFileIndexToCompress.compressed_file_data = newbuf;
			cPckFileIndexToCompress.dwMallocSize = cPckFileIndexToCompress.cFileIndex.dwFileCipherTextSize;
			freeMaxToSubtractMemory(dwUnusedMemory);
			Logger.logOutput(__FUNCTION__, "reduce memory usage success dwUnusedMemory = %u\r\n", dwUnusedMemory);
		}
	}

#pragma endregion

	std::lock_guard<std::mutex> lckQueue(m_LockQueue);
	m_QueueContent.push_back(cPckFileIndexToCompress);

#if PCK_DEBUG_OUTPUT

	LPBYTE lpBuffer = cPckFileIndexToCompress.compressed_file_data;
	if (MALLOCED_EMPTY_DATA == (int)lpBuffer) {
		Logger.logOutput(__FUNCTION__, "id(%05d), lpBuffer(0x%08x):0x0000000000000000, szFile(%s)\r\n", m_dwCurrentQueuePosPut, (int)lpBuffer, cPckFileIndexToCompress.cFileIndex.szFilename);
	}
	else {
		Logger.logOutput(__FUNCTION__, "id(%05d), lpBuffer(0x%08x):0x%016llx, szFile(%s)\r\n", m_dwCurrentQueuePosPut, (int)lpBuffer, _byteswap_uint64(*(unsigned __int64*)lpBuffer), cPckFileIndexToCompress.cFileIndex.szFilename);
	}

	m_dwCurrentQueuePosPut++;

	assert(NULL != lpBuffer);
#endif

	m_cvReadyToPut.notify_one();
	return TRUE;

}

BOOL CPckThreadRunner::getCompressedDataQueue(LPBYTE &lpBuffer, PCKINDEXTABLE_COMPRESS &lpPckIndexTable)
{

	std::unique_lock<std::mutex> lckQueue(m_LockQueue);
	//m_LockQueue.lock();

	while (m_QueueContent.empty()) {

		if (!m_lpPckClassBase->CheckIfNeedForcedStopWorking()) {
			Logger.logOutput(__FUNCTION__ "_Sleep", "SleepConditionVariableSRW\r\n");

			//std::unique_lock<std::mutex> lckQueue(m_LockQueue);
			m_cvReadyToPut.wait(lckQueue);
		}
		else {
			//m_LockQueue.unlock();

			Logger.logOutput(__FUNCTION__ "_Sleep", "user cancled\r\n");
			lpBuffer = NULL;
			m_Index_Compress.push_back(PCKINDEXTABLE_COMPRESS{ 0 });
			return FALSE;
		}
	}

	Logger.logOutput(__FUNCTION__ "_Sleep", "Awake\r\n");

	PCKINDEXTABLE cPckFileIndexToCompress = m_QueueContent.front();
	m_QueueContent.pop_front();
	lckQueue.unlock();


	memset(&lpPckIndexTable, 0, sizeof(PCKINDEXTABLE_COMPRESS));

	lpPckIndexTable.dwCompressedFilesize = cPckFileIndexToCompress.cFileIndex.dwFileCipherTextSize;
	lpPckIndexTable.dwMallocSize = cPckFileIndexToCompress.dwMallocSize;

	lpPckIndexTable.dwAddressFileDataToWrite = cPckFileIndexToCompress.cFileIndex.dwAddressOffset = mt_dwAddressQueue;
	mt_dwAddressQueue += cPckFileIndexToCompress.cFileIndex.dwFileCipherTextSize;

	m_lpPckClassBase->FillAndCompressIndexData(&lpPckIndexTable, &cPckFileIndexToCompress.cFileIndex);
	m_Index_Compress.push_back(lpPckIndexTable);

	Logger.logOutput(__FUNCTION__ "_m_Index_Compress", "m_Index_Compress:%d\r\n", m_Index_Compress.size());

	lpBuffer = cPckFileIndexToCompress.compressed_file_data;

#if PCK_DEBUG_OUTPUT
	assert(NULL != lpBuffer);
	if (0 != lpPckIndexTable.dwIndexDataLength) {
		if (1 == (int)lpBuffer) {
			Logger.logOutput(__FUNCTION__, "id(%05d), lpBuffer(0x%08x):0x0000000000000000, szFile(%s), esize:%d, dsize:%d\r\n",
				m_dwCurrentQueuePosGet,
				(int)lpBuffer,
				cPckFileIndexToCompress.cFileIndex.szFilename,
				cPckFileIndexToCompress.cFileIndex.dwFileCipherTextSize,
				cPckFileIndexToCompress.cFileIndex.dwFileClearTextSize);
		}
		else {
			Logger.logOutput(__FUNCTION__, "id(%05d), lpBuffer(0x%08x):0x%016llx, szFile(%s), esize:%d, dsize:%d\r\n",
				m_dwCurrentQueuePosGet,
				(int)lpBuffer,
				_byteswap_uint64(*(unsigned __int64*)lpBuffer),
				cPckFileIndexToCompress.cFileIndex.szFilename,
				cPckFileIndexToCompress.cFileIndex.dwFileCipherTextSize,
				cPckFileIndexToCompress.cFileIndex.dwFileClearTextSize);
		}
	}
	m_dwCurrentQueuePosGet++;
#endif
	return TRUE;
}

