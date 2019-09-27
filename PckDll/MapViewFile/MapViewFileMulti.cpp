#include "MapViewFileMulti.h"

CMapViewFileMulti::CMapViewFileMulti():
	m_uqwCurrentPos{ 0 },
	m_uqwFullSize{ 0 },
	m_uqwMaxSize{ 0 }
{
	m_cross_view.clear();
}

CMapViewFileMulti::~CMapViewFileMulti() throw()
{
	clear();
}

int CMapViewFileMulti::GetCellIDByPoint(QWORD qwPoint)
{
	size_t nCellCount = m_file_cell.size();
	if(qwPoint >= m_uqwMaxSize.qwValue)
		return (nCellCount - 1);

	for(int i = 0;i < nCellCount;i++) {

		if((qwPoint >= m_file_cell[i].qwCellAddressBegin) && (qwPoint < m_file_cell[i].qwCellAddressEnd)) {
			return i;
		}
	}
	assert(FALSE);
	return -1;
}

void CMapViewFileMulti::SetFilePointer(LONGLONG lDistanceToMove, DWORD dwMoveMethod)
{
	UNQWORD	uqdwSetRealPointerAt;

	switch(dwMoveMethod) {
	case FILE_BEGIN:
		m_uqwCurrentPos.llwValue = lDistanceToMove;
		break;

	case FILE_CURRENT:
		m_uqwCurrentPos.llwValue += lDistanceToMove;
		break;

	case FILE_END:
		m_uqwCurrentPos.llwValue = m_uqwFullSize.llwValue + lDistanceToMove;
		break;
	}

	size_t nCellCount = m_file_cell.size();
	int iCellAt = GetCellIDByPoint(m_uqwCurrentPos.qwValue);
	if(-1 == iCellAt) {
		assert(FALSE);
	}

	for(int i = 0;i < iCellAt;i++) {
		m_file_cell[i].lpMapView->SetFilePointer(0, FILE_END);
	}

	uqdwSetRealPointerAt.qwValue = m_uqwCurrentPos.qwValue - m_file_cell[iCellAt].qwCellAddressBegin;
	m_file_cell[iCellAt].lpMapView->SetFilePointer(uqdwSetRealPointerAt.qwValue);

	for(int i = (iCellAt + 1);i < nCellCount;i++) {
		m_file_cell[i].lpMapView->SetFilePointer(0);
	}

}

QWORD CMapViewFileMulti::GetFileSize()
{
	return m_uqwFullSize.qwValue;
}

//void CMapViewFileMulti::UnmapView(LPVOID lpTargetAddress)
//{
//	BOOL isCrossBuffer = FALSE;
//	LPBYTE lastBuffer = NULL;
//
//	size_t cross_view_count = m_cross_view.size();
//	for(int i = 0;i < cross_view_count;i++) {
//		if(lastBuffer != m_cross_view[i].lpBufferTarget) {
//			free(m_cross_view[i].lpBufferTarget);
//			lastBuffer = m_cross_view[i].lpBufferTarget;
//		}
//	}
//
//
//}

void CMapViewFileMulti::UnmapViewAll()
{
	std::vector<void*> buffer2free;
	uint8_t* lastBuffer = NULL;

	size_t cross_view_count = m_cross_view.size();
	for(int i = 0;i < cross_view_count;i++) {

		if(NULL != m_cross_view[i].lpBufferTargetPtr)
			memcpy(m_cross_view[i].lpMapAddress, m_cross_view[i].lpBufferTargetPtr, m_cross_view[i].size);

		if(lastBuffer != m_cross_view[i].lpBufferTarget) {
			lastBuffer = m_cross_view[i].lpBufferTarget;
			buffer2free.push_back(lastBuffer);
		}
	}
	m_cross_view.clear();

	size_t buffer2free_count = buffer2free.size();
	for(int i = 0;i < buffer2free_count;i++) {
		free(buffer2free[i]);
	}

	size_t nCellCount = m_file_cell.size();

	for(int i = 0;i < nCellCount;i++) {
		m_file_cell[i].lpMapView->UnmapViewAll();
	}
}

void CMapViewFileMulti::UnMaping()
{
	size_t nCellCount = m_file_cell.size();

	for(int i = 0;i < nCellCount;i++) {
		m_file_cell[i].lpMapView->UnMaping();
	}
}

void CMapViewFileMulti::clear()
{
	UnmapViewAll();

	size_t nCellCount = m_file_cell.size();

	for(int i = 0;i < nCellCount;i++) {
		delete m_file_cell[i].lpMapView;
		if(0 == m_file_cell[i].qwCellSize)
			DeleteFileW(m_file_cell[i].szFilename);
	}

	m_file_cell.clear();

	m_uqwCurrentPos.qwValue = 0;
	m_uqwFullSize.qwValue = 0;
}

LPBYTE CMapViewFileMulti::View(QWORD dwAddress, DWORD dwSize, BOOL isReadOnly)
{
	//assert(0 != dwSize);
	if (0 == dwSize)
		return NULL;

	QWORD	dwAddressEndAt = dwAddress + dwSize;
	int iCellIDBegin = GetCellIDByPoint(dwAddress);
	int iCellIDEnd = GetCellIDByPoint(dwAddressEndAt);

	if((-1 == iCellIDBegin) || (-1 == iCellIDEnd))
		return NULL;

	QWORD	dwRealAddress = dwAddress - m_file_cell[iCellIDBegin].qwCellAddressBegin;

	if(iCellIDBegin == iCellIDEnd) {

		return m_file_cell[iCellIDBegin].lpMapView->View(dwRealAddress, dwSize);
	} else {

		//iCellIDBegin iCellIDMid iCellIDMid iCellIDEnd
		int iCellIDMidCount = iCellIDEnd - iCellIDBegin - 1;
		if(0 > iCellIDMidCount)
			return NULL;

		//计算一下各个文件需要的大小
		size_t sizeBegin = m_file_cell[iCellIDBegin].qwCellAddressEnd - dwAddress;
		size_t sizeEnd = dwAddress + dwSize - m_file_cell[iCellIDEnd].qwCellAddressBegin;

		//申请内存
		LPBYTE lpCrossBuffer = NULL, lpCrossBufferPtr;
		if(NULL == (lpCrossBuffer = (LPBYTE)malloc(dwSize))) {
			return NULL;
		}

		lpCrossBufferPtr = lpCrossBuffer;

		if(!BuildCrossViewBuffer(lpCrossBuffer, lpCrossBufferPtr, iCellIDBegin, dwRealAddress, sizeBegin, isReadOnly)) {
			free(lpCrossBuffer);
			return NULL;
		}

		if(0 < iCellIDMidCount) {
			for(int i = (iCellIDBegin + 1);i < iCellIDEnd;i++) {

				if(!BuildCrossViewBuffer(lpCrossBuffer, lpCrossBufferPtr, i, 0, m_file_cell[i].qwCellSize, isReadOnly)) {
					free(lpCrossBuffer);
					return NULL;
				}
			}
		}

		if(!BuildCrossViewBuffer(lpCrossBuffer, lpCrossBufferPtr, iCellIDEnd, 0, sizeEnd, isReadOnly)) {
			free(lpCrossBuffer);
			return NULL;
		}

		assert((lpCrossBuffer + dwSize) == lpCrossBufferPtr);

		return lpCrossBuffer;
	}
}

BOOL CMapViewFileMulti::BuildCrossViewBuffer(LPBYTE lpCrossBuffer, LPBYTE &lpCrossBufferPtr, int cell_id, QWORD qwAddress, DWORD dwSize, BOOL isReadOnly)
{
	CROSS_VIEW cCrossView = { 0 };

	LPBYTE lpMapAddressCell = m_file_cell[cell_id].lpMapView->View(qwAddress, dwSize);

	if(NULL == lpMapAddressCell) {
		return FALSE;
	}

	memcpy(lpCrossBufferPtr, lpMapAddressCell, dwSize);
	
	if(isReadOnly) {
		m_file_cell[cell_id].lpMapView->UnmapView(lpMapAddressCell);

		cCrossView.lpMapAddress = lpMapAddressCell;
		m_cross_view.push_back(cCrossView);

	} else {
		cCrossView.iCellID = cell_id;
		cCrossView.lpBufferTarget = lpCrossBuffer;
		cCrossView.lpBufferTargetPtr = lpCrossBufferPtr;
		cCrossView.size = dwSize;
		cCrossView.lpMapAddress = lpMapAddressCell;
		m_cross_view.push_back(cCrossView);
	}


	lpCrossBufferPtr += dwSize;

	return TRUE;
}

DWORD CMapViewFileMulti::Read(LPVOID buffer, DWORD dwBytesToRead)
{
	size_t nCellCount = m_file_cell.size();
	int iCellIDBegin = GetCellIDByPoint(m_uqwCurrentPos.qwValue);
	LPBYTE buffer2read = (LPBYTE)buffer;

	int dwBytesLeft = dwBytesToRead;
	DWORD dwBytesReadAll = 0;

	while((0 < dwBytesLeft) && (nCellCount >iCellIDBegin)){

		DWORD dwBytesRead = m_file_cell[iCellIDBegin].lpMapView->Read(buffer2read, dwBytesLeft);

		if (0 == dwBytesRead)
			throw MyException("read fail");

		dwBytesLeft -= dwBytesRead;
		buffer2read += dwBytesRead;
		dwBytesReadAll += dwBytesRead;
	}
	m_uqwCurrentPos.qwValue += dwBytesReadAll;
	return dwBytesReadAll;
}

const char*	CMapViewFileMulti::GetFileDiskName()
{
	return m_file_cell[0].lpMapView->GetFileDiskName();
}

DWORD	CMapViewFileMulti::GetCellCount()
{
	return m_file_cell.size();
}

DWORD	CMapViewFileMulti::GetCellSize()
{
	if (1 < m_file_cell.size()) {
		return m_file_cell[0].qwMaxCellSize;
	}
	else {
		return 0x7fffff00;
	}
}