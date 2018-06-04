
VOID CPckClass::CompressThreadRecompress(VOID* pParam)
{
	EnterCriticalSection(&g_mt_threadID);
	++mt_threadID;
	LeaveCriticalSection(&g_mt_threadID);

	CPckClass *pThis = (CPckClass*)pParam;

	int		level = pThis->lpPckParams->dwCompressLevel;

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

					pThis->compress(bufCompressData, &cPckFileIndexDst.dwFileCipherTextSize, lpDecompressBuffer, dwFileClearTextSize, level);
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

//重压缩文件
BOOL CPckClass::RecompressPckFile(LPCTSTR szRecompressPckFile)
{

	m_PckLog.PrintLogI(TEXT_LOG_RECOMPRESS);

	LPPCKINDEXTABLE_COMPRESS	lpPckIndexTable, lpPckIndexTablePtr;
	QWORD	dwAddress = PCK_DATA_START_AT, dwAddressName;
	DWORD	dwFileCount = m_PckAllInfo.dwFileCount;
	DWORD	dwNoDupFileCount = ReCountFiles();
	QWORD	dwTotalFileSizeAfterRebuild = NPckClassFileDisk::GetPckFilesizeRebuild(szRecompressPckFile, m_PckAllInfo.qwPckSize);

	CMapViewFileWrite	*lpFileWrite;
	CMapViewFileRead	cFileRead;
	int					threadnum = lpPckParams->dwMTThread;

	//构造头和尾时需要的参数
	PCK_ALL_INFOS		pckAllInfo;

	//设置界面进度条总值
	lpPckParams->cVarParams.dwUIProgressUpper = dwNoDupFileCount;

	//打开源文件 
	if(!cFileRead.OpenPckAndMappingRead(m_PckAllInfo.szFilename)) {
		assert(FALSE);
		return FALSE;
	}

	if(!BeforeSingleOrMultiThreadProcess(&pckAllInfo, lpFileWrite, szRecompressPckFile, CREATE_ALWAYS, dwTotalFileSizeAfterRebuild, threadnum)) {
		assert(FALSE);
		return FALSE;
	}

	if(!initCompressedDataQueue(threadnum, dwFileCount, PCK_DATA_START_AT)) {
		delete lpFileWrite;
		assert(FALSE);
		return FALSE;
	}

	mt_lpFileRead = &cFileRead;
	mt_lpFileWrite = lpFileWrite;
	mt_dwFileCount = dwFileCount;
	mt_dwFileCountOfWriteTarget = dwNoDupFileCount;

	MultiThreadInitialize(CompressThreadRecompress, WriteThread, threadnum);

	//写文件索引
	lpPckIndexTablePtr = lpPckIndexTable = mt_lpPckIndexTable;
	dwAddressName = dwAddress = mt_dwAddressQueue;

	//清除掉多线程使用的已压缩标记位
	LPPCKINDEXTABLE lpPckIndexTablePtrSrc = m_PckAllInfo.lpPckIndexTable;

	for(DWORD i = 0; i < m_PckAllInfo.dwFileCount; ++i) {

		lpPckIndexTablePtrSrc->isRecompressed = FALSE;
		lpPckIndexTablePtrSrc++;
	}

	//取消后，文件数量
	dwNoDupFileCount = mt_dwFileCountOfWriteTarget;

	WritePckIndexTable(lpFileWrite, mt_lpPckIndexTable, dwNoDupFileCount, dwAddress);

	pckAllInfo.dwAddressName = dwAddressName;
	pckAllInfo.dwFileCount = dwNoDupFileCount;
	AfterProcess(lpFileWrite, pckAllInfo, dwAddress);

	//结束，设置进度为100%
	//lpPckParams->cVarParams.dwUIProgress = lpPckParams->cVarParams.dwUIProgressUpper;

	delete lpFileWrite;

	uninitCompressedDataQueue(threadnum);

	m_PckLog.PrintLogN(TEXT_LOG_WORKING_DONE);

	return TRUE;
}
