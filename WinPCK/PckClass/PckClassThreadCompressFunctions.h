VOID CPckClass::COMPRESSTHREADFUNC(VOID* pParam)
{
	CPckClass *pThis = (CPckClass*)pParam;

	char	szFileMappingNameSpaceFormat[16];
	char	szFileMappingNameSpace[32];
	int		level = pThis->lpPckParams->dwCompressLevel;

	memcpy(szFileMappingNameSpaceFormat, pThis->m_szMapNameRead, 16);
	strcat_s(szFileMappingNameSpaceFormat, 16, "%d");

	EnterCriticalSection(&g_mt_threadID);
	sprintf_s(szFileMappingNameSpace, 32, szFileMappingNameSpaceFormat, mt_threadID++);
	LeaveCriticalSection(&g_mt_threadID);

	BYTE	*bufCompressData = (BYTE*)1;

	LPBYTE				lpBufferToRead;
	PCKFILEINDEX		pckFileIndex;

	//开始
	LPFILES_TO_COMPRESS			lpfirstFile = pThis->m_firstFile;
	PCKINDEXTABLE_COMPRESS		cPckIndexTableComp;

	memset(pckFileIndex.szFilename, 0, MAX_PATH_PCK_260);

	//patch 140424
	CMapViewFileRead	*lpFileRead = new CMapViewFileRead();

	while (NULL != lpfirstFile->next)
	{

#pragma region CriticalSectionCompressedflag
#ifdef _DEBUG
		assert(0 == lpfirstFile->dwCompressedflag || 1 == lpfirstFile->dwCompressedflag);
#endif

		EnterCriticalSection(&g_dwCompressedflag);

		if (0 != lpfirstFile->dwCompressedflag) {
			LeaveCriticalSection(&g_dwCompressedflag);
			lpfirstFile = lpfirstFile->next;
			continue;
		}

		lpfirstFile->dwCompressedflag = 1;

		LeaveCriticalSection(&g_dwCompressedflag);
#pragma endregion

		DWORD dwMallocSize = pThis->GetCompressBoundSizeByFileSize(&pckFileIndex, lpfirstFile->dwFileSize);

		//判断使用的内存是否超过最大值
		pThis->detectMaxAndAddMemory(dwMallocSize);

#if TARGET_PCK_MODE_COMPRESS == PCK_MODE_COMPRESS_CREATE
			memcpy(pckFileIndex.szFilename, lpfirstFile->lpszFileTitle, lpfirstFile->nBytesToCopy);
#elif TARGET_PCK_MODE_COMPRESS == PCK_MODE_COMPRESS_ADD
			memcpy(mystrcpy(pckFileIndex.szFilename, mt_szCurrentNodeString), lpfirstFile->lpszFileTitle, lpfirstFile->nBytesToCopy - mt_nCurrentNodeStringLen);
#endif
		if (!*(mt_lpbThreadRunning))
			break;

		//如果文件大小为0，则跳过打开文件步骤
		if (0 != pckFileIndex.dwFileClearTextSize)
		{
			//文件不为0时的处理
			//打开要进行压缩的文件
			if (NULL == (lpBufferToRead = pThis->OpenMappingAndViewAllRead(lpFileRead, lpfirstFile->szFilename, szFileMappingNameSpace)))
			{
				*(mt_lpbThreadRunning) = FALSE;
				lpFileRead->clear();
				break;
			}

			if (NULL == (bufCompressData = (BYTE*)malloc(dwMallocSize))) {
				*(mt_lpbThreadRunning) = FALSE;
				lpFileRead->clear();
				break;
			}

			if (PCK_BEGINCOMPRESS_SIZE < pckFileIndex.dwFileClearTextSize)
			{
				pThis->compress(bufCompressData, &pckFileIndex.dwFileCipherTextSize,
					lpBufferToRead, pckFileIndex.dwFileClearTextSize, level);
			}
			else {
				memcpy(bufCompressData, lpBufferToRead, pckFileIndex.dwFileClearTextSize);
			}

			lpFileRead->clear();

		}
		else {
			bufCompressData = (BYTE*)1;
		}

		memset(&cPckIndexTableComp, 0, sizeof(PCKINDEXTABLE_COMPRESS));

		EnterCriticalSection(&g_cs);

		//窗口中以显示的文件进度
		(*mt_lpdwCount)++;

#if TARGET_PCK_MODE_COMPRESS == PCK_MODE_COMPRESS_ADD
			//有重复的文件
			if (NULL != lpfirstFile->samePtr)
			{
				//重复的文件，待写入的文件大小小于被覆盖的文件大小，使用老数据区
				if (pckFileIndex.dwFileCipherTextSize <= lpfirstFile->samePtr->cFileIndex.dwFileCipherTextSize)
				{
					cPckIndexTableComp.dwAddressOfDuplicateOldDataArea = lpfirstFile->samePtr->cFileIndex.dwAddressOffset;
				}

				lpfirstFile->samePtr->cFileIndex.dwFileCipherTextSize = pckFileIndex.dwFileCipherTextSize;
				lpfirstFile->samePtr->cFileIndex.dwFileClearTextSize = pckFileIndex.dwFileClearTextSize;

				cPckIndexTableComp.bInvalid = TRUE;

			}
#endif

		LeaveCriticalSection(&g_cs);

		cPckIndexTableComp.dwMallocSize = dwMallocSize;

#ifdef _DEBUG
		if (1 == (int)bufCompressData) {
			logOutput(__FUNCTION__, formatString("file：bufCompressData(0x%08x):0, \t%s\r\n", (int)bufCompressData, lpfirstFile->szFilename));
		}
		else {
			logOutput(__FUNCTION__, formatString("file：bufCompressData(0x%08x):0x%016llx, \t%s\r\n", (int)bufCompressData, _byteswap_uint64(*(unsigned __int64*)bufCompressData), lpfirstFile->szFilename));
		}
#endif

#if TARGET_PCK_MODE_COMPRESS == PCK_MODE_COMPRESS_CREATE

		pThis->putCompressedDataQueue(bufCompressData, &cPckIndexTableComp, &pckFileIndex);

#elif TARGET_PCK_MODE_COMPRESS == PCK_MODE_COMPRESS_ADD
		if (!cPckIndexTableComp.bInvalid)
		{
			//放入队列
			pThis->putCompressedDataQueue(bufCompressData, &cPckIndexTableComp, &pckFileIndex);
		}
		else {

			//放入队列
			pThis->putCompressedDataQueue(bufCompressData, &cPckIndexTableComp, &lpfirstFile->samePtr->cFileIndex);
		}
#endif


		//下一个文件列表
		lpfirstFile = lpfirstFile->next;
	}

	delete lpFileRead;

	EnterCriticalSection(&g_mt_threadID);

	mt_threadID--;
	if (0 == mt_threadID) {
		pThis->PrintLogN(TEXT_LOG_FLUSH_CACHE);
		SetEvent(mt_evtAllCompressFinish);
	}

	LeaveCriticalSection(&g_mt_threadID);


	_endthread();
}

#if PCK_COMPRESS_NEED_ST

BOOL CPckClass::COMPRESSSINGLETHREADFUNC(QWORD &dwAddress)
{
#if TARGET_PCK_MODE_COMPRESS == PCK_MODE_COMPRESS_ADD
	BOOL		bUseCurrent;		//重名时是否使用当前数据，添加模式下有效
	QWORD		dwAddressTemp;			//添加模式下，重名时用来指示被重名文件远大于添加文件时，压缩数据的存放地址
	//DWORD		dwOverWriteModeMaxLength;	->dwMallocSize //重名时，用来覆盖被重名文件时使用的最大ViewMap大小，不然多出的的数据会把后面文件的数据清0
#endif
	int		level = lpPckParams->dwCompressLevel;

	LPBYTE		lpBufferToWrite, lpBufferToRead;

	PCKFILEINDEX	pckFileIndex;

	//初始化指针
	LPPCKINDEXTABLE_COMPRESS	lpPckIndexTablePtr = mt_lpPckIndexTable;
	LPFILES_TO_COMPRESS			lpfirstFile = m_firstFile;

	//初始化变量值 
	memset(pckFileIndex.szFilename, 0, MAX_PATH_PCK_260);

	//patch 140424
	CMapViewFileRead	*lpFileRead = new CMapViewFileRead();

	while (NULL != lpfirstFile->next) {
#if TARGET_PCK_MODE_COMPRESS == PCK_MODE_COMPRESS_CREATE
		memcpy(pckFileIndex.szFilename, lpfirstFile->lpszFileTitle, lpfirstFile->nBytesToCopy);
#elif TARGET_PCK_MODE_COMPRESS == PCK_MODE_COMPRESS_ADD
		memcpy(mystrcpy(pckFileIndex.szFilename, mt_szCurrentNodeString), lpfirstFile->lpszFileTitle, lpfirstFile->nBytesToCopy - mt_nCurrentNodeStringLen);
#endif
		DWORD dwMallocSize = GetCompressBoundSizeByFileSize(&pckFileIndex, lpfirstFile->dwFileSize);

		if (!lpPckParams->cVarParams.bThreadRunning) {
			PrintLogW(TEXT_USERCANCLE);

			//目前已压缩了多少文件，将数据写入dwFileCount，写文件名列表和文件头、尾，完成文件操作
			SET_PCK_FILE_COUNT_GLOBAL
				break;
		}

		//如果文件大小为0，则跳过打开文件步骤
		if (0 != dwMallocSize) {

			//判断一下dwAddress的值会不会超过dwTotalFileSizeAfterCompress
			//如果超过，说明文件空间申请的过小，重新申请一下ReCreateFileMapping
			//新文件大小在原来的基础上增加(lpfirstFile->dwFileSize + 1mb) >= 64mb ? (lpfirstFile->dwFileSize + 1mb) :64mb
			if (!IsNeedExpandWritingFile(mt_lpFileWrite, dwAddress, dwMallocSize, mt_CompressTotalFileSize)) {
				SET_PCK_FILE_COUNT_AT_FAIL
					break;
			}
#if TARGET_PCK_MODE_COMPRESS == PCK_MODE_COMPRESS_ADD
			//添加模式
			if (NULL != lpfirstFile->samePtr) {
				//如果现有的文件大小大于pck中的文件的已压缩大小，使用新buffer
				if (dwMallocSize >= lpfirstFile->samePtr->cFileIndex.dwFileCipherTextSize) {
					lpfirstFile->samePtr->cFileIndex.dwAddressOffset = dwAddress;
					bUseCurrent = TRUE;

					dwAddressTemp = dwAddress;

				}
				else {

					dwAddressTemp = lpfirstFile->samePtr->cFileIndex.dwAddressOffset;
					bUseCurrent = FALSE;
					dwMallocSize = lpfirstFile->samePtr->cFileIndex.dwFileCipherTextSize;


					lpPckIndexTablePtr->dwAddressOfDuplicateOldDataArea = lpfirstFile->samePtr->cFileIndex.dwAddressOffset;

				}

				lpPckIndexTablePtr->bInvalid = TRUE;

			}
			else {
				dwAddressTemp = dwAddress;
			}

			if (NULL == (lpBufferToWrite = mt_lpFileWrite->View(dwAddressTemp, dwMallocSize))) {
#elif TARGET_PCK_MODE_COMPRESS == PCK_MODE_COMPRESS_CREATE
			if (NULL == (lpBufferToWrite = mt_lpFileWrite->View(dwAddress, dwMallocSize))) {
#endif
				mt_lpFileWrite->UnMaping();
				SET_PCK_FILE_COUNT_AT_FAIL
					break;
			}

#pragma region 打开要进行压缩的文件
			if (NULL == (lpBufferToRead = OpenMappingAndViewAllRead(lpFileRead, lpfirstFile->szFilename, m_szMapNameRead)))
			{
				mt_lpFileWrite->UnMaping();
				lpFileRead->clear();

				SET_PCK_FILE_COUNT_AT_FAIL
					break;
			}
#pragma endregion

			if (PCK_BEGINCOMPRESS_SIZE < pckFileIndex.dwFileClearTextSize) {

				compress(lpBufferToWrite, &pckFileIndex.dwFileCipherTextSize,
					lpBufferToRead, pckFileIndex.dwFileClearTextSize, level);
			}
			else {
				memcpy(lpBufferToWrite, lpBufferToRead, pckFileIndex.dwFileClearTextSize);
			}

			mt_lpFileWrite->UnmapView();
			lpFileRead->clear();

			}//文件不为0时的处理
#if TARGET_PCK_MODE_COMPRESS == PCK_MODE_COMPRESS_ADD
		else {
			//if (NULL != lpfirstFile->samePtr) {
				bUseCurrent = FALSE;
			//}
		}

		//写入此文件的PckFileIndex文件压缩信息并压缩
		if (NULL == lpfirstFile->samePtr) {
#endif
			pckFileIndex.dwAddressOffset = dwAddress;		//此文件的压缩数据起始地址
			dwAddress += pckFileIndex.dwFileCipherTextSize;	//下一个文件的压缩数据起始地址

			FillAndCompressIndexData(lpPckIndexTablePtr, &pckFileIndex);

			//窗口中以显示的文件进度
			lpPckParams->cVarParams.dwUIProgress++;
#if TARGET_PCK_MODE_COMPRESS == PCK_MODE_COMPRESS_ADD
		}
		else {

			lpfirstFile->samePtr->cFileIndex.dwFileCipherTextSize = pckFileIndex.dwFileCipherTextSize;
			lpfirstFile->samePtr->cFileIndex.dwFileClearTextSize = pckFileIndex.dwFileClearTextSize;

			if (bUseCurrent) {

				dwAddress += pckFileIndex.dwFileCipherTextSize;

			}
		}
#endif
		//加密的文件名数据数组
		lpPckIndexTablePtr++;
		//下一个文件列表
		lpfirstFile = lpfirstFile->next;
		}

	delete lpFileRead;
	return TRUE;
	}
#endif

#undef COMPRESSTHREADFUNC
#undef COMPRESSSINGLETHREADFUNC
#undef TARGET_PCK_MODE_COMPRESS