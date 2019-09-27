
#include "PckClassHeadTailWriter.h"
CPckClassHeadTailWriter::CPckClassHeadTailWriter()
{}

CPckClassHeadTailWriter::~CPckClassHeadTailWriter()
{
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

BOOL CPckClassHeadTailWriter::WriteHeadAndTail(CMapViewFileMultiPckWrite *lpWrite, LPPCK_ALL_INFOS lpPckAllInfo, QWORD &dwAddress, BOOL isRenewAddtional)
{

	assert(NULL != dwAddress);
	assert(0 != (lpPckAllInfo->dwFileCount + lpPckAllInfo->dwFileCountToAdd));

	if(isRenewAddtional)
		strcpy(lpPckAllInfo->szAdditionalInfo, PCK_ADDITIONAL_INFO);

	//写pckTail
	if (!lpWrite->Write2(dwAddress, m_PckAllInfo.lpSaveAsPckVerFunc->FillTailData(lpPckAllInfo), m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize)) {
		
		Logger_el(TEXT_VIEWMAP_FAIL);
		return FALSE;
	}

	dwAddress += m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize;

	//写pckHead
	lpPckAllInfo->qwPckSize = dwAddress;

	assert(0 != lpPckAllInfo->qwPckSize);

	if (!lpWrite->Write2(0, m_PckAllInfo.lpSaveAsPckVerFunc->FillHeadData(lpPckAllInfo), m_PckAllInfo.lpSaveAsPckVerFunc->dwHeadSize)) {
		Logger_el(TEXT_VIEWMAP_FAIL);
		return FALSE;
	}

	lpWrite->UnMaping();

	//这里将文件大小重新设置一下
	lpWrite->SetFilePointer(dwAddress, FILE_BEGIN);
	lpWrite->SetEndOfFile();
	return TRUE;

}