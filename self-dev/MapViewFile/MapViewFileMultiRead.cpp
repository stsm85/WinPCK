#include "pch_mvf.h"
#include "MapViewFileMulti.h"

CMapViewFileMultiRead::CMapViewFileMultiRead()
{}

CMapViewFileMultiRead::~CMapViewFileMultiRead() noexcept
{}

BOOL CMapViewFileMultiRead::AddFile(CMapViewFileRead *lpRead, const fs::path& lpszFilename)
{
	FILE_CELL cFileCell = {0};

	//wcscpy_s(cFileCell.szFilename, lpszFilename);
	cFileCell.szFilename = lpszFilename;
	cFileCell.lpMapView = lpRead;
	cFileCell.qwCellSize = lpRead->GetFileSize();
	cFileCell.qwCellAddressBegin = this->m_uqwFullSize.qwValue;
	cFileCell.qwCellAddressEnd = this->m_uqwFullSize.qwValue + cFileCell.qwCellSize;
	cFileCell.qwMaxCellSize = cFileCell.qwCellSize;
	
	this->m_file_cell.push_back(cFileCell);

	this->m_uqwFullSize.qwValue += cFileCell.qwCellSize;
	this->m_uqwMaxSize.qwValue += cFileCell.qwCellSize;

	return TRUE;
}

BOOL CMapViewFileMultiRead::AddFile(const fs::path& lpszFilename)
{
	CMapViewFileRead *lpRead = new CMapViewFileRead();

	if(!lpRead->Open(lpszFilename)) {

		delete lpRead;
		return FALSE;
	}

	return this->AddFile(lpRead, lpszFilename);
}

BOOL CMapViewFileMultiRead::Mapping()
{
	size_t nCellCount = this->m_file_cell.size();

	for(int i = 0;i < nCellCount;i++) {

		CMapViewFileRead *lpRead = (CMapViewFileRead*)this->m_file_cell[i].lpMapView;

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