#include "PckClassWriteOperator.h"
#include "PckClassFileDisk.h"

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4267 )
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4244 )

CPckClassWriteOperator::CPckClassWriteOperator()
{}

CPckClassWriteOperator::~CPckClassWriteOperator()
{}

/********************************
*
*公共函数
*
********************/

BOOL CPckClassWriteOperator::RebuildPckFile(LPCTSTR szRebuildPckFile, BOOL bUseRecompress)
{
	BOOL rtn = bUseRecompress ? RecompressPckFile(szRebuildPckFile) : RebuildPckFile(szRebuildPckFile);

	//重建后清除过滤数据
	ResetRebuildFilterInIndexList(m_PckAllInfo);

	return rtn;
}

BOOL CPckClassWriteOperator::RebuildPckFile(LPCTSTR szRebuildPckFile)
{

	m_PckLog.PrintLogI(TEXT_LOG_REBUILD);

	
	QWORD	dwAddress = PCK_DATA_START_AT;
	DWORD	dwFileCount = m_PckAllInfo.dwFileCount;
	DWORD	dwValidFileCount = ReCountFiles();
	QWORD	dwTotalFileSizeAfterRebuild = GetPckFilesizeRebuild(szRebuildPckFile, m_PckAllInfo.qwPckSize);

	//构造头和尾时需要的参数
	PCK_ALL_INFOS		pckAllInfo;
	//构造头和尾时需要的参数
	memcpy(&pckAllInfo, &m_PckAllInfo, sizeof(PCK_ALL_INFOS));
	_tcscpy_s(pckAllInfo.szNewFilename, szRebuildPckFile);

	//设置界面进度条总值
	SetParams_ProgressUpper(dwValidFileCount);

	//打开源文件 
	CMapViewFileRead	cFileRead;
	if(!cFileRead.OpenPckAndMappingRead(pckAllInfo.szFilename)) 
		return FALSE;

	//打开目标文件 
	//以下是创建一个文件，用来保存重建后的文件
	CMapViewFileWrite	cFileWrite(pckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->dwMaxSinglePckSize);

	if(!cFileWrite.OpenPckAndMappingWrite(szRebuildPckFile, CREATE_ALWAYS, dwTotalFileSizeAfterRebuild))
		return FALSE;

	vector<PCKINDEXTABLE_COMPRESS> cPckIndexTable(dwValidFileCount);

	//不使用Enum进行遍历处理，改用_PCK_INDEX_TABLE

	LPPCKINDEXTABLE lpPckIndexTableSource = pckAllInfo.lpPckIndexTable;
	pckAllInfo.dwFileCountToAdd = 0;

	for(DWORD i = 0; i < dwFileCount; ++i) {

		if(!m_lpPckParams->cVarParams.bThreadRunning) {
			m_PckLog.PrintLogW(TEXT_USERCANCLE);
			break;
		}

		if(lpPckIndexTableSource->isInvalid) {
			++lpPckIndexTableSource;
			continue;
		}

		LPBYTE lpBufferToWrite, lpBufferToRead;

		DWORD dwNumberOfBytesToMap = lpPckIndexTableSource->cFileIndex.dwFileCipherTextSize;

		if(NULL == (lpBufferToWrite = cFileWrite.View(dwAddress, dwNumberOfBytesToMap))) {
			m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
			return FALSE;
		}

		DWORD dwSrcAddress = lpPckIndexTableSource->cFileIndex.dwAddressOffset;	//保存原来的地址

		if(NULL == (lpBufferToRead = cFileRead.View(dwSrcAddress, dwNumberOfBytesToMap))) {
			m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
			return FALSE;
		}

		memcpy(lpBufferToWrite, lpBufferToRead, dwNumberOfBytesToMap);

		cFileRead.UnmapView();
		cFileWrite.UnmapView();

		//写入此文件的PckFileIndex文件压缩信息并压缩
		lpPckIndexTableSource->cFileIndex.dwAddressOffset = dwAddress;	//此文件的压缩数据起始地址

		dwAddress += lpPckIndexTableSource->cFileIndex.dwFileCipherTextSize;	//下一个文件的压缩数据起始地址

		FillAndCompressIndexData(&cPckIndexTable[pckAllInfo.dwFileCountToAdd], &lpPckIndexTableSource->cFileIndex);

		lpPckIndexTableSource->cFileIndex.dwAddressOffset = dwSrcAddress;	//还原原来的地址

		++lpPckIndexTableSource;
		++(pckAllInfo.dwFileCountToAdd);
		SetParams_ProgressInc();

	}

	pckAllInfo.dwFileCountOld = pckAllInfo.dwFileCount = 0;
	pckAllInfo.lpPckIndexTableToAdd = &cPckIndexTable;

	//关闭读文件
	//写文件索引
	pckAllInfo.dwAddressOfFilenameIndex = dwAddress;

	WriteAllIndex(&cFileWrite, &pckAllInfo, dwAddress);

	AfterProcess(&cFileWrite, pckAllInfo, dwAddress);

	m_PckLog.PrintLogI(TEXT_LOG_WORKING_DONE);

	return TRUE;
}


//重压缩文件
BOOL CPckClassWriteOperator::RecompressPckFile(LPCTSTR szRecompressPckFile)
{

	m_PckLog.PrintLogI(TEXT_LOG_RECOMPRESS);

	QWORD	dwAddress = PCK_DATA_START_AT;
	DWORD	dwFileCount = m_PckAllInfo.dwFileCount;
	DWORD	dwNoDupFileCount = ReCountFiles();
	QWORD	dwTotalFileSizeAfterRebuild = GetPckFilesizeRebuild(szRecompressPckFile, m_PckAllInfo.qwPckSize);

	THREAD_PARAMS		cThreadParams;
	CMapViewFileRead	cFileRead;
	int					threadnum = m_lpPckParams->dwMTThread;

	//构造头和尾时需要的参数
	PCK_ALL_INFOS		pckAllInfo;
	memcpy(&pckAllInfo, &m_PckAllInfo, sizeof(PCK_ALL_INFOS));
	_tcscpy_s(pckAllInfo.szNewFilename, szRecompressPckFile);

	//设置界面进度条总值
	SetParams_ProgressUpper(dwNoDupFileCount);

	//打开源文件 
	if(!cFileRead.OpenPckAndMappingRead(pckAllInfo.szFilename))
		return FALSE;

#pragma region 创建目标文件
	CMapViewFileWrite cFileWriter(pckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->dwMaxSinglePckSize);

	//OPEN_ALWAYS，新建新的pck(CREATE_ALWAYS)或更新存在的pck(OPEN_EXISTING)
	if(!cFileWriter.OpenPckAndMappingWrite(pckAllInfo.szNewFilename, CREATE_ALWAYS, dwTotalFileSizeAfterRebuild))
		return FALSE;

#pragma endregion

	cThreadParams.pThis = (CPckClassThreadWorker*)this;
	cThreadParams.cDataFetchMethod.lpFileReadPCK = &cFileRead;
	cThreadParams.cDataFetchMethod.dwProcessIndex = 0;
	cThreadParams.cDataFetchMethod.dwtotalIndexCount = pckAllInfo.dwFileCount;
	cThreadParams.cDataFetchMethod.lpPckIndexTablePtrSrc = pckAllInfo.lpPckIndexTable;
	cThreadParams.GetUncompressedData = &GetUncompressedDataFromPCK;

	if(!initMultiThreadVars(&cFileWriter)) {
		assert(FALSE);
		return FALSE;
	}

	if(!initCompressedDataQueue(dwFileCount, PCK_DATA_START_AT)) {
		assert(FALSE);
		return FALSE;
	}

	mt_lpFileRead = &cFileRead;
	//mt_dwFileCount = dwFileCount;
	mt_dwFileCountOfWriteTarget = dwNoDupFileCount;

	ExecuteMainThreadGroup(pckAllInfo, threadnum, &cThreadParams);

	//写文件索引

	pckAllInfo.lpPckIndexTable = NULL;
	pckAllInfo.dwFileCountOld = pckAllInfo.dwFileCount = 0;

	//取消后，文件数量
	dwAddress = pckAllInfo.dwAddressOfFilenameIndex;

	WriteAllIndex(&cFileWriter, &pckAllInfo, dwAddress);
	AfterProcess(&cFileWriter, pckAllInfo, dwAddress);

	uninitCompressedDataQueue(threadnum);

	m_PckLog.PrintLogN(TEXT_LOG_WORKING_DONE);

	return TRUE;
}