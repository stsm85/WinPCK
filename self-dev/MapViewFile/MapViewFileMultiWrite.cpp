#include "pch_mvf.h"
#include "MapViewFileMulti.h"

CMapViewFileMultiWrite::CMapViewFileMultiWrite()
{}

CMapViewFileMultiWrite::~CMapViewFileMultiWrite() noexcept
{}


BOOL CMapViewFileMultiWrite::AddFile(CMapViewFileWrite *lpWrite, QWORD qwMaxSize, fs::path& lpszFilename)
{
	size_t nCellCount = this->m_file_cell.size();

	FILE_CELL cFileCell = { 0 };

	//wcscpy_s(cFileCell.szFilename, lpszFilename);
	cFileCell.szFilename = std::move(lpszFilename);
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
	cFileCell.qwCellAddressBegin = this->m_uqwMaxSize.qwValue;
	cFileCell.qwCellAddressEnd = this->m_uqwMaxSize.qwValue + qwMaxSize;
	cFileCell.qwMaxCellSize = qwMaxSize;

	this->m_file_cell.push_back(cFileCell);

	this->m_uqwFullSize.qwValue += cFileCell.qwCellSize;
	this->m_uqwMaxSize.qwValue += qwMaxSize;

	return TRUE;
}

BOOL CMapViewFileMultiWrite::AddFile(fs::path lpszFilename, DWORD dwCreationDisposition, QWORD qwMaxSize, BOOL isNTFSSparseFile)
{
	CMapViewFileWrite *lpWrite = new CMapViewFileWrite();

	if(!lpWrite->Open(std::ref(lpszFilename), dwCreationDisposition, isNTFSSparseFile)) {

		delete lpWrite;
		return FALSE;
	}
	return this->AddFile(lpWrite, qwMaxSize, std::ref(lpszFilename));
}

#if 0
BOOL CMapViewFileMultiWrite::AddFile(LPCWSTR lpszFilename, DWORD dwCreationDisposition, QWORD qwMaxSize, BOOL isNTFSSparseFile)
{
	CMapViewFileWrite *lpWrite = new CMapViewFileWrite();

	if(!lpWrite->Open(lpszFilename, dwCreationDisposition, isNTFSSparseFile)) {

		delete lpWrite;
		return FALSE;
	}
	return AddFile(lpWrite, qwMaxSize, lpszFilename);
}
#endif

BOOL CMapViewFileMultiWrite::Mapping(QWORD dwMaxSize)
{
	size_t nCellCount = this->m_file_cell.size();

	int iCellID = GetCellIDByPoint(dwMaxSize);
	QWORD	qwLastExpandSize = dwMaxSize - this->m_file_cell[iCellID].qwCellAddressBegin;

	for(int i = 0;i < iCellID;i++) {

		CMapViewFileWrite *lpWrite = (CMapViewFileWrite*)this->m_file_cell[i].lpMapView;

		if(!lpWrite->Mapping(this->m_file_cell[i].qwMaxCellSize)) {
			return FALSE;
		}
		this->m_file_cell[i].qwCellSize = this->m_file_cell[i].qwMaxCellSize;
	}

	if (0 != qwLastExpandSize) {
		CMapViewFileWrite *lpWrite = (CMapViewFileWrite*)this->m_file_cell[iCellID].lpMapView;

		if (!lpWrite->Mapping(qwLastExpandSize)) {
			return FALSE;
		}

		this->m_file_cell[iCellID].qwCellSize = qwLastExpandSize;

	}

	if(dwMaxSize > this->m_uqwFullSize.qwValue)
		this->m_uqwFullSize.qwValue = dwMaxSize;

	return TRUE;
}

LPBYTE CMapViewFileMultiWrite::View(QWORD dwAddress, DWORD dwSize)
{
	return CMapViewFileMulti::View(dwAddress, dwSize, FALSE);
}

BOOL CMapViewFileMultiWrite::SetEndOfFile()
{
	BOOL rtn = TRUE;
	size_t nCellCount = this->m_file_cell.size();

	for(int i = 0;i < nCellCount;i++) {
		if(!((CMapViewFileWrite*)this->m_file_cell[i].lpMapView)->SetEndOfFile()) {
			rtn = FALSE;
		}
		this->m_file_cell[i].qwCellSize = this->m_file_cell[i].lpMapView->GetFileSize();
	}
	this->m_uqwFullSize.qwValue = this->m_uqwCurrentPos.qwValue;
	return rtn;
}

DWORD CMapViewFileMultiWrite::Write(LPVOID buffer, DWORD dwBytesToWrite)
{
	size_t nCellCount = this->m_file_cell.size();
	int iCellIDBegin = GetCellIDByPoint(this->m_uqwCurrentPos.qwValue);
	LPBYTE buffer2write = (LPBYTE)buffer;

	DWORD dwBytesLeft = dwBytesToWrite;
	DWORD dwBytesWriteAll = 0;

	while((0 < dwBytesLeft) && (nCellCount >iCellIDBegin)) {

		DWORD dwBytesWrite = ((CMapViewFileWrite*)this->m_file_cell[iCellIDBegin].lpMapView)->Write(buffer2write, dwBytesLeft);

		dwBytesLeft -= dwBytesWrite;
		buffer2write += dwBytesWrite;
		dwBytesWriteAll += dwBytesWrite;
	}
	this->m_uqwCurrentPos.qwValue += dwBytesWriteAll;
	return dwBytesWriteAll;
}

//创建时，剩余文件的空间量不够时，添加量
#define	PCK_STEP_ADD_SIZE				(64*1024*1024)
//创建时，剩余文件的空间量小于此值时，扩展数据
#define	PCK_SPACE_DETECT_SIZE			(4*1024*1024)
//创建时，剩余文件的空间量小于此值(PCK_SPACE_DETECT_SIZE)时，扩展数据的值
//#define PCK_RENAME_EXPAND_ADD			(16*1024*1024)

//qwCurrentPckFilesize为已经存在的文件大小，qwToAddSpace是需要扩大的大小，返回值为（qwCurrentPckFilesize + 可以再扩大的最大大小）
QWORD CMapViewFileMultiWrite::GetExpanedPckFilesize(QWORD qwDiskFreeSpace, QWORD qwToAddSpace, QWORD qwCurrentPckFilesize)
{
	//计算大概需要多大空间qwTotalFileSize
	QWORD	qwTotalFileSizeTemp = qwToAddSpace;

	//if (-1 != qwDiskFreeSpace) {

		//如果申请的空间小于磁盘剩余空间，则申请文件空间大小等于剩余磁盘空间
		if (qwDiskFreeSpace < qwTotalFileSizeTemp)
			qwTotalFileSizeTemp = qwDiskFreeSpace;
	//}

	return (qwTotalFileSizeTemp + qwCurrentPckFilesize);

}

BOOL CMapViewFileMultiWrite::IsNeedExpandWritingFile(
	QWORD dwWritingAddressPointer,
	QWORD dwFileSizeToWrite)
{
	//判断一下dwAddress的值会不会超过dwTotalFileSizeAfterCompress
	//如果超过，说明文件空间申请的过小，重新申请一下ReCreateFileMapping
	//新文件大小在原来的基础上增加(lpfirstFile->dwFileSize + 1mb) >= 64mb ? (lpfirstFile->dwFileSize + 1mb) :64mb
	//1mb=0x100000
	//64mb=0x4000000

	auto qwOldFileSize = this->m_uqwFullSize.qwValue;

	if ((dwWritingAddressPointer + dwFileSizeToWrite + PCK_SPACE_DETECT_SIZE) > qwOldFileSize) {

		//打印日志
		//CPckClassLog			m_PckLogFD;

		QWORD qwSizeToExpand = ((dwFileSizeToWrite + PCK_SPACE_DETECT_SIZE) > PCK_STEP_ADD_SIZE ? (dwFileSizeToWrite + PCK_SPACE_DETECT_SIZE) : PCK_STEP_ADD_SIZE);
		ULARGE_INTEGER lpfree;

		if (::GetDiskFreeSpaceExA(this->GetFileDiskName(), NULL, NULL, &lpfree)) {

			qwSizeToExpand = this->GetExpanedPckFilesize(lpfree.QuadPart, qwSizeToExpand, 0);
		}
		else {
			//qwSizeToExpand = GetExpanedPckFilesize(-1, qwSizeToExpand, 0);
		}

		if (dwFileSizeToWrite > qwSizeToExpand) {
			//m_PckLogFD.PrintLogW("磁盘空间不足，申请空间：%d，剩余空间：%d", dwFileSizeToWrite, qwSizeToExpand);
			//SetErrMsgFlag(PCK_ERR_DISKFULL);
			return FALSE;
		}

		QWORD qwNewExpectedFileSize = qwOldFileSize + qwSizeToExpand;

		this->UnmapViewAll();
		this->UnMaping();

		if (!this->Mapping(qwNewExpectedFileSize)) {

			//m_PckLogFD.PrintLogW(TEXT_VIEWMAP_FAIL);
			//SetErrMsgFlag(PCK_ERR_VIEWMAP_FAIL);
			this->Mapping(qwOldFileSize);
			return FALSE;
		}
		//dwExpectedTotalCompressedFileSize = qwNewExpectedFileSize;
	}
	return TRUE;
}

BOOL CMapViewFileMultiWrite::ViewAndWrite2(QWORD dwAddress, const void *  buffer, DWORD dwSize)
{
	LPVOID pViewAddress = NULL;

	if (0 == dwSize)
		return TRUE;

	QWORD	dwAddressEndAt = dwAddress + dwSize;
	int iCellIDBegin = this->GetCellIDByPoint(dwAddress);
	int iCellIDEnd = this->GetCellIDByPoint(dwAddressEndAt);

	if ((-1 == iCellIDBegin) || (-1 == iCellIDEnd))
		return FALSE;

	QWORD	dwRealAddress = dwAddress - this->m_file_cell[iCellIDBegin].qwCellAddressBegin;

	if (iCellIDBegin == iCellIDEnd) {

		if (NULL != (pViewAddress = this->m_file_cell[iCellIDBegin].lpMapView->View(dwRealAddress, dwSize))) {

			memcpy(pViewAddress, buffer, dwSize);
			return TRUE;
		}

		return FALSE;
	}
	else {

		//iCellIDBegin iCellIDMid iCellIDMid iCellIDEnd
		int iCellIDMidCount = iCellIDEnd - iCellIDBegin - 1;
		if (0 > iCellIDMidCount)
			return FALSE;

		//计算一下各个文件需要的大小
		size_t sizeBegin = this->m_file_cell[iCellIDBegin].qwCellAddressEnd - dwAddress;
		size_t sizeEnd = dwAddress + dwSize - this->m_file_cell[iCellIDEnd].qwCellAddressBegin;

		const BYTE* lpBuffer = (LPBYTE)buffer;

		if (NULL == (pViewAddress = this->m_file_cell[iCellIDBegin].lpMapView->View(dwRealAddress, sizeBegin)))
			return FALSE;
		

		memcpy(pViewAddress, lpBuffer, sizeBegin);
		lpBuffer += sizeBegin;
		
		if (0 < iCellIDMidCount) {
			for (int i = (iCellIDBegin + 1); i < iCellIDEnd; i++) {

				if (NULL == (pViewAddress = this->m_file_cell[i].lpMapView->View(0, this->m_file_cell[i].qwCellSize)))
					return FALSE;
				
				memcpy(pViewAddress, lpBuffer, this->m_file_cell[i].qwCellSize);
				lpBuffer += this->m_file_cell[i].qwCellSize;

			}
		}

		if (NULL == (pViewAddress = this->m_file_cell[iCellIDEnd].lpMapView->View(0, sizeEnd)))
			return FALSE;
		
		memcpy(pViewAddress, lpBuffer, sizeEnd);
		//lpBuffer += sizeEnd;

		return TRUE;
	}
	return TRUE;
}


//使用MapViewOfFile进行写操作
BOOL CMapViewFileMultiWrite::Write2(QWORD dwAddress, const void* buffer, DWORD dwBytesToWrite)
{
	static int nBytesWriten = 0;
	//PCK_STEP_ADD_SIZE
	if (!this->IsNeedExpandWritingFile(dwAddress, dwBytesToWrite)) {
		return FALSE;
	}

	if (!this->ViewAndWrite2(dwAddress, buffer, dwBytesToWrite)) {
		return FALSE;
	}

	nBytesWriten += dwBytesToWrite;
	if (FLUSH_SIZE_THRESHOLD < nBytesWriten)
	{
		for (int i = 0; i < this->m_file_cell.size(); i++) {
			this->m_file_cell[i].lpMapView->FlushFileBuffers();
		}
		
		nBytesWriten = 0;
	}

	this->UnmapViewAll();

	return TRUE;
}
