
#include "PckClassWriteOperator.h"

//新建、更新pck包
BOOL CPckClassWriteOperator::UpdatePckFile(LPCTSTR szPckFile, const vector<tstring> &lpszFilePath, const PCK_PATH_NODE* lpNodeToInsert)
{
	DWORD		dwNewFileCount = 0;			//文件数量, 原pck文件中的文件数
	DWORD		dwDuplicateFileCount = 0;
	QWORD		qwTotalNewFileSize = 0;						//未压缩时所有文件大小

	int			level = m_lpPckParams->dwCompressLevel;
	int			threadnum = m_lpPckParams->dwMTThread;

	QWORD		dwAddressWhereToAppendData;
	THREAD_PARAMS		cThreadParams;

	//开始查找文件
	const PCK_PATH_NODE*		lpNodeToInsertPtr;

#pragma region 重置压缩参数
	m_zlib.init_compressor(level);
#pragma endregion


#pragma region 设置参数
	
	m_FilesToBeAdded.clear();
	m_PckAllInfo.lpFilesToBeAdded = &m_FilesToBeAdded;

	if(m_PckAllInfo.isPckFileLoaded) {

		lpNodeToInsertPtr = lpNodeToInsert;

		//从文件尾添加数据，操作失败后可回退
		dwAddressWhereToAppendData = m_PckAllInfo.qwPckSize;

		//取得当前节点的相对路径
		if(!GetCurrentNodeString(cThreadParams.cDataFetchMethod.szCurrentNodeString, lpNodeToInsert)) {
			assert(FALSE);
			return FALSE;
		}

		cThreadParams.cDataFetchMethod.nCurrentNodeStringLen = wcslen(cThreadParams.cDataFetchMethod.szCurrentNodeString);

		m_PckLog.PrintLogI(TEXT_LOG_UPDATE_ADD
			"-"
			TEXT_LOG_LEVEL_THREAD, level, threadnum);

	} else {
		//新建文件
		//m_PckAllInfo.dwAddressOfFilenameIndex = PCK_DATA_START_AT;
		dwAddressWhereToAppendData = PCK_DATA_START_AT;

		lpNodeToInsertPtr = m_PckAllInfo.cRootNode.child;
		*cThreadParams.cDataFetchMethod.szCurrentNodeString = 0;
		cThreadParams.cDataFetchMethod.nCurrentNodeStringLen = 0;

		m_PckLog.PrintLogI(TEXT_LOG_UPDATE_NEW
			"-"
			TEXT_LOG_LEVEL_THREAD, level, threadnum);

	}

	_tcscpy_s(m_PckAllInfo.szNewFilename, szPckFile);

#pragma endregion

#pragma region 遍历待添加的文件 
	if(!EnumAllFilesByPathList(lpszFilePath, dwNewFileCount, qwTotalNewFileSize, m_PckAllInfo.lpFilesToBeAdded))
		return FALSE;

	if(0 == dwNewFileCount)return TRUE;

	m_PckAllInfo.dwFileCountToAdd = dwNewFileCount;
#pragma endregion
	//参数说明：
	// mt_dwFileCount	添加的文件总数，计重复
	// dwFileCount		计算过程使用参数，在下面的计算过程中将使用此参数表示添加的文件总数，不计重复

	//文件数写入窗口类中保存以显示进度
	SetParams_ProgressUpper(dwNewFileCount);

	//计算大概需要多大空间qwTotalFileSize
	mt_CompressTotalFileSize = GetPckFilesizeByCompressed(szPckFile, qwTotalNewFileSize, m_PckAllInfo.qwPckSize);

	//与原来目录中的文件对比，是否有重名
	//策略：无条件覆盖吧				如果重名且都为文件或文件夹，则覆盖
	//
	//调用FindFileNode返回-1退出，返回0，表示直接添加，非0就是有重复的
	//写专用的writethread和compressthread,以调用
	//在PCKINDEXTABLE_COMPRESS里添加add专用属性，以判断是否启用此节点（重名时）0使用，1不使用
	//结束 时使用2个循环写入压缩索引 

	//dwFileCount变量在此处指的是添加的文件除去重名的数量 
	if(m_PckAllInfo.isPckFileLoaded) {
		if(!FindDuplicateNodeFromFileList(lpNodeToInsertPtr, dwDuplicateFileCount))
			return FALSE;
	}

	//日志
	m_PckLog.PrintLogI(TEXT_UPDATE_FILE_INFO, m_PckAllInfo.dwFileCountToAdd, mt_CompressTotalFileSize);

#pragma region 创建目标文件
	CMapViewFileMultiPckWrite cFileWriter(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.dwMaxSinglePckSize);

	//OPEN_ALWAYS，新建新的pck(CREATE_ALWAYS)或更新存在的pck(OPEN_EXISTING)
	if(!cFileWriter.OpenPckAndMappingWrite(m_PckAllInfo.szNewFilename, OPEN_ALWAYS, mt_CompressTotalFileSize)) {
		return FALSE;
	}

#pragma endregion

	cThreadParams.pThis = (CPckClassThreadWorker*)this;
	cThreadParams.cDataFetchMethod.ciFilesList = m_PckAllInfo.lpFilesToBeAdded->cbegin();
	cThreadParams.cDataFetchMethod.ciFilesListEnd = m_PckAllInfo.lpFilesToBeAdded->cend();

	cThreadParams.GetUncompressedData = &GetUncompressedDataFromFile;

	//呵呵，下面两个函数好像永远返回TRUE
	if(!initMultiThreadVars(&cFileWriter)) {
		return FALSE;
	}

	if(!initCompressedDataQueue(mt_dwFileCountOfWriteTarget = dwNewFileCount, dwAddressWhereToAppendData/*m_PckAllInfo.dwAddressOfFilenameIndex*/)) {
		return FALSE;
	}

	ExecuteMainThreadGroup(m_PckAllInfo, threadnum, &cThreadParams);
	dwAddressWhereToAppendData = m_PckAllInfo.dwAddressOfFilenameIndex;

	//写文件索引
	m_PckAllInfo.dwFileCount = m_PckAllInfo.dwFileCountOld - dwDuplicateFileCount;

	WriteAllIndex(mt_lpFileWrite, &m_PckAllInfo, dwAddressWhereToAppendData);

	AfterProcess(mt_lpFileWrite, m_PckAllInfo, dwAddressWhereToAppendData);

	uninitCompressedDataQueue();

	//在这里重新打开一次，或者直接打开，由界面线程完成
	m_lpPckParams->cVarParams.dwOldFileCount = m_PckAllInfo.dwFileCountOld;
	m_lpPckParams->cVarParams.dwPrepareToAddFileCount = dwNewFileCount;
	m_lpPckParams->cVarParams.dwChangedFileCount = m_PckAllInfo.dwFileCountToAdd;
	m_lpPckParams->cVarParams.dwDuplicateFileCount = dwDuplicateFileCount;
	//m_lpPckParams->cVarParams.dwUseNewDataAreaInDuplicateFileSize = m_PckAllInfo.dwFinalFileCount - m_PckAllInfo.dwFileCountOld;
	m_lpPckParams->cVarParams.dwFinalFileCount = m_PckAllInfo.dwFinalFileCount;

	m_PckLog.PrintLogI(TEXT_LOG_WORKING_DONE);

	return TRUE;

}
