#include "pch_mvf.h"
#include "MapViewFileMulti.h"

CMapViewFileMulti::CMapViewFileMulti()
{
	this->m_cross_view.clear();
}

CMapViewFileMulti::~CMapViewFileMulti() noexcept
{
	this->clear();
}

int CMapViewFileMulti::GetCellIDByPoint(QWORD qwPoint)
{
	size_t nCellCount = this->m_file_cell.size();
	if(qwPoint >= this->m_uqwMaxSize.qwValue)
		return (nCellCount - 1);

	for(int i = 0; i < nCellCount;i++) {

		if((qwPoint >= this->m_file_cell[i].qwCellAddressBegin) && (qwPoint < this->m_file_cell[i].qwCellAddressEnd)) {
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
		this->m_uqwCurrentPos.llwValue = lDistanceToMove;
		break;

	case FILE_CURRENT:
		this->m_uqwCurrentPos.llwValue += lDistanceToMove;
		break;

	case FILE_END:
		this->m_uqwCurrentPos.llwValue = this->m_uqwFullSize.llwValue + lDistanceToMove;
		break;
	}

	size_t nCellCount = this->m_file_cell.size();
	int iCellAt = this->GetCellIDByPoint(this->m_uqwCurrentPos.qwValue);
	if(-1 == iCellAt) {
		assert(FALSE);
	}

	for(int i = 0;i < iCellAt;i++) {
		this->m_file_cell[i].lpMapView->SetFilePointer(0, FILE_END);
	}

	uqdwSetRealPointerAt.qwValue = this->m_uqwCurrentPos.qwValue - this->m_file_cell[iCellAt].qwCellAddressBegin;
	this->m_file_cell[iCellAt].lpMapView->SetFilePointer(uqdwSetRealPointerAt.qwValue);

	for(int i = (iCellAt + 1);i < nCellCount;i++) {
		this->m_file_cell[i].lpMapView->SetFilePointer(0);
	}

}

QWORD CMapViewFileMulti::GetFileSize() const
{
	return this->m_uqwFullSize.qwValue;
}


void CMapViewFileMulti::UnmapViewAll()
{
	std::vector<void*> buffer2free;
	uint8_t* lastBuffer = NULL;

	size_t cross_view_count = this->m_cross_view.size();
	for(int i = 0;i < cross_view_count;i++) {

		if(NULL != this->m_cross_view[i].lpBufferTargetPtr)
			memcpy(this->m_cross_view[i].lpMapAddress, this->m_cross_view[i].lpBufferTargetPtr, this->m_cross_view[i].size);

		if(lastBuffer != this->m_cross_view[i].lpBufferTarget) {
			lastBuffer = this->m_cross_view[i].lpBufferTarget;
			buffer2free.push_back(lastBuffer);
		}
	}
	this->m_cross_view.clear();

	size_t buffer2free_count = buffer2free.size();
	for(int i = 0;i < buffer2free_count;i++) {
		free(buffer2free[i]);
	}

	size_t nCellCount = this->m_file_cell.size();

	for(int i = 0;i < nCellCount;i++) {
		this->m_file_cell[i].lpMapView->UnmapViewAll();
	}
}

void CMapViewFileMulti::UnMaping()
{
	size_t nCellCount = this->m_file_cell.size();

	for(int i = 0;i < nCellCount;i++) {
		this->m_file_cell[i].lpMapView->UnMaping();
	}
}

void CMapViewFileMulti::clear()
{
	this->UnmapViewAll();

	size_t nCellCount = this->m_file_cell.size();

	for(int i = 0;i < nCellCount;i++) {
		delete this->m_file_cell[i].lpMapView;
		if(0 == this->m_file_cell[i].qwCellSize)
			fs::remove(this->m_file_cell[i].szFilename);
	}

	this->m_file_cell.clear();

	this->m_uqwCurrentPos.qwValue = 0;
	this->m_uqwFullSize.qwValue = 0;
}

LPBYTE CMapViewFileMulti::View(QWORD dwAddress, DWORD dwSize, BOOL isReadOnly)
{
	//assert(0 != dwSize);
	if (0 == dwSize)
		return nullptr;

	QWORD	dwAddressEndAt = dwAddress + dwSize;
	int iCellIDBegin = this->GetCellIDByPoint(dwAddress);
	int iCellIDEnd = this->GetCellIDByPoint(dwAddressEndAt);

	if((-1 == iCellIDBegin) || (-1 == iCellIDEnd))
		return nullptr;

	QWORD	dwRealAddress = dwAddress - this->m_file_cell[iCellIDBegin].qwCellAddressBegin;

	if(iCellIDBegin == iCellIDEnd) {

		return this->m_file_cell[iCellIDBegin].lpMapView->View(dwRealAddress, dwSize);
	} else {

		//iCellIDBegin iCellIDMid iCellIDMid iCellIDEnd
		int iCellIDMidCount = iCellIDEnd - iCellIDBegin - 1;
		if(0 > iCellIDMidCount)
			return nullptr;

		//计算一下各个文件需要的大小
		size_t sizeBegin = this->m_file_cell[iCellIDBegin].qwCellAddressEnd - dwAddress;
		size_t sizeEnd = dwAddress + dwSize - this->m_file_cell[iCellIDEnd].qwCellAddressBegin;

		//申请内存
		LPBYTE lpCrossBuffer = nullptr, lpCrossBufferPtr;
		if(nullptr == (lpCrossBuffer = (LPBYTE)malloc(dwSize))) {
			return nullptr;
		}

		lpCrossBufferPtr = lpCrossBuffer;

		if(!this->BuildCrossViewBuffer(lpCrossBuffer, lpCrossBufferPtr, iCellIDBegin, dwRealAddress, sizeBegin, isReadOnly)) {
			free(lpCrossBuffer);
			return nullptr;
		}

		if(0 < iCellIDMidCount) {
			for(int i = (iCellIDBegin + 1);i < iCellIDEnd;i++) {

				if(!this->BuildCrossViewBuffer(lpCrossBuffer, lpCrossBufferPtr, i, 0, this->m_file_cell[i].qwCellSize, isReadOnly)) {
					free(lpCrossBuffer);
					return nullptr;
				}
			}
		}

		if(!this->BuildCrossViewBuffer(lpCrossBuffer, lpCrossBufferPtr, iCellIDEnd, 0, sizeEnd, isReadOnly)) {
			free(lpCrossBuffer);
			return nullptr;
		}

		assert((lpCrossBuffer + dwSize) == lpCrossBufferPtr);

		return lpCrossBuffer;
	}
}

BOOL CMapViewFileMulti::BuildCrossViewBuffer(LPBYTE lpCrossBuffer, LPBYTE &lpCrossBufferPtr, int cell_id, QWORD qwAddress, DWORD dwSize, BOOL isReadOnly)
{
	CROSS_VIEW cCrossView = { 0 };

	LPBYTE lpMapAddressCell = this->m_file_cell[cell_id].lpMapView->View(qwAddress, dwSize);

	if(nullptr == lpMapAddressCell) {
		return FALSE;
	}

	memcpy(lpCrossBufferPtr, lpMapAddressCell, dwSize);
	
	if(isReadOnly) {
		this->m_file_cell[cell_id].lpMapView->UnmapView(lpMapAddressCell);

		cCrossView.lpMapAddress = lpMapAddressCell;
		this->m_cross_view.push_back(cCrossView);

	} else {
		cCrossView.iCellID = cell_id;
		cCrossView.lpBufferTarget = lpCrossBuffer;
		cCrossView.lpBufferTargetPtr = lpCrossBufferPtr;
		cCrossView.size = dwSize;
		cCrossView.lpMapAddress = lpMapAddressCell;
		this->m_cross_view.push_back(cCrossView);
	}


	lpCrossBufferPtr += dwSize;

	return TRUE;
}

DWORD CMapViewFileMulti::Read(LPVOID buffer, DWORD dwBytesToRead)
{
	size_t nCellCount = this->m_file_cell.size();
	int iCellIDBegin = this->GetCellIDByPoint(this->m_uqwCurrentPos.qwValue);
	auto buffer2read = (LPBYTE)buffer;

	int dwBytesLeft = dwBytesToRead;
	DWORD dwBytesReadAll = 0;

	while((0 < dwBytesLeft) && (nCellCount >iCellIDBegin)){

		auto dwBytesRead = this->m_file_cell[iCellIDBegin].lpMapView->Read(buffer2read, dwBytesLeft);

		if (0 == dwBytesRead)
			throw std::runtime_error("read file fail");

		dwBytesLeft -= dwBytesRead;
		buffer2read += dwBytesRead;
		dwBytesReadAll += dwBytesRead;
	}
	this->m_uqwCurrentPos.qwValue += dwBytesReadAll;
	return dwBytesReadAll;
}

const char*	CMapViewFileMulti::GetFileDiskName() const
{
	return this->m_file_cell[0].lpMapView->GetFileDiskName();
}

DWORD	CMapViewFileMulti::GetCellCount() const
{
	return this->m_file_cell.size();
}

DWORD	CMapViewFileMulti::GetCellSize() const
{
	if (1 < this->m_file_cell.size()) {
		return this->m_file_cell[0].qwMaxCellSize;
	}
	else {
		return 0x7fffff00;
	}
}