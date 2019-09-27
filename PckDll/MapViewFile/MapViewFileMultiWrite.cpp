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

BOOL CMapViewFileMultiWrite::AddFile(LPCSTR lpszFilename, DWORD dwCreationDisposition, QWORD qwMaxSize, BOOL isNTFSSparseFile)
{
	CMapViewFileWrite *lpWrite = new CMapViewFileWrite();

	if(!lpWrite->Open(lpszFilename, dwCreationDisposition, isNTFSSparseFile)) {

		delete lpWrite;
		return FALSE;
	}
	CAnsi2Ucs cA2U(CP_ACP);

	return AddFile(lpWrite, qwMaxSize, cA2U.GetString(lpszFilename));
}

BOOL CMapViewFileMultiWrite::AddFile(LPCWSTR lpszFilename, DWORD dwCreationDisposition, QWORD qwMaxSize, BOOL isNTFSSparseFile)
{
	CMapViewFileWrite *lpWrite = new CMapViewFileWrite();

	if(!lpWrite->Open(lpszFilename, dwCreationDisposition, isNTFSSparseFile)) {

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

	if (0 != qwLastExpandSize) {
		CMapViewFileWrite *lpWrite = (CMapViewFileWrite*)m_file_cell[iCellID].lpMapView;

		if (!lpWrite->Mapping(qwLastExpandSize)) {
			return FALSE;
		}

		m_file_cell[iCellID].qwCellSize = qwLastExpandSize;

	}

	if(dwMaxSize > m_uqwFullSize.qwValue)
		m_uqwFullSize.qwValue = dwMaxSize;

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

		DWORD dwBytesWrite = ((CMapViewFileWrite*)m_file_cell[iCellIDBegin].lpMapView)->Write(buffer2write, dwBytesLeft);

		dwBytesLeft -= dwBytesWrite;
		buffer2write += dwBytesWrite;
		dwBytesWriteAll += dwBytesWrite;
	}
	m_uqwCurrentPos.qwValue += dwBytesWriteAll;
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

	QWORD qwOldFileSize = m_uqwFullSize.qwValue;

	if ((dwWritingAddressPointer + dwFileSizeToWrite + PCK_SPACE_DETECT_SIZE) > qwOldFileSize) {

		//打印日志
		//CPckClassLog			m_PckLogFD;

		QWORD qwSizeToExpand = ((dwFileSizeToWrite + PCK_SPACE_DETECT_SIZE) > PCK_STEP_ADD_SIZE ? (dwFileSizeToWrite + PCK_SPACE_DETECT_SIZE) : PCK_STEP_ADD_SIZE);
		ULARGE_INTEGER lpfree;

		if (GetDiskFreeSpaceExA(GetFileDiskName(), NULL, NULL, &lpfree)) {

			qwSizeToExpand = GetExpanedPckFilesize(lpfree.QuadPart, qwSizeToExpand, 0);
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

		UnmapViewAll();
		UnMaping();

		if (!Mapping(qwNewExpectedFileSize)) {

			//m_PckLogFD.PrintLogW(TEXT_VIEWMAP_FAIL);
			//SetErrMsgFlag(PCK_ERR_VIEWMAP_FAIL);
			Mapping(qwOldFileSize);
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
	int iCellIDBegin = GetCellIDByPoint(dwAddress);
	int iCellIDEnd = GetCellIDByPoint(dwAddressEndAt);

	if ((-1 == iCellIDBegin) || (-1 == iCellIDEnd))
		return FALSE;

	QWORD	dwRealAddress = dwAddress - m_file_cell[iCellIDBegin].qwCellAddressBegin;

	if (iCellIDBegin == iCellIDEnd) {

		if (NULL != (pViewAddress = m_file_cell[iCellIDBegin].lpMapView->View(dwRealAddress, dwSize))) {

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
		size_t sizeBegin = m_file_cell[iCellIDBegin].qwCellAddressEnd - dwAddress;
		size_t sizeEnd = dwAddress + dwSize - m_file_cell[iCellIDEnd].qwCellAddressBegin;

		const BYTE* lpBuffer = (LPBYTE)buffer;

		if (NULL == (pViewAddress = m_file_cell[iCellIDBegin].lpMapView->View(dwRealAddress, sizeBegin))) 
			return FALSE;
		

		memcpy(pViewAddress, lpBuffer, sizeBegin);
		lpBuffer += sizeBegin;
		
		if (0 < iCellIDMidCount) {
			for (int i = (iCellIDBegin + 1); i < iCellIDEnd; i++) {

				if (NULL == (pViewAddress = m_file_cell[i].lpMapView->View(0, m_file_cell[i].qwCellSize))) 
					return FALSE;
				
				memcpy(pViewAddress, lpBuffer, m_file_cell[i].qwCellSize);
				lpBuffer += m_file_cell[i].qwCellSize;

			}
		}

		if (NULL == (pViewAddress = m_file_cell[iCellIDEnd].lpMapView->View(0, sizeEnd))) 
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
	if (!IsNeedExpandWritingFile(dwAddress, dwBytesToWrite)) {
		return FALSE;
	}

	if (!ViewAndWrite2(dwAddress, buffer, dwBytesToWrite)) {
		return FALSE;
	}

	nBytesWriten += dwBytesToWrite;
	if (FLUSH_SIZE_THRESHOLD < nBytesWriten)
	{
		for (int i = 0; i < m_file_cell.size(); i++) {
			m_file_cell[i].lpMapView->FlushFileBuffers();
		}
		
		nBytesWriten = 0;
	}

	UnmapViewAll();

	return TRUE;
}
