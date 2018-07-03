
#include "PckClassHeadTailWriter.h"
CPckClassHeadTailWriter::CPckClassHeadTailWriter()
{}

CPckClassHeadTailWriter::~CPckClassHeadTailWriter()
{}

BOOL CPckClassHeadTailWriter::AfterProcess(CMapViewFileMultiPckWrite *lpWrite, PCK_ALL_INFOS &PckAllInfo, QWORD &dwAddress, BOOL isRenewAddtional)
{

	assert(NULL != dwAddress);
	assert(0 != (PckAllInfo.dwFileCount + PckAllInfo.dwFileCountToAdd));

	LPBYTE lpBufferToWrite;

	if(isRenewAddtional)
		strcpy(PckAllInfo.szAdditionalInfo, PCK_ADDITIONAL_INFO
			PCK_ADDITIONAL_INFO_STSM);

	//写pckTail
	if(NULL == (lpBufferToWrite = lpWrite->View(dwAddress, m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize))) {

		m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
		//lpWrite->SetFilePointer(dwAddress, FILE_BEGIN);
		//dwAddress += lpWrite->Write(m_PckAllInfo.lpSaveAsPckVerFunc->FillTailData(&PckAllInfo), \
		//	m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize);
		return FALSE;
	} else {

		memcpy(lpBufferToWrite, m_PckAllInfo.lpSaveAsPckVerFunc->FillTailData(&PckAllInfo), \
			m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize);
		dwAddress += m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize;
		lpWrite->UnmapViewAll();
	}

	//写pckHead
	PckAllInfo.qwPckSize = dwAddress;

	assert(0 != PckAllInfo.qwPckSize);

	if(NULL == (lpBufferToWrite = lpWrite->View(0, m_PckAllInfo.lpSaveAsPckVerFunc->dwHeadSize))) {

		m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
		//lpWrite->SetFilePointer(0, FILE_BEGIN);
		//lpWrite->Write(m_PckAllInfo.lpSaveAsPckVerFunc->FillHeadData(&PckAllInfo), \
		//	m_PckAllInfo.lpSaveAsPckVerFunc->dwHeadSize);
		return FALSE;
	} else {
		memcpy(lpBufferToWrite, m_PckAllInfo.lpSaveAsPckVerFunc->FillHeadData(&PckAllInfo), \
			m_PckAllInfo.lpSaveAsPckVerFunc->dwHeadSize);
		lpWrite->UnmapViewAll();
	}

	lpWrite->UnMaping();

	//这里将文件大小重新设置一下
	lpWrite->SetFilePointer(dwAddress, FILE_BEGIN);
	lpWrite->SetEndOfFile();
	return TRUE;

}