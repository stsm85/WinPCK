#include "PckClassThreadHelper.h"

#if ! USE_OLD_QUEUE

BOOL CPckClassThreadHelper::initCompressedDataQueue(int threadnum, DWORD dwFileCount, QWORD dwAddressStartAt)
{
#if 0
	//申请空间,文件名压缩数据 数组
	if(NULL == (mt_lpPckIndexTable = new PCKINDEXTABLE_COMPRESS[dwFileCount])) {
		m_PckLog.PrintLogEL(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
		assert(FALSE);
		return FALSE;
	}
	memset(mt_lpPckIndexTable, 0, sizeof(PCKINDEXTABLE_COMPRESS) * dwFileCount);

	if(NULL == (mt_pckCompressedDataPtrArray = (BYTE**)malloc(sizeof(BYTE*) * (dwFileCount + 1)))) {
		m_PckLog.PrintLogEL(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
		assert(FALSE);
		return FALSE;
	}
	memset(mt_pckCompressedDataPtrArray, 0, sizeof(BYTE*) * (dwFileCount + 1));

	mt_lpPckIndexTablePut = mt_lpPckIndexTableGet = mt_lpPckIndexTable;
	mt_pckCompressedDataPtrArrayPut = mt_pckCompressedDataPtrArrayGet = mt_pckCompressedDataPtrArray;
	mt_dwMaxQueueLength = dwFileCount;
#ifdef _DEBUG
	mt_dwCurrentQueuePosPut = mt_dwCurrentQueuePosGet = 0;
#endif
	mt_dwCurrentQueueLength = 0;
#endif

	mt_lpIndexData = new CPckClassIndexDataAppend();
	mt_CompressedIndexs.clear();
	mt_dwAddressNameQueue = mt_dwAddressQueue = dwAddressStartAt;


	return TRUE;
}

BOOL CPckClassThreadHelper::putCompressedDataQueue(LPBYTE lpBuffer, LPPCKINDEXTABLE_COMPRESS lpPckIndexTableComped, LPPCKFILEINDEX lpPckFileIndexToCompress)
{
	//先不考虑ptr>count的情况
	if(!lpPckIndexTableComped->bInvalid) {
		lpPckIndexTableComped->dwCompressedFilesize = lpPckFileIndexToCompress->dwFileCipherTextSize;
		EnterCriticalSection(&g_mt_pckCompressedDataPtrArrayPtr);
		//lpPckIndexTableComped->dwAddressFileDataToWrite = lpPckFileIndexToCompress->dwAddressOffset = mt_dwAddressQueue;

		//是否使用老数据区（使用时说明更新的文件压缩后的数据比之前的要小）
		if(0 == lpPckIndexTableComped->dwAddressOfDuplicateOldDataArea) {
			lpPckIndexTableComped->dwAddressFileDataToWrite = lpPckFileIndexToCompress->dwAddressOffset = mt_dwAddressQueue;
			mt_dwAddressQueue += lpPckFileIndexToCompress->dwFileCipherTextSize;
			lpPckIndexTableComped->dwAddressAddStep = lpPckFileIndexToCompress->dwFileCipherTextSize;
		} else {
			lpPckIndexTableComped->dwAddressFileDataToWrite = lpPckIndexTableComped->dwAddressOfDuplicateOldDataArea;
			lpPckIndexTableComped->dwAddressAddStep = 0;
		}


		FillAndCompressIndexData(lpPckIndexTableComped, lpPckFileIndexToCompress);


#ifdef _DEBUG
#if 0
		if(1 == (int)lpBuffer) {
			logOutput(__FUNCTION__, formatString("id(%05d), lpBuffer(0x%08x):0x0000000000000000, lpPckIndexTable(0x%08x), szFile(%s)\r\n", mt_dwCurrentQueuePosPut, (int)lpBuffer, (int)mt_lpPckIndexTablePut, lpPckFileIndexToCompress->szFilename));
		} else {
			logOutput(__FUNCTION__, formatString("id(%05d), lpBuffer(0x%08x):0x%016llx, lpPckIndexTable(0x%08x), szFile(%s)\r\n", mt_dwCurrentQueuePosPut, (int)lpBuffer, _byteswap_uint64(*(unsigned __int64*)lpBuffer), (int)mt_lpPckIndexTablePut, lpPckFileIndexToCompress->szFilename));
		}

		mt_dwCurrentQueuePosPut++;

		assert(NULL != lpBuffer);
#endif
#endif

		lpPckIndexTableComped->compressed_file_data = lpBuffer;
		mt_lpIndexData->append()

		//(*mt_pckCompressedDataPtrArrayPut) = lpBuffer;
		//memcpy(mt_lpPckIndexTablePut, lpPckIndexTableComped, sizeof(PCKINDEXTABLE_COMPRESS));

		mt_dwCurrentQueueLength++;
		mt_lpPckIndexTablePut++;
		mt_pckCompressedDataPtrArrayPut++;
		LeaveCriticalSection(&g_mt_pckCompressedDataPtrArrayPtr);
	}
	return TRUE;
}

#else
BOOL CPckClassThreadHelper::initCompressedDataQueue(int threadnum, DWORD dwFileCount, QWORD dwAddressStartAt)
{
	//申请空间,文件名压缩数据 数组
	if(NULL == (mt_lpPckIndexTable = new PCKINDEXTABLE_COMPRESS[dwFileCount])) {
		m_PckLog.PrintLogEL(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
		assert(FALSE);
		return FALSE;
	}
	memset(mt_lpPckIndexTable, 0, sizeof(PCKINDEXTABLE_COMPRESS) * dwFileCount);

	if(NULL == (mt_pckCompressedDataPtrArray = (BYTE**)malloc(sizeof(BYTE*) * (dwFileCount + 1)))) {
		m_PckLog.PrintLogEL(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
		assert(FALSE);
		return FALSE;
	}
	memset(mt_pckCompressedDataPtrArray, 0, sizeof(BYTE*) * (dwFileCount + 1));

	mt_lpPckIndexTablePut = mt_lpPckIndexTableGet = mt_lpPckIndexTable;
	mt_pckCompressedDataPtrArrayPut = mt_pckCompressedDataPtrArrayGet = mt_pckCompressedDataPtrArray;
	mt_dwMaxQueueLength = dwFileCount;
#ifdef _DEBUG
	mt_dwCurrentQueuePosPut = mt_dwCurrentQueuePosGet = 0;
#endif
	mt_dwCurrentQueueLength = 0;
	mt_dwAddressNameQueue = mt_dwAddressQueue = dwAddressStartAt;


	return TRUE;
}

BOOL CPckClassThreadHelper::putCompressedDataQueue(LPBYTE lpBuffer, LPPCKINDEXTABLE_COMPRESS lpPckIndexTableComped, LPPCKFILEINDEX lpPckFileIndexToCompress)
{
	//先不考虑ptr>count的情况

	lpPckIndexTableComped->dwCompressedFilesize = lpPckFileIndexToCompress->dwFileCipherTextSize;
	EnterCriticalSection(&g_mt_pckCompressedDataPtrArrayPtr);
	//lpPckIndexTableComped->dwAddressFileDataToWrite = lpPckFileIndexToCompress->dwAddressOffset = mt_dwAddressQueue;

	//是否使用老数据区（使用时说明更新的文件压缩后的数据比之前的要小）
	if(0 == lpPckIndexTableComped->dwAddressOfDuplicateOldDataArea) {
		lpPckIndexTableComped->dwAddressFileDataToWrite = lpPckFileIndexToCompress->dwAddressOffset = mt_dwAddressQueue;
		mt_dwAddressQueue += lpPckFileIndexToCompress->dwFileCipherTextSize;
		lpPckIndexTableComped->dwAddressAddStep = lpPckFileIndexToCompress->dwFileCipherTextSize;
	} else {
		lpPckIndexTableComped->dwAddressFileDataToWrite = lpPckIndexTableComped->dwAddressOfDuplicateOldDataArea;
		lpPckIndexTableComped->dwAddressAddStep = 0;
	}

	if(!lpPckIndexTableComped->bInvalid) {
		FillAndCompressIndexData(lpPckIndexTableComped, lpPckFileIndexToCompress);
	}

#ifdef _DEBUG
	if(1 == (int)lpBuffer) {
		logOutput(__FUNCTION__, formatString("id(%05d), lpBuffer(0x%08x):0x0000000000000000, lpPckIndexTable(0x%08x), szFile(%s)\r\n", mt_dwCurrentQueuePosPut, (int)lpBuffer, (int)mt_lpPckIndexTablePut, lpPckFileIndexToCompress->szFilename));
	} else {
		logOutput(__FUNCTION__, formatString("id(%05d), lpBuffer(0x%08x):0x%016llx, lpPckIndexTable(0x%08x), szFile(%s)\r\n", mt_dwCurrentQueuePosPut, (int)lpBuffer, _byteswap_uint64(*(unsigned __int64*)lpBuffer), (int)mt_lpPckIndexTablePut, lpPckFileIndexToCompress->szFilename));
	}

	mt_dwCurrentQueuePosPut++;

	assert(NULL != lpBuffer);
#endif

	(*mt_pckCompressedDataPtrArrayPut) = lpBuffer;
	memcpy(mt_lpPckIndexTablePut, lpPckIndexTableComped, sizeof(PCKINDEXTABLE_COMPRESS));

	mt_dwCurrentQueueLength++;
	mt_lpPckIndexTablePut++;
	mt_pckCompressedDataPtrArrayPut++;
	LeaveCriticalSection(&g_mt_pckCompressedDataPtrArrayPtr);

	return TRUE;
}

BOOL CPckClassThreadHelper::getCompressedDataQueue(LPBYTE &lpBuffer, LPPCKINDEXTABLE_COMPRESS &lpPckIndexTable)
{

	EnterCriticalSection(&g_mt_pckCompressedDataPtrArrayPtr);
	if(mt_dwCurrentQueueLength) {

		mt_dwCurrentQueueLength--;
		lpBuffer = *mt_pckCompressedDataPtrArrayGet;
		lpPckIndexTable = mt_lpPckIndexTableGet;

		LeaveCriticalSection(&g_mt_pckCompressedDataPtrArrayPtr);

#ifdef _DEBUG

		assert(NULL != lpBuffer);

		if(0 != lpPckIndexTable->dwIndexDataLength) {
			char szFile[MAX_INDEXTABLE_CLEARTEXT_LENGTH];
			DWORD dwTest = MAX_INDEXTABLE_CLEARTEXT_LENGTH;

			decompress(szFile, &dwTest, lpPckIndexTable->buffer, lpPckIndexTable->dwIndexDataLength);
			if(1 == (int)lpBuffer) {
				logOutput(__FUNCTION__, formatString("id(%05d), lpBuffer(0x%08x):0x0000000000000000, lpPckIndexTable(0x%08x), szFile(%s)\r\n", mt_dwCurrentQueuePosGet, (int)lpBuffer, (int)lpPckIndexTable, szFile));
			} else {
				logOutput(__FUNCTION__, formatString("id(%05d), lpBuffer(0x%08x):0x%016llx, lpPckIndexTable(0x%08x), szFile(%s)\r\n", mt_dwCurrentQueuePosGet, (int)lpBuffer, _byteswap_uint64(*(unsigned __int64*)lpBuffer), (int)lpPckIndexTable, szFile));
			}
		}
		mt_dwCurrentQueuePosGet++;
#endif

		mt_lpPckIndexTableGet++;
		mt_pckCompressedDataPtrArrayGet++;

		return TRUE;
	} else {
		LeaveCriticalSection(&g_mt_pckCompressedDataPtrArrayPtr);
		return FALSE;
	}
}

void CPckClassThreadHelper::uninitCompressedDataQueue(int threadnum)
{
	delete[] mt_lpPckIndexTable;

	free(mt_pckCompressedDataPtrArray);
}

#endif