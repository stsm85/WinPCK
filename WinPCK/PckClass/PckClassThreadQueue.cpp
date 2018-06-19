#include "PckClassThread.h"

BOOL CPckClassThreadWorker::initCompressedDataQueue(DWORD dwFileCount, QWORD dwAddressStartAt)
{

#ifdef _DEBUG
	mt_dwCurrentQueuePosPut = mt_dwCurrentQueuePosGet = 0;
#endif
	mt_dwAddressNameQueue = mt_dwAddressQueue = dwAddressStartAt;

	m_QueueContent.clear();
	m_Index_Compress.clear();

	return TRUE;
}

BOOL CPckClassThreadWorker::putCompressedDataQueue(PCKINDEXTABLE &cPckFileIndexToCompress)
{
#pragma region ¼õÉÙÄÚ´æÏûºÄ
	DWORD dwUnusedMemory = cPckFileIndexToCompress.dwMallocSize - cPckFileIndexToCompress.cFileIndex.dwFileCipherTextSize;
	if((10 * 1024 * 1024) < dwUnusedMemory) {
		LPBYTE newbuf = (LPBYTE)realloc(cPckFileIndexToCompress.compressed_file_data, cPckFileIndexToCompress.cFileIndex.dwFileCipherTextSize);
		if(NULL != newbuf) {
			cPckFileIndexToCompress.compressed_file_data = newbuf;
			cPckFileIndexToCompress.dwMallocSize = cPckFileIndexToCompress.cFileIndex.dwFileCipherTextSize;
		}
		freeMaxToSubtractMemory(dwUnusedMemory);
	}
#pragma endregion

	AcquireSRWLockExclusive(&m_LockQueue);
	m_QueueContent.push_back(cPckFileIndexToCompress);

#ifdef _DEBUG

	LPBYTE lpBuffer = cPckFileIndexToCompress.compressed_file_data;
	if(MALLOCED_EMPTY_DATA == (int)lpBuffer) {
		logOutput(__FUNCTION__, "id(%05d), lpBuffer(0x%08x):0x0000000000000000, szFile(%s)\r\n", mt_dwCurrentQueuePosPut, (int)lpBuffer, cPckFileIndexToCompress.cFileIndex.szFilename);
	} else {
		logOutput(__FUNCTION__, "id(%05d), lpBuffer(0x%08x):0x%016llx, szFile(%s)\r\n", mt_dwCurrentQueuePosPut, (int)lpBuffer, _byteswap_uint64(*(unsigned __int64*)lpBuffer), cPckFileIndexToCompress.cFileIndex.szFilename);
	}

	mt_dwCurrentQueuePosPut++;

	assert(NULL != lpBuffer);
#endif

	ReleaseSRWLockExclusive(&m_LockQueue);
	WakeConditionVariable(&m_cvReadyToPut);

	return TRUE;

}

BOOL CPckClassThreadWorker::getCompressedDataQueue(LPBYTE &lpBuffer, PCKINDEXTABLE_COMPRESS &lpPckIndexTable)
{

	AcquireSRWLockExclusive(&m_LockQueue);

	while(m_QueueContent.empty()) {

		if(m_lpPckParams->cVarParams.bThreadRunning) {
			logOutput(__FUNCTION__ "_Sleep", "SleepConditionVariableSRW\r\n");
			SleepConditionVariableSRW(&m_cvReadyToPut, &m_LockQueue, INFINITE, 0);
		} else {
			ReleaseSRWLockExclusive(&m_LockQueue);
			logOutput(__FUNCTION__ "_Sleep", "user cancled\r\n");
			lpBuffer = NULL;
			m_Index_Compress.push_back(PCKINDEXTABLE_COMPRESS{ 0 });
			return FALSE;
		}
	}

	logOutput(__FUNCTION__ "_Sleep", "Awake\r\n");

	PCKINDEXTABLE cPckFileIndexToCompress = m_QueueContent.front();
	m_QueueContent.pop_front();
	ReleaseSRWLockExclusive(&m_LockQueue);

	memset(&lpPckIndexTable, 0, sizeof(PCKINDEXTABLE_COMPRESS));

	lpPckIndexTable.dwCompressedFilesize = cPckFileIndexToCompress.cFileIndex.dwFileCipherTextSize;
	lpPckIndexTable.dwMallocSize = cPckFileIndexToCompress.dwMallocSize;

	lpPckIndexTable.dwAddressFileDataToWrite = cPckFileIndexToCompress.cFileIndex.dwAddressOffset = mt_dwAddressQueue;
	mt_dwAddressQueue += cPckFileIndexToCompress.cFileIndex.dwFileCipherTextSize;

	FillAndCompressIndexData(&lpPckIndexTable, &cPckFileIndexToCompress.cFileIndex);
	m_Index_Compress.push_back(lpPckIndexTable);
#ifdef _DEBUG
	logOutput(__FUNCTION__ "_m_Index_Compress", "m_Index_Compress:%d\r\n", m_Index_Compress.size());
#endif
	lpBuffer = cPckFileIndexToCompress.compressed_file_data;

#ifdef _DEBUG
	assert(NULL != lpBuffer);
	if(0 != lpPckIndexTable.dwIndexDataLength) {
		if(1 == (int)lpBuffer) {
			logOutput(__FUNCTION__, "id(%05d), lpBuffer(0x%08x):0x0000000000000000, szFile(%s)\r\n", mt_dwCurrentQueuePosGet, (int)lpBuffer, cPckFileIndexToCompress.cFileIndex.szFilename);
		} else {
			logOutput(__FUNCTION__, "id(%05d), lpBuffer(0x%08x):0x%016llx, szFile(%s)\r\n", mt_dwCurrentQueuePosGet, (int)lpBuffer, _byteswap_uint64(*(unsigned __int64*)lpBuffer), cPckFileIndexToCompress.cFileIndex.szFilename);
		}
	}
	mt_dwCurrentQueuePosGet++;
#endif
	return TRUE;
}

void CPckClassThreadWorker::uninitCompressedDataQueue(int threadnum)
{
	m_QueueContent.clear();
	m_Index_Compress.clear();
}
