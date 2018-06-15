
#include "PckClassIndexWriter.h"

CPckClassIndexWriter::CPckClassIndexWriter()
{}

CPckClassIndexWriter::~CPckClassIndexWriter()
{}

//写入单个索引
BOOL CPckClassIndexWriter::WritePckIndex(CMapViewFileWrite *lpWrite, const PCKINDEXTABLE_COMPRESS *lpPckIndexTablePtr, QWORD &dwAddress)
{

	LPBYTE	lpBufferToWrite;
	DWORD	dwNumberOfBytesToMap = lpPckIndexTablePtr->dwIndexDataLength + 8;

	assert(lpPckIndexTablePtr->dwIndexValueHead != 0);

	if(NULL == (lpBufferToWrite = lpWrite->View(dwAddress, dwNumberOfBytesToMap))) {
		m_PckLog.PrintLogEL(TEXT_WRITE_PCK_INDEX, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	memcpy(lpBufferToWrite, lpPckIndexTablePtr->compressed_index_data, dwNumberOfBytesToMap);
	lpWrite->UnmapView();

	dwAddress += dwNumberOfBytesToMap;

	//窗口中以显示的文件进度
	SetParams_ProgressInc();


	return TRUE;
}

//写入全部索引
BOOL CPckClassIndexWriter::WriteAllIndex(CMapViewFileWrite *lpWrite, LPPCK_ALL_INFOS lpPckAllInfo,  QWORD &dwAddress)
{

	//窗口中以显示的文件进度，初始化，显示写索引进度
	SetParams_ProgressUpper(lpPckAllInfo->dwFileCount + lpPckAllInfo->dwFileCountToAdd);

	//写原来的文件
	LPPCKINDEXTABLE	lpPckIndexTableOld = lpPckAllInfo->lpPckIndexTable;
	DWORD dwOldPckFileCount = lpPckAllInfo->dwFileCountOld;
	DWORD dwFinalFileCount = 0;

	for(DWORD i = 0; i < dwOldPckFileCount; i++) {

		if(!lpPckIndexTableOld->isInvalid) {
			PCKINDEXTABLE_COMPRESS	pckIndexTableTemp;
			WritePckIndex(lpWrite, FillAndCompressIndexData(&pckIndexTableTemp, &lpPckIndexTableOld->cFileIndex), dwAddress);
			++dwFinalFileCount;
		}
		lpPckIndexTableOld++;

	}

	lpPckAllInfo->dwFileCount = dwFinalFileCount;

	const vector<PCKINDEXTABLE_COMPRESS> *lpCompedPckIndexTableNew = lpPckAllInfo->lpPckIndexTableToAdd;

	DWORD dwNewPckFileCount = lpPckAllInfo->dwFileCountToAdd;
#ifdef _DEBUG
	DWORD dwVectorSize = 0;
	if(NULL != lpCompedPckIndexTableNew) dwVectorSize = lpCompedPckIndexTableNew->size();
	assert(dwNewPckFileCount <= dwVectorSize);
#endif
	for(DWORD i = 0; i < dwNewPckFileCount; i++) {
		if(!WritePckIndex(lpWrite, &(*lpCompedPckIndexTableNew)[i], dwAddress)) {
			//如果写入失败，则保留前面写入成功的文件
			lpPckAllInfo->dwFinalFileCount = dwFinalFileCount;
			assert(FALSE);
			return FALSE;
		}
		++dwFinalFileCount;

	}

	lpPckAllInfo->dwFinalFileCount = dwFinalFileCount;

	return TRUE;
}