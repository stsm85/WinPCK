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

char* CPckClass::UCS2toA(LPCWSTR src, int max_len)
{
	static char dst[8192];
	::WideCharToMultiByte(CP_ACP, 0, src, max_len, dst, 8192, "_", 0);
	return dst;
}

BOOL CPckClass::OpenPckAndMappingRead(CMapViewFileRead *lpRead, LPCSTR lpFileName, LPCSTR lpszMapNamespace)
{

	if(!(lpRead->OpenPck(lpFileName))){

		PrintLogE(TEXT_OPENNAME_FAIL, lpFileName, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	if(!(lpRead->Mapping(lpszMapNamespace))){

		PrintLogE(TEXT_CREATEMAPNAME_FAIL, lpFileName, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;

	}

	return TRUE;
}

BOOL CPckClass::OpenPckAndMappingRead(CMapViewFileRead *lpRead, LPCWSTR lpFileName, LPCSTR lpszMapNamespace)
{

	if(!(lpRead->OpenPck(lpFileName))){

		PrintLogE(TEXT_OPENNAME_FAIL, lpFileName, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	if(!(lpRead->Mapping(lpszMapNamespace))){

		PrintLogE(TEXT_CREATEMAPNAME_FAIL, lpFileName, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;

	}

	return TRUE;
}

LPBYTE CPckClass::OpenMappingAndViewAllRead(CMapViewFileRead *lpRead, LPCSTR lpFileName, LPCSTR lpszMapNamespace)
{
	if (OpenPckAndMappingRead(lpRead, lpFileName, lpszMapNamespace))
		return lpRead->View(0, 0);
	else
		return NULL;
}

BOOL CPckClass::OpenPckAndMappingWrite(CMapViewFileWrite *lpWrite, LPCTSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap)
{
	if (!lpWrite->OpenPck(lpFileName, dwCreationDisposition))
	{
		PrintLogE(TEXT_OPENWRITENAME_FAIL, lpFileName, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	if(!lpWrite->Mapping(m_szMapNameWrite, qdwSizeToMap))
	{
		PrintLogE(TEXT_CREATEMAPNAME_FAIL, lpFileName, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	return TRUE;
}


void CPckClass::AfterProcess(CMapViewFileWrite *lpWrite, PCK_ALL_INFOS &PckAllInfo, QWORD &dwAddress, BOOL isRenewAddtional)
{

#ifdef _DEBUG
	assert(NULL != dwAddress);
	assert(0 != PckAllInfo.dwFileCount);
#endif

	LPBYTE lpBufferToWrite;

	if(isRenewAddtional)
		strcpy(PckAllInfo.szAdditionalInfo,	PCK_ADDITIONAL_INFO
											PCK_ADDITIONAL_INFO_STSM);

	//写pckTail
	if (NULL == (lpBufferToWrite = lpWrite->View(dwAddress, m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize))) {

		PrintLogE(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
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

#ifdef _DEBUG
	assert(0 != PckAllInfo.qwPckSize);
#endif

	if (NULL == (lpBufferToWrite = lpWrite->View(0, m_PckAllInfo.lpSaveAsPckVerFunc->dwHeadSize))) {

		PrintLogE(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
		lpWrite->SetFilePointer(0, FILE_BEGIN);
		lpWrite->Write(m_PckAllInfo.lpSaveAsPckVerFunc->FillHeadData(&PckAllInfo), \
						m_PckAllInfo.lpSaveAsPckVerFunc->dwHeadSize);
	}
	else {
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
	LPPCKINDEXTABLE lpPckIndexTableSource = m_lpPckIndexTable;

	for (DWORD i = 0; i < m_PckAllInfo.dwFileCount; ++i) {

		if (!lpPckIndexTableSource->isInvalid) {
			++deNewFileCount;
		}
		++lpPckIndexTableSource;
	}
	return deNewFileCount;
}

//在写文件时，文件空间申请的过小，重新申请一下
BOOL CPckClass::IsNeedExpandWritingFile(
	CMapViewFileWrite *lpWrite,
	QWORD dwAddress, 
	QWORD dwFileSize,
	QWORD &dwCompressTotalFileSize
	)
{
	//判断一下dwAddress的值会不会超过dwTotalFileSizeAfterCompress
	//如果超过，说明文件空间申请的过小，重新申请一下ReCreateFileMapping
	//新文件大小在原来的基础上增加(lpfirstFile->dwFileSize + 1mb) >= 64mb ? (lpfirstFile->dwFileSize + 1mb) :64mb
	//1mb=0x100000
	//64mb=0x4000000
	if ((dwAddress + dwFileSize + PCK_SPACE_DETECT_SIZE) > dwCompressTotalFileSize)
	{
		lpWrite->UnMaping();

		dwCompressTotalFileSize +=
			((dwFileSize + PCK_SPACE_DETECT_SIZE) > PCK_STEP_ADD_SIZE ? (dwFileSize + PCK_SPACE_DETECT_SIZE) : PCK_STEP_ADD_SIZE);

		if (!lpWrite->Mapping(m_szMapNameWrite, dwCompressTotalFileSize))
			return FALSE;
	}
	return TRUE;
}


BOOL CPckClass::WritePckIndex(CMapViewFileWrite *lpWrite, LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr, QWORD &dwAddress)
{

	LPBYTE	lpBufferToWrite;
	DWORD	dwNumberOfBytesToMap = lpPckIndexTablePtr->dwIndexDataLength + 8;

#ifdef _DEBUG
	assert(lpPckIndexTablePtr->dwIndexValueHead != 0);
#endif

	if (NULL == (lpBufferToWrite = lpWrite->View(dwAddress, dwNumberOfBytesToMap)))
	{
		PrintLogE(TEXT_WRITE_PCK_INDEX, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	memcpy(lpBufferToWrite, lpPckIndexTablePtr, dwNumberOfBytesToMap);
	lpWrite->UnmapView();

	dwAddress += dwNumberOfBytesToMap;

	//窗口中以显示的文件进度
	lpPckParams->cVarParams.dwUIProgress++;

	return TRUE;
}


BOOL CPckClass::WritePckIndexTable(CMapViewFileWrite *lpWrite, LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr, DWORD &dwFileCount,  QWORD &dwAddress)
{

	//窗口中以显示的文件进度，初始化，显示写索引进度
	lpPckParams->cVarParams.dwUIProgress = 0;

	for (DWORD i = 0; i < dwFileCount; i++)
	{
		if (!WritePckIndex(lpWrite, lpPckIndexTablePtr, dwAddress)) {
			//如果写入失败，则保留前面写入成功的文件
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
	while (0 != *bufferPtrToWrite)
	{
		if (1 != (int)(*bufferPtrToWrite))
			free(*bufferPtrToWrite);
		bufferPtrToWrite++;
	}

}

LPPCKINDEXTABLE_COMPRESS CPckClass::FillAndCompressIndexData(LPPCKINDEXTABLE_COMPRESS lpPckIndexTableComped, LPPCKFILEINDEX lpPckFileIndexToCompress)
{
	BYTE pckFileIndexBuf[MAX_INDEXTABLE_CLEARTEXT_LENGTH];
	lpPckIndexTableComped->dwIndexDataLength = MAX_INDEXTABLE_CLEARTEXT_LENGTH;
	compress(lpPckIndexTableComped->buffer, &lpPckIndexTableComped->dwIndexDataLength,
		m_PckAllInfo.lpSaveAsPckVerFunc->FillIndexData(lpPckFileIndexToCompress, pckFileIndexBuf), m_PckAllInfo.lpSaveAsPckVerFunc->dwFileIndexSize, lpPckParams->dwCompressLevel);
	//将获取的
	lpPckIndexTableComped->dwIndexValueHead = lpPckIndexTableComped->dwIndexDataLength ^ m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->IndexCompressedFilenameDataLengthCryptKey1;
	lpPckIndexTableComped->dwIndexValueTail = lpPckIndexTableComped->dwIndexDataLength ^ m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->IndexCompressedFilenameDataLengthCryptKey2;

	return lpPckIndexTableComped;
}

DWORD CPckClass::GetCompressBoundSizeByFileSize(LPPCKFILEINDEX	lpPckFileIndex, DWORD dwFileSize)
{
	if (PCK_BEGINCOMPRESS_SIZE < dwFileSize)
	{
		lpPckFileIndex->dwFileClearTextSize = dwFileSize;
		lpPckFileIndex->dwFileCipherTextSize = this->compressBound(dwFileSize);
	}
	else {
		lpPckFileIndex->dwFileCipherTextSize = lpPckFileIndex->dwFileClearTextSize = dwFileSize;
	}

	return lpPckFileIndex->dwFileCipherTextSize;
}