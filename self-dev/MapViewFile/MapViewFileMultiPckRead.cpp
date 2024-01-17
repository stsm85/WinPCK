#include "pch_mvf.h"
#include "MapViewFileMultiPck.h"


CMapViewFileMultiPckRead::CMapViewFileMultiPckRead()
{}

CMapViewFileMultiPckRead::~CMapViewFileMultiPckRead()
{}

BOOL CMapViewFileMultiPckRead::OpenPck(fs::path lpszFilename)
{
	BOOL rtn = FALSE;

	this->m_szPckFileName[ID_PCK] = lpszFilename;
	this->m_szPckFileName[ID_PKX] = lpszFilename.replace_extension(".pkx");
	this->m_szPckFileName[ID_PKG] = lpszFilename.replace_extension(".pkg");

	for(int i = 0;i < ID_END;i++) {
		if (!this->AddFile(m_szPckFileName[i])) {
			if (0 < i)
				SetLastError(NOERROR);
			break;
		}

		rtn = TRUE;
	}
	this->m_uqwPckStructSize.qwValue = CMapViewFileMulti::GetFileSize();
	return rtn;
}

#if 0
BOOL CMapViewFileMultiPckRead::OpenPck(LPCWSTR lpszFilename)
{
	BOOL rtn = FALSE;
	wcscpy_s(m_tszPckFileName[ID_PCK], lpszFilename);
	GetPkXName(m_tszPckFileName[ID_PKX], m_tszPckFileName[ID_PCK], ID_PKX);
	GetPkXName(m_tszPckFileName[ID_PKG], m_tszPckFileName[ID_PCK], ID_PKG);

	for(int i = 0;i < ID_END;i++) {

		if (!AddFile(m_tszPckFileName[i])) {
			if (0 < i)
				SetLastError(NOERROR);
			break;
		}
		rtn = TRUE;
	}

	m_uqwPckStructSize.qwValue = CMapViewFileMulti::GetFileSize();
	return rtn;
}
#endif

BOOL CMapViewFileMultiPckRead::OpenPckAndMappingRead(fs::path lpFileName)
{
	if(!(this->OpenPck(lpFileName)))
		return FALSE;

	if(!(this->Mapping()))
		return FALSE;

	return TRUE;
}

#if 0
BOOL CMapViewFileMultiPckRead::OpenPckAndMappingRead(LPCWSTR lpFileName)
{
	if(!(OpenPck(lpFileName)))
		return FALSE;

	if(!(Mapping()))
		return FALSE;

	return TRUE;
}
#endif

#if 0
LPBYTE CMapViewFileMultiPckRead::OpenMappingAndViewAllRead(LPCSTR lpFileName)
{
	if(OpenPckAndMappingRead(lpFileName))
		return View(0, 0);
	else
		return NULL;
}

LPBYTE CMapViewFileMultiPckRead::OpenMappingAndViewAllRead(LPCWSTR lpFileName)
{
	if(OpenPckAndMappingRead(lpFileName))
		return View(0, 0);
	else
		return NULL;
}
#endif

QWORD CMapViewFileMultiPckRead::GetFileSize() const
{
	return m_uqwPckStructSize.qwValue;
}