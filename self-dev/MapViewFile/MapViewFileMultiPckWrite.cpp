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

//OPEN_ALWAYS, CREATE_ALWAYES, OPEN_EXISTING
BOOL CMapViewFileMultiPckWrite::OpenPck(const fs::path& lpszFilename, DWORD dwCreationDisposition, BOOL isNTFSSparseFile)
{
	BOOL rtn = FALSE;

	this->initialization_filenames(lpszFilename);

	for(int i = 0;i < this->m_szPckFileName.size();i++) {
		if(!this->AddFile(this->m_szPckFileName[i], dwCreationDisposition, this->m_Max_PckFile_Size, isNTFSSparseFile))
			break;

		rtn = TRUE;
	}
	this->m_uqwPckStructSize.qwValue = CMapViewFileMulti::GetFileSize();
	return rtn;
}

BOOL CMapViewFileMultiPckWrite::OpenPckAndMappingWrite(const fs::path& lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap, BOOL isNTFSSparseFile)
{
	if(!this->OpenPck(lpFileName, dwCreationDisposition, isNTFSSparseFile))
		return FALSE;

	if(!this->Mapping(qdwSizeToMap))
		return FALSE;

	return TRUE;
}
