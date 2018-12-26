
#include "PckClassHeadTailWriter.h"
CPckClassHeadTailWriter::CPckClassHeadTailWriter()
{}

CPckClassHeadTailWriter::~CPckClassHeadTailWriter()
{}

BOOL CPckClassHeadTailWriter::AfterProcess(CMapViewFileMultiPckWrite *lpWrite, PCK_ALL_INFOS &PckAllInfo, QWORD &dwAddress, BOOL isRenewAddtional)
{

	assert(NULL != dwAddress);
	assert(0 != (PckAllInfo.dwFileCount + PckAllInfo.dwFileCountToAdd));

	if(isRenewAddtional)
		strcpy(PckAllInfo.szAdditionalInfo, PCK_ADDITIONAL_INFO);

	//写pckTail
	if (!lpWrite->Write2(dwAddress, m_PckAllInfo.lpSaveAsPckVerFunc->FillTailData(&PckAllInfo), m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize)) {
		
		m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	dwAddress += m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize;

	//写pckHead
	PckAllInfo.qwPckSize = dwAddress;

	assert(0 != PckAllInfo.qwPckSize);

	if (!lpWrite->Write2(0, m_PckAllInfo.lpSaveAsPckVerFunc->FillHeadData(&PckAllInfo), m_PckAllInfo.lpSaveAsPckVerFunc->dwHeadSize)) {
		m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	lpWrite->UnMaping();

	//这里将文件大小重新设置一下
	lpWrite->SetFilePointer(dwAddress, FILE_BEGIN);
	lpWrite->SetEndOfFile();
	return TRUE;

}