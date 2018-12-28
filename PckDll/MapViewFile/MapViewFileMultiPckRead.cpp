#include "MapViewFileMultiPck.h"


CMapViewFileMultiPckRead::CMapViewFileMultiPckRead()
{}

CMapViewFileMultiPckRead::~CMapViewFileMultiPckRead()
{}

BOOL CMapViewFileMultiPckRead::OpenPck(LPCSTR lpszFilename)
{
	BOOL rtn = FALSE;
	strcpy_s(m_szPckFileName[ID_PCK], lpszFilename);
	GetPkXName(m_szPckFileName[ID_PKX], m_szPckFileName[ID_PCK], ID_PKX);
	GetPkXName(m_szPckFileName[ID_PKG], m_szPckFileName[ID_PCK], ID_PKG);

	for(int i = 0;i < ID_END;i++) {
		if (!AddFile(m_szPckFileName[i])) {
			if (0 < i)
				SetLastError(NOERROR);
			break;
		}

		rtn = TRUE;
	}
	m_uqwPckStructSize.qwValue = CMapViewFileMulti::GetFileSize();
	return rtn;
}

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

BOOL CMapViewFileMultiPckRead::OpenPckAndMappingRead(LPCSTR lpFileName)
{
	if(!(OpenPck(lpFileName)))
		return FALSE;

	if(!(Mapping()))
		return FALSE;

	return TRUE;
}

BOOL CMapViewFileMultiPckRead::OpenPckAndMappingRead(LPCWSTR lpFileName)
{
	if(!(OpenPck(lpFileName)))
		return FALSE;

	if(!(Mapping()))
		return FALSE;

	return TRUE;
}

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

QWORD CMapViewFileMultiPckRead::GetFileSize()
{
	return m_uqwPckStructSize.qwValue;
}