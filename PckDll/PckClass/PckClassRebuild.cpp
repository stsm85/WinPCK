#include "PckClassWriteOperator.h"
#include "PckClassFileDisk.h"
#include "PckClassRebuildFilter.h"

#include <functional>

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4267 )
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4244 )

CPckClassWriteOperator::CPckClassWriteOperator()
{}

CPckClassWriteOperator::~CPckClassWriteOperator()
{
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

/********************************
*
*公共函数
*
********************/

BOOL CPckClassWriteOperator::RebuildPckFile(const wchar_t * lpszScriptFile, const wchar_t * szRebuildPckFile, BOOL bUseRecompress)
{
	CPckClassRebuildFilter cScriptFilter;

	if ((nullptr != lpszScriptFile) && (0 != *lpszScriptFile)) 
		cScriptFilter.ApplyScript(lpszScriptFile, &m_PckAllInfo.cRootNode);

	return bUseRecompress ? RecompressPckFile(szRebuildPckFile) : RebuildPckFile(szRebuildPckFile);
}

BOOL CPckClassWriteOperator::StripPck(const wchar_t * lpszStripedPckFile, int flag)
{
	//首先过滤*\textures\*.dds
	CPckClassRebuildFilter cScriptFilter;

	if (PCK_STRIP_DDS & flag) {
		
		cScriptFilter.StripModelTexture(
			m_PckAllInfo.lpPckIndexTable, 
			m_PckAllInfo.dwFileCount,
			m_PckAllInfo.cRootNode.child,
			m_PckAllInfo.szFileTitle
		);
	}
	return RecompressPckFile(lpszStripedPckFile, flag);
}

BOOL CPckClassWriteOperator::RebuildPckFile(const wchar_t * szRebuildPckFile)
{

	Logger.i(TEXT_LOG_REBUILD);

	
	QWORD	dwAddress = PCK_DATA_START_AT;
	DWORD	dwFileCount = m_PckAllInfo.dwFileCount;
	DWORD	dwValidFileCount = ReCountFiles();
	QWORD	dwTotalFileSizeAfterRebuild = GetPckFilesizeRebuild(szRebuildPckFile, m_PckAllInfo.qwPckSize);

	//构造头和尾时需要的参数
	PCK_ALL_INFOS		pckAllInfo;
	//构造头和尾时需要的参数
	memcpy(&pckAllInfo, &m_PckAllInfo, sizeof(PCK_ALL_INFOS));
	wcscpy_s(pckAllInfo.szNewFilename, szRebuildPckFile);

	//线程标记
	SetThreadFlag(TRUE);

	//设置界面进度条总值
	SetParams_ProgressUpper(dwValidFileCount);

	//打开源文件 
	CMapViewFileMultiPckRead	cFileRead;
	if(!cFileRead.OpenPckAndMappingRead(pckAllInfo.szFilename)) 
		return FALSE;

	//打开目标文件 
	//以下是创建一个文件，用来保存重建后的文件
	CMapViewFileMultiPckWrite	cFileWrite(pckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.dwMaxSinglePckSize);

	if(!cFileWrite.OpenPckAndMappingWrite(szRebuildPckFile, CREATE_ALWAYS, dwTotalFileSizeAfterRebuild))
		return FALSE;

	vector<PCKINDEXTABLE_COMPRESS> cPckIndexTable(dwValidFileCount);

	//不使用Enum进行遍历处理，改用_PCK_INDEX_TABLE

	LPPCKINDEXTABLE lpPckIndexTableSource = pckAllInfo.lpPckIndexTable;
	pckAllInfo.dwFileCountToAdd = 0;

	for(DWORD i = 0; i < dwFileCount; ++i) {

		if(CheckIfNeedForcedStopWorking()) {
			Logger.w(TEXT_USERCANCLE);
			break;
		}

		if(lpPckIndexTableSource->isInvalid) {
			++lpPckIndexTableSource;
			continue;
		}

		LPBYTE lpBufferToRead;

		DWORD dwNumberOfBytesToMap = lpPckIndexTableSource->cFileIndex.dwFileCipherTextSize;
		DWORD dwSrcAddress = lpPckIndexTableSource->cFileIndex.dwAddressOffset;	//保存原来的地址

		if (0 != dwNumberOfBytesToMap) {

			if (NULL == (lpBufferToRead = cFileRead.View(dwSrcAddress, dwNumberOfBytesToMap))) {
				Logger_el(TEXT_VIEWMAP_FAIL);
				return FALSE;
			}

			cFileWrite.Write2(dwAddress, lpBufferToRead, dwNumberOfBytesToMap);
			cFileRead.UnmapViewAll();

		}

		//写入此文件的PckFileIndex文件压缩信息并压缩
		lpPckIndexTableSource->cFileIndex.dwAddressOffset = dwAddress;	//此文件的压缩数据起始地址

		dwAddress += dwNumberOfBytesToMap;	//下一个文件的压缩数据起始地址

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
	pckAllInfo.dwAddressOfFileEntry = dwAddress;

	WriteAllIndex(&cFileWrite, &pckAllInfo, dwAddress);

	WriteHeadAndTail(&cFileWrite, &pckAllInfo, dwAddress);

	//线程标记
	SetThreadFlag(FALSE);

	Logger.i(TEXT_LOG_WORKING_DONE);

	return TRUE;
}


//重压缩文件
BOOL CPckClassWriteOperator::RecompressPckFile(const wchar_t * szRecompressPckFile, int iStripMode)
{

	Logger.i(TEXT_LOG_RECOMPRESS);

	//QWORD	dwAddress = PCK_DATA_START_AT;
	//DWORD	dwFileCount = m_PckAllInfo.dwFileCount;
	DWORD	dwNoDupFileCount = ReCountFiles();
	QWORD	dwTotalFileSizeAfterRebuild = GetPckFilesizeRebuild(szRecompressPckFile, m_PckAllInfo.qwPckSize);

	THREAD_PARAMS		cThreadParams;
	CMapViewFileMultiPckRead	cFileRead;

#pragma region 重置压缩参数
	m_zlib.init_compressor(m_lpPckParams->dwCompressLevel);
#pragma endregion

	//构造头和尾时需要的参数
	PCK_ALL_INFOS		pckAllInfo;
	memcpy(&pckAllInfo, &m_PckAllInfo, sizeof(PCK_ALL_INFOS));
	wcscpy_s(pckAllInfo.szNewFilename, szRecompressPckFile);

	//设置界面进度条总值
	SetParams_ProgressUpper(dwNoDupFileCount);

	//打开源文件 
	if(!cFileRead.OpenPckAndMappingRead(pckAllInfo.szFilename))
		return FALSE;

#pragma region 创建目标文件
	CMapViewFileMultiPckWrite cFileWriter(pckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.dwMaxSinglePckSize);

	//OPEN_ALWAYS，新建新的pck(CREATE_ALWAYS)或更新存在的pck(OPEN_EXISTING)
	if(!cFileWriter.OpenPckAndMappingWrite(pckAllInfo.szNewFilename, CREATE_ALWAYS, dwTotalFileSizeAfterRebuild))
		return FALSE;

#pragma endregion

	cThreadParams.cDataFetchMethod.lpFileReadPCK = &cFileRead;
	cThreadParams.cDataFetchMethod.iStripFlag = iStripMode;
	cThreadParams.cDataFetchMethod.dwProcessIndex = 0;
	cThreadParams.cDataFetchMethod.dwTotalIndexCount = pckAllInfo.dwFileCount;
	cThreadParams.cDataFetchMethod.lpPckIndexTablePtrSrc = pckAllInfo.lpPckIndexTable;

	cThreadParams.lpPckClassThreadWorker = this;
	cThreadParams.lpFileWrite = &cFileWriter;
	cThreadParams.pck_data_src = DATA_FROM_PCK;
	cThreadParams.dwAddressStartAt = PCK_DATA_START_AT;
	cThreadParams.lpPckAllInfo = &pckAllInfo;
	cThreadParams.pckParams = m_lpPckParams;
	cThreadParams.dwFileCountOfWriteTarget = dwNoDupFileCount;

	//写文件索引
	pckAllInfo.lpPckIndexTable = NULL;
	pckAllInfo.dwFileCountOld = pckAllInfo.dwFileCount = 0;

	CPckThreadRunner m_threadRunner(&cThreadParams);
	m_threadRunner.start();

	Logger.n(TEXT_LOG_WORKING_DONE);

	return TRUE;
}