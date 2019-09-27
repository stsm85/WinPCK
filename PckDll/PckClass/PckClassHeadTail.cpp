
//#include <Windows.h>
#include "PckClassHeadTail.h"
#include "MapViewFileMultiPck.h"

CPckClassHeadTail::CPckClassHeadTail()
{}

CPckClassHeadTail::~CPckClassHeadTail()
{
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}


const char * CPckClassHeadTail::GetAdditionalInfo()
{
	return m_PckAllInfo.szAdditionalInfo;
}

BOOL CPckClassHeadTail::SetAdditionalInfo(const char* lpszAdditionalInfo)
{
	memcpy(m_PckAllInfo.szAdditionalInfo, lpszAdditionalInfo, PCK_ADDITIONAL_INFO_SIZE);

	if(0 == *m_PckAllInfo.szFilename)
		return FALSE;

	CMapViewFileMultiPckWrite	cWritefile(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.dwMaxSinglePckSize);

	if(NULL == strstr(m_PckAllInfo.szAdditionalInfo, PCK_ADDITIONAL_KEY)) {
		strcpy(m_PckAllInfo.szAdditionalInfo, PCK_ADDITIONAL_INFO);
	}

	if(!cWritefile.OpenPck(m_PckAllInfo.szFilename, OPEN_EXISTING)) {
		Logger_el(UCSTEXT(TEXT_OPENWRITENAME_FAIL), m_PckAllInfo.szFilename);
		return FALSE;
	}

	cWritefile.SetFilePointer(-((QWORD)(m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize)), FILE_END);

	if(!cWritefile.Write(
		m_PckAllInfo.lpSaveAsPckVerFunc->FillTailData(&m_PckAllInfo), 
		m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize)) {

		Logger_el(TEXT_WRITEFILE_FAIL);
		return FALSE;
	}

	return TRUE;
}
