#include "MapViewFileMulti.h"
#include "CharsCodeConv.h"

CMapViewFileMultiRead::CMapViewFileMultiRead()
{}

CMapViewFileMultiRead::~CMapViewFileMultiRead()
{}

BOOL CMapViewFileMultiRead::AddFile(CMapViewFileRead *lpRead, LPCWSTR lpszFilename)
{
	FILE_CELL cFileCell = {0};

	wcscpy_s(cFileCell.szFilename, lpszFilename);
	cFileCell.lpMapView = lpRead;
	cFileCell.qwCellSize = lpRead->GetFileSize();
	cFileCell.qwCellAddressBegin = m_uqwFullSize.qwValue;
	cFileCell.qwCellAddressEnd = m_uqwFullSize.qwValue + cFileCell.qwCellSize;
	cFileCell.qwMaxCellSize = cFileCell.qwCellSize;
	
	m_file_cell.push_back(cFileCell);

	m_uqwFullSize.qwValue += cFileCell.qwCellSize;
	m_uqwMaxSize.qwValue += cFileCell.qwCellSize;

	return TRUE;
}

BOOL CMapViewFileMultiRead::AddFile(LPCSTR lpszFilename)
{
	CMapViewFileRead *lpRead = new CMapViewFileRead();

	if(!lpRead->Open(lpszFilename)) {

		delete lpRead;
		return FALSE;
	}

	CAnsi2Ucs cA2U(CP_ACP);
	return AddFile(lpRead, cA2U.GetString(lpszFilename));
}

BOOL CMapViewFileMultiRead::AddFile(LPCWSTR lpszFilename)
{
	CMapViewFileRead *lpRead = new CMapViewFileRead();

	if(!lpRead->Open(lpszFilename)) {

		delete lpRead;
		return FALSE;
	}
	return AddFile(lpRead, lpszFilename);
}

BOOL CMapViewFileMultiRead::Mapping()
{
	size_t nCellCount = m_file_cell.size();

	for(int i = 0;i < nCellCount;i++) {

		CMapViewFileRead *lpRead = (CMapViewFileRead*)m_file_cell[i].lpMapView;

		if(!lpRead->Mapping()) {
			return FALSE;
		}
	}
	return TRUE;
}

LPBYTE CMapViewFileMultiRead::View(QWORD dwAddress, DWORD dwSize)
{
	return CMapViewFileMulti::View(dwAddress, dwSize, TRUE);
}