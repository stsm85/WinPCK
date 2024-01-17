#include "pch_mvf.h"
#include "MapViewFileMultiPck.h"

#if _TEST_MAX_PCK_CELL
CMapViewFileMultiPckWrite::CMapViewFileMultiPckWrite(QWORD qwMaxPckSize):
	m_Max_PckFile_Size(100 * 1024 * 1024)
{}
#else
CMapViewFileMultiPckWrite::CMapViewFileMultiPckWrite(QWORD qwMaxPckSize) :
	m_Max_PckFile_Size(qwMaxPckSize)
{}
#endif


CMapViewFileMultiPckWrite::~CMapViewFileMultiPckWrite()
{}

//OPEN_ALWAYS, CREATE_ALWAYES, OPEN_EXISTING
BOOL CMapViewFileMultiPckWrite::OpenPck(fs::path lpszFilename, DWORD dwCreationDisposition, BOOL isNTFSSparseFile)
{
	BOOL rtn = FALSE;

	this->m_szPckFileName[ID_PCK] = lpszFilename;
	this->m_szPckFileName[ID_PKX] = lpszFilename.replace_extension(".pkx");
	this->m_szPckFileName[ID_PKG] = lpszFilename.replace_extension(".pkg");

	for(int i = 0;i < ID_END;i++) {
		if(!this->AddFile(this->m_szPckFileName[i], dwCreationDisposition, this->m_Max_PckFile_Size, isNTFSSparseFile))
			break;

		rtn = TRUE;
	}
	this->m_uqwPckStructSize.qwValue = CMapViewFileMulti::GetFileSize();
	return rtn;
}


#if 0
BOOL CMapViewFileMultiPckWrite::OpenPck(LPCWSTR lpszFilename, DWORD dwCreationDisposition, BOOL isNTFSSparseFile)
{
	BOOL rtn = FALSE;
	wcscpy_s(m_tszPckFileName[ID_PCK], lpszFilename);
	GetPkXName(m_tszPckFileName[ID_PKX], m_tszPckFileName[ID_PCK], ID_PKX);
	GetPkXName(m_tszPckFileName[ID_PKG], m_tszPckFileName[ID_PCK], ID_PKG);

	for(int i = 0;i < ID_END;i++) {

		if(!AddFile(m_tszPckFileName[i], dwCreationDisposition, m_Max_PckFile_Size, isNTFSSparseFile))
			break;
		rtn = TRUE;
	}
	m_uqwPckStructSize.qwValue = CMapViewFileMulti::GetFileSize();
	return rtn;
}
#endif

BOOL CMapViewFileMultiPckWrite::OpenPckAndMappingWrite(fs::path lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap, BOOL isNTFSSparseFile)
{
	if(!this->OpenPck(lpFileName, dwCreationDisposition, isNTFSSparseFile))
		return FALSE;

	if(!this->Mapping(qdwSizeToMap))
		return FALSE;

	return TRUE;
}

#if 0
BOOL CMapViewFileMultiPckWrite::OpenPckAndMappingWrite(LPCWSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap, BOOL isNTFSSparseFile)
{
	if(!OpenPck(lpFileName, dwCreationDisposition, isNTFSSparseFile))
		return FALSE;

	if(!Mapping(qdwSizeToMap))
		return FALSE;

	return TRUE;
}
#endif