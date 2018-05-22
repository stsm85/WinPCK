
#define COMPRESSTHREADFUNC CompressThreadAdd
#define COMPRESSSINGLETHREADFUNC UpdatePckFileSingleThread
#define TARGET_PCK_MODE_COMPRESS PCK_MODE_COMPRESS_ADD
#include "PckClassThreadCompressFunctions.h"


//更新pck包
BOOL CPckClass::UpdatePckFile(LPTSTR szPckFile, TCHAR(*lpszFilePath)[MAX_PATH], int nFileCount, LPPCK_PATH_NODE lpNodeToInsert)
{
	DWORD		dwFileCount = 0, dwOldPckFileCount;					//文件数量, 原pck文件中的文件数
	QWORD		qwTotalFileSize = 0, qwTotalFileSizeTemp;			//未压缩时所有文件大小
	size_t		nLen;

	char		szPathMbsc[MAX_PATH];
	char		szLogString[LOG_BUFFER];

	int			level = lpPckParams->dwCompressLevel;
	int			threadnum = lpPckParams->dwMTThread;

	QWORD		dwAddress;

	//开始查找文件
	LPFILES_TO_COMPRESS			lpfirstFile;
	TCHAR(*lpszFilePathPtr)[MAX_PATH] = (TCHAR(*)[MAX_PATH])lpszFilePath;
	DWORD				dwAppendCount = nFileCount;
	LPPCK_PATH_NODE		lpNodeToInsertPtr;


	//设置参数
	if(m_ReadCompleted) {
		lstrcpy(szPckFile, m_PckAllInfo.szFilename);

		dwOldPckFileCount = m_PckAllInfo.dwFileCount;

		lpNodeToInsertPtr = lpNodeToInsert;

		//取得当前节点的相对路径
		if(!GetCurrentNodeString(mt_szCurrentNodeString, lpNodeToInsert)) {
			free(lpszFilePath);
			assert(FALSE);
			return FALSE;
		}

		mt_nCurrentNodeStringLen = strlen(mt_szCurrentNodeString);

		sprintf_s(szLogString, TEXT_LOG_UPDATE_ADD
			"-"
			TEXT_LOG_LEVEL_THREAD, level, threadnum);
		PrintLogI(szLogString);

	} else {

		m_PckAllInfo.dwAddressName = PCK_DATA_START_AT;

		dwOldPckFileCount = 0;

		lpNodeToInsertPtr = m_RootNode.child;

		*mt_szCurrentNodeString = 0;

		mt_nCurrentNodeStringLen = 0;

		sprintf_s(szLogString, TEXT_LOG_UPDATE_NEW
			"-"
			TEXT_LOG_LEVEL_THREAD, level, threadnum);
		PrintLogI(szLogString);

	}

	if(NULL == m_firstFile)m_firstFile = (LPFILES_TO_COMPRESS)AllocMemory(sizeof(FILES_TO_COMPRESS));
	if(NULL == m_firstFile) {

		PrintLogEL(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
		free(lpszFilePath);
		assert(FALSE);
		return FALSE;
	}

	lpfirstFile = m_firstFile;

	//读文件
	CMapViewFileRead cFileRead;
	for(DWORD i = 0; i < dwAppendCount; i++) {

#ifdef UNICODE
		CUcs2Ansi cU2A;
		cU2A.GetString(*lpszFilePathPtr, szPathMbsc, MAX_PATH);
#else
		strcpy_s(szPathMbsc, *lpszFilePathPtr);
#endif
		nLen = (size_t)(strrchr(szPathMbsc, '\\') - szPathMbsc) + 1;

		if(FILE_ATTRIBUTE_DIRECTORY == (FILE_ATTRIBUTE_DIRECTORY & GetFileAttributesA(szPathMbsc))) {
			//文件夹
			EnumFile(szPathMbsc, FALSE, dwFileCount, lpfirstFile, qwTotalFileSize, nLen);
		} else {

			if(!cFileRead.Open(szPathMbsc)) {
				DeallocateFileinfo();
				free(lpszFilePath);
				PrintLogEL(TEXT_OPENNAME_FAIL, *lpszFilePathPtr, __FILE__, __FUNCTION__, __LINE__);

				assert(FALSE);
				return FALSE;
			}

			strcpy(lpfirstFile->szFilename, szPathMbsc);

			lpfirstFile->lpszFileTitle = lpfirstFile->szFilename + nLen;
			lpfirstFile->nBytesToCopy = MAX_PATH - nLen;

			qwTotalFileSize += (lpfirstFile->dwFileSize = cFileRead.GetFileSize());

			lpfirstFile->next = (LPFILES_TO_COMPRESS)AllocMemory(sizeof(FILES_TO_COMPRESS));
			lpfirstFile = lpfirstFile->next;

			cFileRead.clear();

			dwFileCount++;
		}

		lpszFilePathPtr++;
	}

	free(lpszFilePath);

	assert(0 != dwFileCount);

	if(0 == dwFileCount)return TRUE;

	//参数说明：
	// mt_dwFileCount	添加的文件总数，计重复
	// dwAllCount		界面显示的总数->lpPckParams->cVarParams.dwUIProgressUpper
	// dwFileCount		计算过程使用参数，在下面的计算过程中将使用此参数表示添加的文件总数，不计重复
	// 

	//文件数写入窗口类中保存以显示进度
	DWORD dwPrepareToAdd = mt_dwFileCount = lpPckParams->cVarParams.dwUIProgressUpper = dwFileCount;

	//计算大概需要多大空间qwTotalFileSize
	qwTotalFileSizeTemp = qwTotalFileSize * 0.6 + m_PckAllInfo.dwAddressName + PCK_SPACE_DETECT_SIZE;

#if !PCK_SIZE_UNLIMITED
	if(((0 != (qwTotalFileSizeTemp >> 32)) && (0x20002 == m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->Version)) || \
		((0 != (qwTotalFileSizeTemp >> 33)) && (0x20003 == m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->Version))) {

		PrintLogE(TEXT_COMPFILE_TOOBIG, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}
#endif
	mt_CompressTotalFileSize = qwTotalFileSizeTemp;

	//if (PCK_SPACE_DETECT_SIZE >= mt_CompressTotalFileSize)mt_CompressTotalFileSize = PCK_STEP_ADD_SIZE;

	//与原来目录中的文件对比，是否有重名
	//策略：无条件覆盖吧				如果重名且都为文件或文件夹，则覆盖
	//
	//调用FindFileNode返回-1退出，返回0，表示直接添加，非0就是有重复的
	//写专用的writethread和compressthread,以调用
	//在PCKINDEXTABLE_COMPRESS里添加add专用属性，以判断是否启用此节点（重名时）0使用，1不使用
	//结束 时使用2个循环写入压缩索引 

	//dwFileCount变量在此处指的是添加的文件除去重名的数量 
	if(m_ReadCompleted) {
		lpfirstFile = m_firstFile;
		while(NULL != lpfirstFile->next) {
			LPPCK_PATH_NODE lpDuplicateNode;
			lpDuplicateNode = FindFileNode(lpNodeToInsertPtr, lpfirstFile->lpszFileTitle);

			if(-1 == (int)lpDuplicateNode) {
				DeallocateFileinfo();
				PrintLogE(TEXT_ERROR_DUP_FOLDER_FILE);
				assert(FALSE);
				return FALSE;
			}

			if(NULL != lpDuplicateNode) {
				lpfirstFile->samePtr = lpDuplicateNode->lpPckIndexTable;
				dwFileCount--;
			}

			lpfirstFile = lpfirstFile->next;

		}
	}

	//日志
	sprintf_s(szLogString, TEXT_UPDATE_FILE_INFO, dwPrepareToAdd, mt_CompressTotalFileSize);
	PrintLogI(szLogString);

	PCK_ALL_INFOS	pckAllInfo;
	//OPEN_ALWAYS，新建新的pck(CREATE_ALWAYS)或更新存在的pck(OPEN_EXISTING)
	if(!BeforeSingleOrMultiThreadProcess(&pckAllInfo, mt_lpFileWrite, szPckFile, OPEN_ALWAYS, mt_CompressTotalFileSize, threadnum)) {
		DeallocateFileinfo();
		//PrintLogE(TEXT_ERROR_DUP_FOLDER_FILE);
		assert(FALSE);
		return FALSE;
	}
	if(!initCompressedDataQueue(threadnum, mt_dwFileCountOfWriteTarget = mt_dwFileCount, dwAddress = m_PckAllInfo.dwAddressName)) {
		delete mt_lpFileWrite;
		DeallocateFileinfo();
		//PrintLogE(TEXT_ERROR_DUP_FOLDER_FILE);
		assert(FALSE);
		return FALSE;
	}

	if(PCK_COMPRESS_NEED_ST < threadnum) {

		MultiThreadInitialize(CompressThreadAdd, WriteThread, threadnum);
		dwAddress = mt_dwAddressQueue;
	}
#if PCK_COMPRESS_NEED_ST
	else {
		UpdatePckFileSingleThread(dwAddress);
	}
#endif
	//打印报告用参数
	DWORD	dwUseNewDataAreaInDuplicateFile = 0;

	//写文件索引
	QWORD	dwAddressName = dwAddress;

	//窗口中以显示的文件进度，初始化，显示写索引进度mt_hFileToWrite
	lpPckParams->cVarParams.dwUIProgress = 0;
	//dwAllCount = mt_dwFileCount + dwOldPckFileCount;	//这里的文件数包含了重名的数量，应该是下面的公式
	lpPckParams->cVarParams.dwUIProgressUpper = dwFileCount + dwOldPckFileCount;


	//写原来的文件
	LPPCKINDEXTABLE	lpPckIndexTableSrc = m_lpPckIndexTable;

	for(DWORD i = 0; i < dwOldPckFileCount; i++) {

		PCKINDEXTABLE_COMPRESS	pckIndexTableTemp;
		WritePckIndex(mt_lpFileWrite, FillAndCompressIndexData(&pckIndexTableTemp, &lpPckIndexTableSrc->cFileIndex), dwAddress);
		lpPckIndexTableSrc++;

	}

	//写添加的文件
	LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr = mt_lpPckIndexTable;

	mt_dwFileCount = dwFileCount = mt_dwFileCountOfWriteTarget;
	for(DWORD i = 0; i < mt_dwFileCountOfWriteTarget; i++) {
		////处理lpPckFileIndex->dwAddressOffset

		if(!lpPckIndexTablePtr->bInvalid) {

			WritePckIndex(mt_lpFileWrite, lpPckIndexTablePtr, dwAddress);

		} else {

			assert(0 != dwFileCount);
			dwFileCount--;
		}

		if(0 == lpPckIndexTablePtr->dwAddressOfDuplicateOldDataArea)
			dwUseNewDataAreaInDuplicateFile++;

		lpPckIndexTablePtr++;

	}

	//pckTail.dwFileCount = dwFileCount + dwOldPckFileCount;//mt_dwFileCount是实际写入数，重复的已经在上面减去了
	pckAllInfo.dwAddressName = dwAddressName;
	pckAllInfo.dwFileCount = dwFileCount + dwOldPckFileCount;
	AfterProcess(mt_lpFileWrite, pckAllInfo, dwAddress);

	delete mt_lpFileWrite;
	DeallocateFileinfo();

	uninitCompressedDataQueue(threadnum);

	//在这里重新打开一次，或者直接打开，由界面线程完成
	lpPckParams->cVarParams.dwOldFileCount = dwOldPckFileCount;
	lpPckParams->cVarParams.dwPrepareToAddFileCount = dwPrepareToAdd;
	lpPckParams->cVarParams.dwChangedFileCount = mt_dwFileCount;
	lpPckParams->cVarParams.dwDuplicateFileCount = mt_dwFileCount - dwFileCount;
	lpPckParams->cVarParams.dwUseNewDataAreaInDuplicateFileSize = dwUseNewDataAreaInDuplicateFile;
	lpPckParams->cVarParams.dwFinalFileCount = pckAllInfo.dwFileCount;

	PrintLogI(TEXT_LOG_WORKING_DONE);

	return TRUE;

}

