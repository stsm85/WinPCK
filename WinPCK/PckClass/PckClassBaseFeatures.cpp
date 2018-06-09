#include <Windows.h>
#include "PckClassBaseFeatures.h"
#include <tchar.h>

CPckClassBaseFeatures::CPckClassBaseFeatures():
	m_PckAllInfo({ 0 }),
	m_firstFile(NULL)
{}

CPckClassBaseFeatures::~CPckClassBaseFeatures()
{}


#ifdef _DEBUG
char *CPckClassBaseFeatures::formatString(const char *format, ...)
{
	const int BUFFER_SIZE = 4097;
	static char strbuf[BUFFER_SIZE];
	memset(strbuf, 0, sizeof(strbuf));

	va_list ap;
	va_start(ap, format);
	int result = ::vsnprintf(strbuf, BUFFER_SIZE - 1, format, ap);
	va_end(ap);

	return strbuf;
}

int CPckClassBaseFeatures::logOutput(const char *file, const char *text)
{
	char szFile[MAX_PATH];
	if(0 == strncmp("CPckClass::", file, strlen("CPckClass::"))) {
		sprintf_s(szFile, "G:\\pcktest\\%s.log", file + strlen("CPckClass::"));
	} else {
		sprintf_s(szFile, "G:\\pcktest\\%s.log", file);
	}


	FILE *pFile = fopen(szFile, "ab");
	if(pFile != NULL) {
		fseek(pFile, 0, SEEK_END);
		//string tstr = getLogString() + " - ";
		//int ret = fwrite(tstr.c_str(), 1, tstr.length(), pFile);
		int ret = fwrite(text, 1, strlen(text), pFile);
		fclose(pFile);
		pFile = NULL;
		return ret;
	}
	return -1;
}
#endif

void CPckClassBaseFeatures::ResetPckInfos()
{
	memset(&m_PckAllInfo, 0, sizeof(PCK_ALL_INFOS));
	m_PckAllInfo.dwAddressOfFilenameIndex = PCK_DATA_START_AT;
}

CONST	LPPCKINDEXTABLE CPckClassBaseFeatures::GetPckIndexTable()
{
	return m_PckAllInfo.lpPckIndexTable;
}

CONST	LPPCK_PATH_NODE CPckClassBaseFeatures::GetPckPathNode()
{
	return &m_PckAllInfo.cRootNode;
}

QWORD CPckClassBaseFeatures::GetPckSize()
{
	return m_PckAllInfo.qwPckSize;
}

DWORD CPckClassBaseFeatures::GetPckFileCount()
{
	return m_PckAllInfo.dwFileCount;
}

QWORD CPckClassBaseFeatures::GetPckDataAreaSize()
{
	return m_PckAllInfo.dwAddressOfFilenameIndex - PCK_DATA_START_AT;
}

QWORD CPckClassBaseFeatures::GetPckRedundancyDataSize()
{
	return m_PckAllInfo.dwAddressOfFilenameIndex - PCK_DATA_START_AT - m_PckAllInfo.cRootNode.child->qdwDirCipherTextSize;
}


#pragma region UI_Params
void CPckClassBaseFeatures::SetParams_ProgressUpper(DWORD dwUIProgressUpper)
{
	m_lpPckParams->cVarParams.dwUIProgress = 0;
	m_lpPckParams->cVarParams.dwUIProgressUpper = dwUIProgressUpper;
}
#pragma endregion
