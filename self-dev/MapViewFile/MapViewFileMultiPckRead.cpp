#include "pch_mvf.h"
#include "MapViewFileMultiPck.h"

BOOL CMapViewFileMultiPckRead::OpenPck(const fs::path& lpszFilename)
{
	BOOL rtn = FALSE;

	this->initialization_filenames(lpszFilename);

	for(int i = 0;i < this->m_szPckFileName.size();i++) {
		if (!this->AddFile(this->m_szPckFileName[i])) {
			if (0 < i)
				SetLastError(NOERROR);
			break;
		}

		rtn = TRUE;
	}
	this->m_uqwPckStructSize.qwValue = CMapViewFileMulti::GetFileSize();
	return rtn;
}

BOOL CMapViewFileMultiPckRead::OpenPckAndMappingRead(const fs::path& lpFileName)
{
	if(!(this->OpenPck(lpFileName)))
		return FALSE;

	if(!(this->Mapping()))
		return FALSE;

	return TRUE;
}

QWORD CMapViewFileMultiPckRead::GetFileSize() const
{
	return this->m_uqwPckStructSize.qwValue;
}