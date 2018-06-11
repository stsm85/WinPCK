
#include "PckClassAppendFiles.h"
#include "PckClassFileDisk.h"
#include "CharsCodeConv.h"

CPckClassAppendFiles::CPckClassAppendFiles()
{}

CPckClassAppendFiles::~CPckClassAppendFiles()
{}


//新建、更新pck包
BOOL CPckClassAppendFiles::UpdatePckFile(LPCTSTR szPckFile, vector<tstring> &lpszFilePath, const LPPCK_PATH_NODE lpNodeToInsert)
{
	DWORD		dwNewFileCount = 0, dwOldPckFileCount;			//文件数量, 原pck文件中的文件数
	QWORD		qwTotalNewFileSize = 0;						//未压缩时所有文件大小

	int			level = m_lpPckParams->dwCompressLevel;
	int			threadnum = m_lpPckParams->dwMTThread;

	QWORD		dwAddress;

	//开始查找文件
	LinkList<FILES_TO_COMPRESS> cFileLinkList;
	LPPCK_PATH_NODE		lpNodeToInsertPtr;

#pragma region 设置参数

	if(m_PckAllInfo.isPckFileLoaded) {

		dwOldPckFileCount = m_PckAllInfo.dwFileCount;

		lpNodeToInsertPtr = lpNodeToInsert;

		//取得当前节点的相对路径
		if(!GetCurrentNodeString(mt_szCurrentNodeString, lpNodeToInsert)) {
			assert(FALSE);
			return FALSE;
		}

		mt_nCurrentNodeStringLen = strlen(mt_szCurrentNodeString);

		m_PckLog.PrintLogI(TEXT_LOG_UPDATE_ADD
			"-"
			TEXT_LOG_LEVEL_THREAD, level, threadnum);

	} else {

		m_PckAllInfo.dwAddressOfFilenameIndex = PCK_DATA_START_AT;
		dwOldPckFileCount = 0;
		lpNodeToInsertPtr = m_PckAllInfo.cRootNode.child;
		*mt_szCurrentNodeString = 0;
		mt_nCurrentNodeStringLen = 0;

		m_PckLog.PrintLogI(TEXT_LOG_UPDATE_NEW
			"-"
			TEXT_LOG_LEVEL_THREAD, level, threadnum);

	}

	_tcscpy_s(m_PckAllInfo.szNewFilename, szPckFile);
	m_firstFile = cFileLinkList.first();

#pragma endregion

#pragma region 遍历待添加的文件 
	if(!EnumAllFilesByPathList(lpszFilePath, dwNewFileCount, qwTotalNewFileSize, &cFileLinkList))
		return FALSE;

	if(0 == dwNewFileCount)return TRUE;

	m_PckAllInfo.dwFileCountToAdd = dwNewFileCount;
#pragma endregion
	//参数说明：
	// mt_dwFileCount	添加的文件总数，计重复
	// dwFileCount		计算过程使用参数，在下面的计算过程中将使用此参数表示添加的文件总数，不计重复

	//文件数写入窗口类中保存以显示进度
	SetParams_ProgressUpper(mt_dwFileCount = dwNewFileCount);

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
		if(!FindDuplicateNodeFromFileList(lpNodeToInsertPtr, dwNewFileCount))
			return FALSE;
	}

	//日志
	m_PckLog.PrintLogI(TEXT_UPDATE_FILE_INFO, m_PckAllInfo.dwFileCountToAdd, mt_CompressTotalFileSize);

	//PCK_ALL_INFOS	pckAllInfo;
	//OPEN_ALWAYS，新建新的pck(CREATE_ALWAYS)或更新存在的pck(OPEN_EXISTING)

#pragma region 创建目标文件
	CMapViewFileWrite cFileWriter(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->dwMaxSinglePckSize);

	//OPEN_ALWAYS，新建新的pck(CREATE_ALWAYS)或更新存在的pck(OPEN_EXISTING)
	if(!cFileWriter.OpenPckAndMappingWrite(m_PckAllInfo.szNewFilename, OPEN_ALWAYS, mt_CompressTotalFileSize)) {
		return FALSE;
	}

#pragma endregion

	if(!BeforeMultiThreadProcess(&cFileWriter)) {
		assert(FALSE);
		return FALSE;
	}
	if(!initCompressedDataQueue(threadnum, mt_dwFileCountOfWriteTarget = mt_dwFileCount, dwAddress = m_PckAllInfo.dwAddressOfFilenameIndex)) {
		delete mt_lpFileWrite;
		assert(FALSE);
		return FALSE;
	}

	MultiThreadInitialize(CompressThreadAdd, WriteThread, threadnum);
	dwAddress = mt_dwAddressQueue;

	//打印报告用参数
	DWORD	dwUseNewDataAreaInDuplicateFile = 0;

	//写文件索引
	QWORD	dwAddressName = dwAddress;

	//窗口中以显示的文件进度，初始化，显示写索引进度mt_hFileToWrite
	SetParams_ProgressUpper(dwNewFileCount + dwOldPckFileCount);

	//写原来的文件
	LPPCKINDEXTABLE	lpPckIndexTableSrc = m_PckAllInfo.lpPckIndexTable;

	for(DWORD i = 0; i < dwOldPckFileCount; i++) {

		PCKINDEXTABLE_COMPRESS	pckIndexTableTemp;
		WritePckIndex(mt_lpFileWrite, FillAndCompressIndexData(&pckIndexTableTemp, &lpPckIndexTableSrc->cFileIndex), dwAddress);
		lpPckIndexTableSrc++;

	}

	//写添加的文件
	LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr = mt_lpPckIndexTable;

	mt_dwFileCount = dwNewFileCount = mt_dwFileCountOfWriteTarget;
	for(DWORD i = 0; i < mt_dwFileCountOfWriteTarget; i++) {
		////处理lpPckFileIndex->dwAddressOffset

		if(!lpPckIndexTablePtr->bInvalid) {

			WritePckIndex(mt_lpFileWrite, lpPckIndexTablePtr, dwAddress);

		} else {

			assert(0 != dwNewFileCount);
			dwNewFileCount--;
		}

		if(0 == lpPckIndexTablePtr->dwAddressOfDuplicateOldDataArea)
			dwUseNewDataAreaInDuplicateFile++;

		lpPckIndexTablePtr++;

	}

	m_PckAllInfo.dwAddressOfFilenameIndex = dwAddressName;
	//pckAllInfo.dwFileCount = dwFileCount + dwOldPckFileCount;
	AfterProcess(mt_lpFileWrite, m_PckAllInfo, dwAddress);

	uninitCompressedDataQueue(threadnum);

	//在这里重新打开一次，或者直接打开，由界面线程完成
	m_lpPckParams->cVarParams.dwOldFileCount = dwOldPckFileCount;
	m_lpPckParams->cVarParams.dwPrepareToAddFileCount = m_PckAllInfo.dwFileCountToAdd;
	m_lpPckParams->cVarParams.dwChangedFileCount = mt_dwFileCount;
	m_lpPckParams->cVarParams.dwDuplicateFileCount = mt_dwFileCount - dwNewFileCount;
	m_lpPckParams->cVarParams.dwUseNewDataAreaInDuplicateFileSize = dwUseNewDataAreaInDuplicateFile;
	m_lpPckParams->cVarParams.dwFinalFileCount = m_PckAllInfo.dwFileCount + m_PckAllInfo.dwFileCountToAdd;

	m_PckLog.PrintLogI(TEXT_LOG_WORKING_DONE);

	return TRUE;

}
