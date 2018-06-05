//////////////////////////////////////////////////////////////////////
// PckClassHelpFunction.cpp: PCK文件功能过程中的子功能集合
//
// 此程序由 李秋枫/stsm/liqf 编写，pck结构参考若水的pck结构.txt，并
// 参考了其易语言代码中并于读pck文件列表的部分
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2015.5.19
//////////////////////////////////////////////////////////////////////

#include "PckClass.h"

#pragma warning ( disable : 4267 )
#pragma warning ( disable : 4996 )

void CPckClass::AfterProcess(CMapViewFileWrite *lpWrite, PCK_ALL_INFOS &PckAllInfo, QWORD &dwAddress, BOOL isRenewAddtional)
{

	assert(NULL != dwAddress);
	assert(0 != PckAllInfo.dwFileCount);

	LPBYTE lpBufferToWrite;

	if(isRenewAddtional)
		strcpy(PckAllInfo.szAdditionalInfo, PCK_ADDITIONAL_INFO
			PCK_ADDITIONAL_INFO_STSM);

	//写pckTail
	if(NULL == (lpBufferToWrite = lpWrite->View(dwAddress, m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize))) {

		m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
		lpWrite->SetFilePointer(dwAddress, FILE_BEGIN);
		dwAddress += lpWrite->Write(m_PckAllInfo.lpSaveAsPckVerFunc->FillTailData(&PckAllInfo), \
			m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize);
	} else {

		memcpy(lpBufferToWrite, m_PckAllInfo.lpSaveAsPckVerFunc->FillTailData(&PckAllInfo), \
			m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize);
		dwAddress += m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize;
		lpWrite->UnmapView();
	}

	//写pckHead
	PckAllInfo.qwPckSize = dwAddress;

	assert(0 != PckAllInfo.qwPckSize);

	if(NULL == (lpBufferToWrite = lpWrite->View(0, m_PckAllInfo.lpSaveAsPckVerFunc->dwHeadSize))) {

		m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
		lpWrite->SetFilePointer(0, FILE_BEGIN);
		lpWrite->Write(m_PckAllInfo.lpSaveAsPckVerFunc->FillHeadData(&PckAllInfo), \
			m_PckAllInfo.lpSaveAsPckVerFunc->dwHeadSize);
	} else {
		memcpy(lpBufferToWrite, m_PckAllInfo.lpSaveAsPckVerFunc->FillHeadData(&PckAllInfo), \
			m_PckAllInfo.lpSaveAsPckVerFunc->dwHeadSize);
		lpWrite->UnmapView();
	}

	lpWrite->UnMaping();

	//这里将文件大小重新设置一下
	lpWrite->SetFilePointer(dwAddress, FILE_BEGIN);
	lpWrite->SetEndOfFile();

}

//重建时计算有效文件数量，排除重复的文件
DWORD CPckClass::ReCountFiles()
{
	DWORD deNewFileCount = 0;
	LPPCKINDEXTABLE lpPckIndexTableSource = m_PckAllInfo.lpPckIndexTable;

	for(DWORD i = 0; i < m_PckAllInfo.dwFileCount; ++i) {

		if(!lpPckIndexTableSource->isInvalid) {
			++deNewFileCount;
		}
		++lpPckIndexTableSource;
	}
	return deNewFileCount;
}

BOOL CPckClass::WritePckIndex(CMapViewFileWrite *lpWrite, LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr, QWORD &dwAddress)
{

	LPBYTE	lpBufferToWrite;
	DWORD	dwNumberOfBytesToMap = lpPckIndexTablePtr->dwIndexDataLength + 8;

	assert(lpPckIndexTablePtr->dwIndexValueHead != 0);

	if(NULL == (lpBufferToWrite = lpWrite->View(dwAddress, dwNumberOfBytesToMap))) {
		m_PckLog.PrintLogEL(TEXT_WRITE_PCK_INDEX, __FILE__, __FUNCTION__, __LINE__);
		assert(FALSE);
		return FALSE;
	}

	memcpy(lpBufferToWrite, lpPckIndexTablePtr, dwNumberOfBytesToMap);
	lpWrite->UnmapView();

	dwAddress += dwNumberOfBytesToMap;

	//窗口中以显示的文件进度
	m_lpPckParams->cVarParams.dwUIProgress++;

	return TRUE;
}


BOOL CPckClass::WritePckIndexTable(CMapViewFileWrite *lpWrite, LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr, DWORD &dwFileCount, QWORD &dwAddress)
{

	//窗口中以显示的文件进度，初始化，显示写索引进度
	m_lpPckParams->cVarParams.dwUIProgress = 0;

	for(DWORD i = 0; i < dwFileCount; i++) {
		if(!WritePckIndex(lpWrite, lpPckIndexTablePtr, dwAddress)) {
			//如果写入失败，则保留前面写入成功的文件
			assert(FALSE);
			dwFileCount = i;
			return FALSE;
		}
		lpPckIndexTablePtr++;

	}

	return TRUE;
}

//写线程失败后的处理
void CPckClass::AfterWriteThreadFailProcess(BOOL *lpbThreadRunning, HANDLE hevtAllCompressFinish, DWORD &dwFileCount, DWORD dwFileHasBeenWritten, QWORD &dwAddressFinal, QWORD dwAddress, BYTE **bufferPtrToWrite)
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

LPPCKINDEXTABLE_COMPRESS CPckClass::FillAndCompressIndexData(LPPCKINDEXTABLE_COMPRESS lpPckIndexTableComped, LPPCKFILEINDEX lpPckFileIndexToCompress)
{
	BYTE pckFileIndexBuf[MAX_INDEXTABLE_CLEARTEXT_LENGTH];
	lpPckIndexTableComped->dwIndexDataLength = MAX_INDEXTABLE_CLEARTEXT_LENGTH;
	compress(lpPckIndexTableComped->buffer, &lpPckIndexTableComped->dwIndexDataLength,
		m_PckAllInfo.lpSaveAsPckVerFunc->FillIndexData(lpPckFileIndexToCompress, pckFileIndexBuf), m_PckAllInfo.lpSaveAsPckVerFunc->dwFileIndexSize, m_lpPckParams->dwCompressLevel);
	//将获取的
	lpPckIndexTableComped->dwIndexValueHead = lpPckIndexTableComped->dwIndexDataLength ^ m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->IndexCompressedFilenameDataLengthCryptKey1;
	lpPckIndexTableComped->dwIndexValueTail = lpPckIndexTableComped->dwIndexDataLength ^ m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->IndexCompressedFilenameDataLengthCryptKey2;

	return lpPckIndexTableComped;
}

DWORD CPckClass::GetCompressBoundSizeByFileSize(LPPCKFILEINDEX	lpPckFileIndex, DWORD dwFileSize)
{
	if(PCK_BEGINCOMPRESS_SIZE < dwFileSize) {
		lpPckFileIndex->dwFileClearTextSize = dwFileSize;
		lpPckFileIndex->dwFileCipherTextSize = this->compressBound(dwFileSize);
	} else {
		lpPckFileIndex->dwFileCipherTextSize = lpPckFileIndex->dwFileClearTextSize = dwFileSize;
	}

	return lpPckFileIndex->dwFileCipherTextSize;
}