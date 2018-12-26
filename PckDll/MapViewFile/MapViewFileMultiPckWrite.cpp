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


BOOL CMapViewFileMultiPckWrite::OpenPck(LPCSTR lpszFilename, DWORD dwCreationDisposition, BOOL isNTFSSparseFile)
{
	BOOL rtn = FALSE;
	strcpy_s(m_szPckFileName[ID_PCK], lpszFilename);
	GetPkXName(m_szPckFileName[ID_PKX], m_szPckFileName[ID_PCK], ID_PKX);
	GetPkXName(m_szPckFileName[ID_PKG], m_szPckFileName[ID_PCK], ID_PKG);

	for(int i = 0;i < ID_END;i++) {
		if(!AddFile(m_szPckFileName[i], dwCreationDisposition, m_Max_PckFile_Size, isNTFSSparseFile))
			break;

		rtn = TRUE;
	}
	m_uqwPckStructSize.qwValue = CMapViewFileMulti::GetFileSize();
	return rtn;
}

//OPEN_ALWAYS, CREATE_ALWAYES, OPEN_EXISTING
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


BOOL CMapViewFileMultiPckWrite::OpenPckAndMappingWrite(LPCSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap, BOOL isNTFSSparseFile)
{
	if(!OpenPck(lpFileName, dwCreationDisposition, isNTFSSparseFile))
		return FALSE;

	if(!Mapping(qdwSizeToMap))
		return FALSE;

	return TRUE;
}

BOOL CMapViewFileMultiPckWrite::OpenPckAndMappingWrite(LPCWSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap, BOOL isNTFSSparseFile)
{
	if(!OpenPck(lpFileName, dwCreationDisposition, isNTFSSparseFile))
		return FALSE;

	if(!Mapping(qdwSizeToMap))
		return FALSE;

	return TRUE;
}
