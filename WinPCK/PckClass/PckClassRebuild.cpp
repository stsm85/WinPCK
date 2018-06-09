#include "PckClassRebuild.h"
#include "PckClassFileDisk.h"
#include "PckClassRebuildFilter.h"

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4267 )
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4244 )

CPckClassRebuild::CPckClassRebuild()
{}

CPckClassRebuild::~CPckClassRebuild()
{}

/********************************
*
*公共函数
*
********************/

BOOL CPckClassRebuild::RebuildPckFile(LPCTSTR szRebuildPckFile, BOOL bUseRecompress)
{
	BOOL rtn = bUseRecompress ? RecompressPckFile(szRebuildPckFile) : RebuildPckFile(szRebuildPckFile);

	//重建后清除过滤数据
	NPckClassRebuildFilter::ResetRebuildFilterInIndexList(m_PckAllInfo);

	return rtn;
}

BOOL CPckClassRebuild::RebuildPckFile(LPCTSTR szRebuildPckFile)
{

	m_PckLog.PrintLogI(TEXT_LOG_REBUILD);

	LPPCKINDEXTABLE_COMPRESS	lpPckIndexTable, lpPckIndexTablePtr;
	QWORD	dwAddress = PCK_DATA_START_AT, dwAddressName;
	DWORD	dwFileCount = m_PckAllInfo.dwFileCount;
	DWORD	dwNoDupFileCount = ReCountFiles();
	QWORD	dwTotalFileSizeAfterRebuild = GetPckFilesizeRebuild(szRebuildPckFile, m_PckAllInfo.qwPckSize);

	PCK_ALL_INFOS		pckAllInfo;

	LPBYTE				lpBufferToWrite, lpBufferToRead;
	CMapViewFileRead	cFileRead;

	//构造头和尾时需要的参数
	memset(&pckAllInfo, 0, sizeof(PCK_ALL_INFOS));
	pckAllInfo.lpSaveAsPckVerFunc = m_PckAllInfo.lpSaveAsPckVerFunc;

	//设置界面进度条总值
	m_lpPckParams->cVarParams.dwUIProgressUpper = dwNoDupFileCount;

	//打开源文件 
	if(!cFileRead.OpenPckAndMappingRead(m_PckAllInfo.szFilename)) {
		return FALSE;
	}

	//打开目标文件 
	//以下是创建一个文件，用来保存重建后的文件
	CMapViewFileWrite	cFileWrite(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->dwMaxSinglePckSize);

	if(!cFileWrite.OpenPckAndMappingWrite(szRebuildPckFile, CREATE_ALWAYS, dwTotalFileSizeAfterRebuild)) {
		return FALSE;
	}

	//申请空间,文件名压缩数据 数组
	if(NULL == (lpPckIndexTable = new PCKINDEXTABLE_COMPRESS[dwNoDupFileCount])) {
		m_PckLog.PrintLogEL(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	//不使用Enum进行遍历处理，改用_PCK_INDEX_TABLE
	lpPckIndexTablePtr = lpPckIndexTable;

	LPPCKINDEXTABLE lpPckIndexTableSource = m_PckAllInfo.lpPckIndexTable;
	DWORD		IndexCompressedFilenameDataLengthCryptKey1 = m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->IndexCompressedFilenameDataLengthCryptKey1;
	DWORD		IndexCompressedFilenameDataLengthCryptKey2 = m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->IndexCompressedFilenameDataLengthCryptKey2;
	int			level = m_lpPckParams->dwCompressLevel;

	for(DWORD i = 0; i < dwFileCount; ++i) {

		if(lpPckIndexTableSource->isInvalid) {
			++lpPckIndexTableSource;
			continue;
		}

		DWORD dwNumberOfBytesToMap = lpPckIndexTableSource->cFileIndex.dwFileCipherTextSize;

		if(NULL == (lpBufferToWrite = cFileWrite.View(dwAddress, dwNumberOfBytesToMap))) {
			m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
			delete[] lpPckIndexTable;
			return FALSE;
		}

		DWORD dwSrcAddress = lpPckIndexTableSource->cFileIndex.dwAddressOffset;	//保存原来的地址

		if(NULL == (lpBufferToRead = cFileRead.View(dwSrcAddress, dwNumberOfBytesToMap))) {
			m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
			delete[] lpPckIndexTable;
			return FALSE;
		}

		memcpy(lpBufferToWrite, lpBufferToRead, dwNumberOfBytesToMap);

		cFileRead.UnmapView();

		//写入此文件的PckFileIndex文件压缩信息并压缩
		lpPckIndexTableSource->cFileIndex.dwAddressOffset = dwAddress;	//此文件的压缩数据起始地址

		dwAddress += lpPckIndexTableSource->cFileIndex.dwFileCipherTextSize;	//下一个文件的压缩数据起始地址

		FillAndCompressIndexData(lpPckIndexTablePtr, &lpPckIndexTableSource->cFileIndex);

		lpPckIndexTableSource->cFileIndex.dwAddressOffset = dwSrcAddress;	//还原原来的地址

		cFileWrite.UnmapView();

		++lpPckIndexTablePtr;
		++lpPckIndexTableSource;
		++(m_lpPckParams->cVarParams.dwUIProgress);

	}

	if(!m_lpPckParams->cVarParams.bThreadRunning) {
		m_PckLog.PrintLogW(TEXT_USERCANCLE);
		dwFileCount = m_lpPckParams->cVarParams.dwUIProgress;
	}

	//关闭读文件
	//写文件索引
	dwAddressName = dwAddress;
	WritePckIndexTable(&cFileWrite, lpPckIndexTable, dwNoDupFileCount, dwAddress);

	pckAllInfo.dwAddressOfFilenameIndex = dwAddressName;
	pckAllInfo.dwFileCount = dwNoDupFileCount;
	AfterProcess(&cFileWrite, pckAllInfo, dwAddress);

	delete[] lpPckIndexTable;

	m_PckLog.PrintLogI(TEXT_LOG_WORKING_DONE);

	return TRUE;

}


//重压缩文件
BOOL CPckClassRebuild::RecompressPckFile(LPCTSTR szRecompressPckFile)
{

	m_PckLog.PrintLogI(TEXT_LOG_RECOMPRESS);

	LPPCKINDEXTABLE_COMPRESS	lpPckIndexTable, lpPckIndexTablePtr;
	QWORD	dwAddress = PCK_DATA_START_AT, dwAddressName;
	DWORD	dwFileCount = m_PckAllInfo.dwFileCount;
	DWORD	dwNoDupFileCount = ReCountFiles();
	QWORD	dwTotalFileSizeAfterRebuild = GetPckFilesizeRebuild(szRecompressPckFile, m_PckAllInfo.qwPckSize);

	//CMapViewFileWrite	*lpFileWrite;
	CMapViewFileRead	cFileRead;
	int					threadnum = m_lpPckParams->dwMTThread;

	//构造头和尾时需要的参数
	PCK_ALL_INFOS		pckAllInfo_backup;
	memcpy(&pckAllInfo_backup, &m_PckAllInfo, sizeof(PCK_ALL_INFOS));
	_tcscpy_s(m_PckAllInfo.szNewFilename, szRecompressPckFile);

	//设置界面进度条总值
	SetParams_ProgressUpper(dwNoDupFileCount);

	//打开源文件 
	if(!cFileRead.OpenPckAndMappingRead(m_PckAllInfo.szFilename)) {
		assert(FALSE);
		return FALSE;
	}

#pragma region 创建目标文件
	CMapViewFileWrite cFileWriter(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->dwMaxSinglePckSize);

	//OPEN_ALWAYS，新建新的pck(CREATE_ALWAYS)或更新存在的pck(OPEN_EXISTING)
	if(!cFileWriter.OpenPckAndMappingWrite(m_PckAllInfo.szNewFilename, CREATE_ALWAYS, dwTotalFileSizeAfterRebuild)) {

		return FALSE;
	}

#pragma endregion

	if(!BeforeMultiThreadProcess(&cFileWriter)) {
		assert(FALSE);
		return FALSE;
	}

	if(!initCompressedDataQueue(threadnum, dwFileCount, PCK_DATA_START_AT)) {
		assert(FALSE);
		return FALSE;
	}

	mt_lpFileRead = &cFileRead;
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

	WritePckIndexTable(&cFileWriter, mt_lpPckIndexTable, dwNoDupFileCount, dwAddress);

	m_PckAllInfo.dwAddressOfFilenameIndex = dwAddressName;
	m_PckAllInfo.dwFileCount = dwNoDupFileCount;
	AfterProcess(&cFileWriter, m_PckAllInfo, dwAddress);

	uninitCompressedDataQueue(threadnum);

	m_PckLog.PrintLogN(TEXT_LOG_WORKING_DONE);

	//还原
	memcpy(&m_PckAllInfo, &pckAllInfo_backup, sizeof(PCK_ALL_INFOS));

	return TRUE;
}