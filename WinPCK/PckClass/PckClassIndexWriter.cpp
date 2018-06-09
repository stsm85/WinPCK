
#include "PckClassIndexWriter.h"

CPckClassIndexWriter::CPckClassIndexWriter()
{}

CPckClassIndexWriter::~CPckClassIndexWriter()
{}

//写入单个索引
BOOL CPckClassIndexWriter::WritePckIndex(CMapViewFileWrite *lpWrite, LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr, QWORD &dwAddress)
{

	LPBYTE	lpBufferToWrite;
	DWORD	dwNumberOfBytesToMap = lpPckIndexTablePtr->dwIndexDataLength + 8;

	assert(lpPckIndexTablePtr->dwIndexValueHead != 0);

	if(NULL == (lpBufferToWrite = lpWrite->View(dwAddress, dwNumberOfBytesToMap))) {
		m_PckLog.PrintLogEL(TEXT_WRITE_PCK_INDEX, __FILE__, __FUNCTION__, __LINE__);
		assert(FALSE);
		return FALSE;
	}

	memcpy(lpBufferToWrite, lpPckIndexTablePtr->compressed_index_data, dwNumberOfBytesToMap);
	lpWrite->UnmapView();

	dwAddress += dwNumberOfBytesToMap;

	//窗口中以显示的文件进度
	m_lpPckParams->cVarParams.dwUIProgress++;

	return TRUE;
}

//写入全部索引
BOOL CPckClassIndexWriter::WritePckIndexTable(CMapViewFileWrite *lpWrite, LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr, DWORD &dwFileCount, QWORD &dwAddress)
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