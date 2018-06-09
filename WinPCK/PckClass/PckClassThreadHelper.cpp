
#include "PckClassThreadHelper.h"

CPckClassThreadHelper::CPckClassThreadHelper()
{
	DWORD	dwCurrentPID = GetCurrentProcessId();

	sprintf_s(m_szEventAllWriteFinish, 16, TEXT_EVENT_WRITE_PCK_DATA_FINISH, dwCurrentPID);
	sprintf_s(m_szEventAllCompressFinish, 16, TEXT_EVENT_COMPRESS_PCK_DATA_FINISH, dwCurrentPID);
	sprintf_s(m_szEventMaxMemory, 16, TEXT_EVENT_PCK_MAX_MEMORY, dwCurrentPID);
}

CPckClassThreadHelper::~CPckClassThreadHelper()
{}

BOOL CPckClassThreadHelper::BeforeMultiThreadProcess(CMapViewFileWrite *lpFileWriter)
{
#if 0
	//构造头和尾时需要的参数
	//memset(lpPckAllInfo, 0, sizeof(PCK_ALL_INFOS));
	//lpPckAllInfo->lpSaveAsPckVerFunc = m_PckAllInfo.lpSaveAsPckVerFunc;

	//开始压缩
	//以下是创建一个文件，用来保存压缩后的文件
	lpWrite = new CMapViewFileWrite(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->dwMaxSinglePckSize);

	//OPEN_ALWAYS，新建新的pck(CREATE_ALWAYS)或更新存在的pck(OPEN_EXISTING)
	if(!lpWrite->OpenPckAndMappingWrite(szPckFile, dwCreationDisposition, qdwSizeToMap)) {

		delete lpWrite;
		return FALSE;
	}
#endif
	//多线程使用参数初始化
	mt_lpFileWrite = lpFileWriter;
	mt_lpbThreadRunning = &m_lpPckParams->cVarParams.bThreadRunning;	//监视线程暂停
	mt_lpdwCount = &m_lpPckParams->cVarParams.dwUIProgress;			//dwCount;
	mt_MaxMemory = m_lpPckParams->dwMTMaxMemory;						//可用最大缓存;
	mt_lpMaxMemory = &m_lpPckParams->cVarParams.dwMTMemoryUsed;		//已使用缓存;
	mt_nMallocBlocked = 0;											//等待的线程数

	return TRUE;
}

//写线程失败后的处理
void CPckClassThreadHelper::AfterWriteThreadFailProcess(BOOL *lpbThreadRunning, HANDLE hevtAllCompressFinish, DWORD &dwFileCount, DWORD dwFileHasBeenWritten, QWORD &dwAddressFinal, QWORD dwAddress, BYTE **bufferPtrToWrite)
{

	*(lpbThreadRunning) = FALSE;

	WaitForSingleObject(hevtAllCompressFinish, INFINITE);
	dwFileCount = dwFileHasBeenWritten;
	dwAddressFinal = dwAddress;

	//释放
	while(0 != *bufferPtrToWrite) {
		if(1 != (int)(*bufferPtrToWrite))
			free(*bufferPtrToWrite);
		bufferPtrToWrite++;
	}

}

void CPckClassThreadHelper::MultiThreadInitialize(VOID CompressThread(VOID*), VOID WriteThread(VOID*), int threadnum)
{
	mt_evtAllWriteFinish = CreateEventA(NULL, FALSE, FALSE, m_szEventAllWriteFinish);
	mt_evtAllCompressFinish = CreateEventA(NULL, TRUE, FALSE, m_szEventAllCompressFinish);
	mt_evtMaxMemory = CreateEventA(NULL, FALSE, FALSE, m_szEventMaxMemory);

	mt_threadID = 0;

	InitializeCriticalSection(&g_cs);
	InitializeCriticalSection(&g_mt_threadID);
	InitializeCriticalSection(&g_mt_nMallocBlocked);
	InitializeCriticalSection(&g_mt_lpMaxMemory);
	InitializeCriticalSection(&g_dwCompressedflag);
	InitializeCriticalSection(&g_mt_pckCompressedDataPtrArrayPtr);

	for(int i = 0; i < threadnum; i++) {
		_beginthread(CompressThread, 0, this);
	}
	_beginthread(WriteThread, 0, this);

	WaitForSingleObject(mt_evtAllCompressFinish, INFINITE);

	//SetEvent(mt_evtToWrite);		//防止Write线程卡住 
	WaitForSingleObject(mt_evtAllWriteFinish, INFINITE);

	DeleteCriticalSection(&g_cs);
	DeleteCriticalSection(&g_mt_threadID);
	DeleteCriticalSection(&g_mt_nMallocBlocked);
	DeleteCriticalSection(&g_mt_lpMaxMemory);
	DeleteCriticalSection(&g_dwCompressedflag);
	DeleteCriticalSection(&g_mt_pckCompressedDataPtrArrayPtr);

	CloseHandle(mt_evtMaxMemory);
	CloseHandle(mt_evtAllCompressFinish);
	CloseHandle(mt_evtAllWriteFinish);

	if(!m_lpPckParams->cVarParams.bThreadRunning) {
		m_PckLog.PrintLogW(TEXT_USERCANCLE);
	}

}


VOID CPckClassThreadHelper::WriteThread(VOID* pParam)
{
	CPckClassThreadHelper *pThis = (CPckClassThreadHelper*)pParam;

	LPBYTE		dataToWrite;
	QWORD		dwTotalFileSizeAfterCompress = mt_CompressTotalFileSize;
	LPBYTE		lpBufferToWrite;
	LPPCKINDEXTABLE_COMPRESS	lpPckIndexTableComp;

	QWORD						dwAddressDataAreaEndAt = mt_dwAddressNameQueue;
	DWORD						nWrite = 0;

	while(1) {
		if(pThis->getCompressedDataQueue(dataToWrite, lpPckIndexTableComp)) {
			QWORD dwAddress = lpPckIndexTableComp->dwAddressFileDataToWrite;

			//判断一下dwAddress的值会不会超过dwTotalFileSizeAfterCompress
			//如果超过，说明文件空间申请的过小，重新申请一下ReCreateFileMapping
			//新文件大小在原来的基础上增加(lpfirstFile->dwFileSize + 1mb) >= 64mb ? (lpfirstFile->dwFileSize + 1mb) :64mb
			if(!pThis->IsNeedExpandWritingFile(mt_lpFileWrite, dwAddress, lpPckIndexTableComp->dwCompressedFilesize, dwTotalFileSizeAfterCompress)) {

				pThis->AfterWriteThreadFailProcess(
					mt_lpbThreadRunning,
					mt_evtAllCompressFinish,
					mt_dwFileCount,
					nWrite,
					mt_dwAddressQueue,
					dwAddressDataAreaEndAt,
					mt_pckCompressedDataPtrArrayGet);

				break;

			}

			//处理lpPckFileIndex->dwAddressOffset
			if(0 != lpPckIndexTableComp->dwCompressedFilesize) {

				if(NULL == (lpBufferToWrite = mt_lpFileWrite->View(dwAddress, lpPckIndexTableComp->dwCompressedFilesize))) {
					//写过程中目标文件无法View
					mt_lpFileWrite->UnMaping();

					pThis->AfterWriteThreadFailProcess(
						mt_lpbThreadRunning,
						mt_evtAllCompressFinish,
						mt_dwFileCount,
						nWrite,
						mt_dwAddressQueue,
						dwAddressDataAreaEndAt,
						mt_pckCompressedDataPtrArrayGet);

					break;
				}

				memcpy(lpBufferToWrite, dataToWrite, lpPckIndexTableComp->dwCompressedFilesize);

				dwAddressDataAreaEndAt += lpPckIndexTableComp->dwAddressAddStep;

				free(dataToWrite);
				mt_lpFileWrite->UnmapView();
			}

			//已读入文件数+1
			nWrite++;

			pThis->freeMaxAndSubtractMemory(lpPckIndexTableComp->dwMallocSize);

			if(mt_dwFileCountOfWriteTarget == nWrite) {
				WaitForSingleObject(mt_evtAllCompressFinish, INFINITE);
				mt_dwAddressQueue = dwAddressDataAreaEndAt;

				break;
			}

		} else {

			if(!(*mt_lpbThreadRunning)) {
				//取消
				WaitForSingleObject(mt_evtAllCompressFinish, INFINITE);

				mt_dwFileCountOfWriteTarget = nWrite;
				mt_dwAddressQueue = dwAddressDataAreaEndAt;

				break;
			}

			::Sleep(10);
		}
	}

	SetEvent(mt_evtAllWriteFinish);
	return;
}

#define COMPRESSTHREADFUNC CompressThreadCreate
#define COMPRESSSINGLETHREADFUNC CreatePckFileSingleThread
#define TARGET_PCK_MODE_COMPRESS PCK_MODE_COMPRESS_CREATE
#include "PckClassThreadCompressFunctions.h"

#define COMPRESSTHREADFUNC CompressThreadAdd
#define COMPRESSSINGLETHREADFUNC UpdatePckFileSingleThread
#define TARGET_PCK_MODE_COMPRESS PCK_MODE_COMPRESS_ADD
#include "PckClassThreadCompressFunctions.h"


VOID CPckClassThreadHelper::CompressThreadRecompress(VOID* pParam)
{
	EnterCriticalSection(&g_mt_threadID);
	++mt_threadID;
	LeaveCriticalSection(&g_mt_threadID);

	CPckClassThreadHelper *pThis = (CPckClassThreadHelper*)pParam;

	int		level = pThis->m_lpPckParams->dwCompressLevel;

	//当文件大小为0小，使用1填充指针
	BYTE	*bufCompressData = (BYTE*)1;

	LPBYTE				lpBufferToRead;
	//保存重压缩数据的解压的数据
	LPBYTE				lpDecompressBuffer = NULL;
	//保存重压缩数据的源数据
	LPBYTE				lpSourceBuffer = NULL;
	//
	DWORD				dwMaxMallocSource = 0, dwMaxMallocDecompress = 0;

	//开始
	LPPCKINDEXTABLE					lpPckIndexTablePtrSrc = pThis->m_PckAllInfo.lpPckIndexTable;		//源文件的文件索引信息
	PCKINDEXTABLE_COMPRESS			cPckIndexTableDst;

	DWORD	dwFileCount = pThis->m_PckAllInfo.dwFileCount;

	for(DWORD i = 0; i < dwFileCount; ++i) {

		if(lpPckIndexTablePtrSrc->isInvalid) {
			++lpPckIndexTablePtrSrc;
			continue;
		}

		//判断此文件是否已经压缩处理过了
		{
			EnterCriticalSection(&g_dwCompressedflag);

			if(0 != lpPckIndexTablePtrSrc->isRecompressed) {
				LeaveCriticalSection(&g_dwCompressedflag);
				++lpPckIndexTablePtrSrc;
				continue;
			}

			lpPckIndexTablePtrSrc->isRecompressed = 1;
			LeaveCriticalSection(&g_dwCompressedflag);
		}

		/*

		//获取数据压缩后的大小，如果源大小小于一定值就不压缩
		DWORD CPckClassBodyWriter::GetCompressBoundSizeByFileSize(LPPCKFILEINDEX	lpPckFileIndex, DWORD dwFileSize)
		{
		if(PCK_BEGINCOMPRESS_SIZE < dwFileSize) {
		lpPckFileIndex->dwFileClearTextSize = dwFileSize;
		lpPckFileIndex->dwFileCipherTextSize = compressBound(dwFileSize);
		} else {
		lpPckFileIndex->dwFileCipherTextSize = lpPckFileIndex->dwFileClearTextSize = dwFileSize;
		}

		return lpPckFileIndex->dwFileCipherTextSize;
		}
		*/
		DWORD dwNumberOfBytesToMap = lpPckIndexTablePtrSrc->cFileIndex.dwFileCipherTextSize;
		DWORD dwFileClearTextSize = lpPckIndexTablePtrSrc->cFileIndex.dwFileClearTextSize;

		PCKFILEINDEX cPckFileIndexDst;
		memcpy(&cPckFileIndexDst, &lpPckIndexTablePtrSrc->cFileIndex, sizeof(PCKFILEINDEX));

		if(PCK_BEGINCOMPRESS_SIZE < dwFileClearTextSize) {
			cPckFileIndexDst.dwFileCipherTextSize = pThis->compressBound(dwFileClearTextSize);
		}

		DWORD dwMallocSize = cPckFileIndexDst.dwFileCipherTextSize;

		if(!*(mt_lpbThreadRunning))
			break;

		if(0 != dwFileClearTextSize) {

			//判断使用的内存是否超过最大值
			pThis->detectMaxAndAddMemory(dwMallocSize);

			bufCompressData = (BYTE*)malloc(dwMallocSize);

			//文件数据需要重压缩
			if(PCK_BEGINCOMPRESS_SIZE < dwFileClearTextSize) {
				//保存源数据的空间
				if(dwMaxMallocSource < dwNumberOfBytesToMap) {

					dwMaxMallocSource = dwNumberOfBytesToMap;

					if(NULL != lpSourceBuffer)
						free(lpSourceBuffer);

					if(NULL == (lpSourceBuffer = (LPBYTE)malloc(dwNumberOfBytesToMap))) {
						*(mt_lpbThreadRunning) = FALSE;
						if(NULL != lpDecompressBuffer)
							free(lpDecompressBuffer);
						break;
					}
				}

				//保存解压数据的空间
				if(dwMaxMallocDecompress < dwFileClearTextSize) {

					dwMaxMallocDecompress = dwFileClearTextSize;

					if(NULL != lpDecompressBuffer)
						free(lpDecompressBuffer);

					if(NULL == (lpDecompressBuffer = (LPBYTE)malloc(dwFileClearTextSize))) {
						free(lpSourceBuffer);
						*(mt_lpbThreadRunning) = FALSE;
						break;
					}
				}

				EnterCriticalSection(&g_mt_threadID);
				if(NULL == (lpBufferToRead = mt_lpFileRead->View(lpPckIndexTablePtrSrc->cFileIndex.dwAddressOffset, dwNumberOfBytesToMap))) {
					free(lpSourceBuffer);
					free(lpDecompressBuffer);
					*(mt_lpbThreadRunning) = FALSE;
					break;
				}
				memcpy(lpSourceBuffer, lpBufferToRead, dwNumberOfBytesToMap);
				mt_lpFileRead->UnmapView();
				LeaveCriticalSection(&g_mt_threadID);

				pThis->decompress(lpDecompressBuffer, &dwFileClearTextSize, lpSourceBuffer, dwNumberOfBytesToMap);
				if(dwFileClearTextSize == lpPckIndexTablePtrSrc->cFileIndex.dwFileClearTextSize) {

					pThis->compress(bufCompressData, &cPckFileIndexDst.dwFileCipherTextSize, lpDecompressBuffer, dwFileClearTextSize);
				} else {
					memcpy(bufCompressData, lpSourceBuffer, dwNumberOfBytesToMap);
				}
			} else {
#pragma region 文件过小不需要压缩时
				EnterCriticalSection(&g_mt_threadID);
				if(NULL == (lpBufferToRead = mt_lpFileRead->View(lpPckIndexTablePtrSrc->cFileIndex.dwAddressOffset, dwNumberOfBytesToMap))) {
					*(mt_lpbThreadRunning) = FALSE;
					break;
				}
				memcpy(bufCompressData, lpBufferToRead, dwNumberOfBytesToMap);
				mt_lpFileRead->UnmapView();
				LeaveCriticalSection(&g_mt_threadID);
#pragma endregion
			}

		} else {
			bufCompressData = (BYTE*)1;
		}

		EnterCriticalSection(&g_cs);
		(*mt_lpdwCount)++;										//窗口中以显示的文件进度
		LeaveCriticalSection(&g_cs);

		memset(&cPckIndexTableDst, 0, sizeof(PCKINDEXTABLE_COMPRESS));

		cPckIndexTableDst.dwMallocSize = dwMallocSize;

		//放入队列
		pThis->putCompressedDataQueue(bufCompressData, &cPckIndexTableDst, &cPckFileIndexDst);

		++lpPckIndexTablePtrSrc;
	}

	free(lpSourceBuffer);
	free(lpDecompressBuffer);

	EnterCriticalSection(&g_mt_threadID);

	mt_threadID--;
	if(0 == mt_threadID) {
		pThis->m_PckLog.PrintLogN(TEXT_LOG_FLUSH_CACHE);
		SetEvent(mt_evtAllCompressFinish);
	}

	LeaveCriticalSection(&g_mt_threadID);

	return;
}