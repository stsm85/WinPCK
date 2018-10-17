#include "MapViewFileMulti.h"
#include "CharsCodeConv.h"

CMapViewFileMultiWrite::CMapViewFileMultiWrite()
{}

CMapViewFileMultiWrite::~CMapViewFileMultiWrite()
{}


BOOL CMapViewFileMultiWrite::AddFile(CMapViewFileWrite *lpWrite, QWORD qwMaxSize, LPCWSTR lpszFilename)
{
	size_t nCellCount = m_file_cell.size();

	FILE_CELL cFileCell = { 0 };

	wcscpy_s(cFileCell.szFilename, lpszFilename);

	cFileCell.lpMapView = lpWrite;
	cFileCell.qwCellSize = lpWrite->GetFileSize();
#if 0
	if((0 != cFileCell.qwCellSize) && (0 != nCellCount)){

		LPFILE_CELL lpFileCell = &m_file_cell.at(nCellCount - 1);
		lpFileCell->qwMaxCellSize = lpFileCell->qwCellSize;
		lpFileCell->qwCellAddressEnd = lpFileCell->qwCellAddressBegin + lpFileCell->qwCellSize;
		m_uqwMaxSize.qwValue = m_uqwFullSize.qwValue;
	}
#endif
	cFileCell.qwCellAddressBegin = m_uqwMaxSize.qwValue;
	cFileCell.qwCellAddressEnd = m_uqwMaxSize.qwValue + qwMaxSize;
	cFileCell.qwMaxCellSize = qwMaxSize;

	m_file_cell.push_back(cFileCell);

	m_uqwFullSize.qwValue += cFileCell.qwCellSize;
	m_uqwMaxSize.qwValue += qwMaxSize;

	return TRUE;
}

BOOL CMapViewFileMultiWrite::AddFile(LPCSTR lpszFilename, DWORD dwCreationDisposition, QWORD qwMaxSize)
{
	CMapViewFileWrite *lpWrite = new CMapViewFileWrite();

	if(!lpWrite->Open(lpszFilename, dwCreationDisposition)) {

		delete lpWrite;
		return FALSE;
	}
	CAnsi2Ucs cA2U(CP_ACP);

	return AddFile(lpWrite, qwMaxSize, cA2U.GetString(lpszFilename));
}

BOOL CMapViewFileMultiWrite::AddFile(LPCWSTR lpszFilename, DWORD dwCreationDisposition, QWORD qwMaxSize)
{
	CMapViewFileWrite *lpWrite = new CMapViewFileWrite();

	if(!lpWrite->Open(lpszFilename, dwCreationDisposition)) {

		delete lpWrite;
		return FALSE;
	}
	return AddFile(lpWrite, qwMaxSize, lpszFilename);
}

BOOL CMapViewFileMultiWrite::Mapping(QWORD dwMaxSize)
{
	size_t nCellCount = m_file_cell.size();

	int iCellID = GetCellIDByPoint(dwMaxSize);
	QWORD	qwLastExpandSize = dwMaxSize - m_file_cell[iCellID].qwCellAddressBegin;

	for(int i = 0;i < iCellID;i++) {

		CMapViewFileWrite *lpWrite = (CMapViewFileWrite*)m_file_cell[i].lpMapView;

		if(!lpWrite->Mapping(m_file_cell[i].qwMaxCellSize)) {
			return FALSE;
		}
		m_file_cell[i].qwCellSize = m_file_cell[i].qwMaxCellSize;
	}

	CMapViewFileWrite *lpWrite = (CMapViewFileWrite*)m_file_cell[iCellID].lpMapView;

	if(!lpWrite->Mapping(qwLastExpandSize)) {
		return FALSE;
	}

	m_file_cell[iCellID].qwCellSize = qwLastExpandSize;

	return TRUE;
}

LPBYTE CMapViewFileMultiWrite::View(QWORD dwAddress, DWORD dwSize)
{
	return CMapViewFileMulti::View(dwAddress, dwSize, FALSE);
}

BOOL CMapViewFileMultiWrite::SetEndOfFile()
{
	BOOL rtn = TRUE;
	size_t nCellCount = m_file_cell.size();

	for(int i = 0;i < nCellCount;i++) {
		if(!((CMapViewFileWrite*)m_file_cell[i].lpMapView)->SetEndOfFile()) {
			rtn = FALSE;
		}
		m_file_cell[i].qwCellSize = m_file_cell[i].lpMapView->GetFileSize();
	}
	m_uqwFullSize.qwValue = m_uqwCurrentPos.qwValue;
	return rtn;
}

DWORD CMapViewFileMultiWrite::Write(LPVOID buffer, DWORD dwBytesToWrite)
{
	size_t nCellCount = m_file_cell.size();
	int iCellIDBegin = GetCellIDByPoint(m_uqwCurrentPos.qwValue);
	LPBYTE buffer2write = (LPBYTE)buffer;

	DWORD dwBytesLeft = dwBytesToWrite;
	DWORD dwBytesWriteAll = 0;

	while((0 < dwBytesLeft) && (nCellCount >iCellIDBegin)) {

		DWORD dwBytesWrite = m_file_cell[iCellIDBegin].lpMapView->Read(buffer2write, dwBytesLeft);

		dwBytesLeft -= dwBytesWrite;
		buffer2write += dwBytesWrite;
		dwBytesWriteAll += dwBytesWrite;
	}
	m_uqwCurrentPos.qwValue += dwBytesWriteAll;
	return dwBytesWriteAll;
}