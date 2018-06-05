VOID CPckClass::COMPRESSTHREADFUNC(VOID* pParam)
{
	CPckClass *pThis = (CPckClass*)pParam;

	//char	szFileMappingNameSpaceFormat[16];
	//char	szFileMappingNameSpace[32];
	int		level = pThis->m_lpPckParams->dwCompressLevel;

	//memcpy(szFileMappingNameSpaceFormat, pThis->m_szMapNameRead, 16);
	//strcat_s(szFileMappingNameSpaceFormat, 16, "%d");

	EnterCriticalSection(&g_mt_threadID);
	//sprintf_s(szFileMappingNameSpace, 32, szFileMappingNameSpaceFormat, mt_threadID++);
	mt_threadID++;
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
			if (NULL == (lpBufferToRead = lpFileRead->OpenMappingAndViewAllRead(lpfirstFile->szFilename)))
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
		pThis->m_PckLog.PrintLogN(TEXT_LOG_FLUSH_CACHE);
		SetEvent(mt_evtAllCompressFinish);
	}

	LeaveCriticalSection(&g_mt_threadID);

	return;
}

#undef COMPRESSTHREADFUNC
#undef COMPRESSSINGLETHREADFUNC
#undef TARGET_PCK_MODE_COMPRESS