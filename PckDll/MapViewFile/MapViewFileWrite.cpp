//////////////////////////////////////////////////////////////////////
// MapViewFileWrite.cpp: 用于映射文件视图（写）
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2014.4.24
//////////////////////////////////////////////////////////////////////

#include "MapViewFile.h"


CMapViewFileWrite::CMapViewFileWrite()
{}

CMapViewFileWrite::~CMapViewFileWrite()
{}

BOOL CMapViewFileWrite::Open(LPCSTR lpszFilename, DWORD dwCreationDisposition, BOOL isNTFSSparseFile)
{
	if (CMapViewFile::Open(lpszFilename, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, dwCreationDisposition, isNTFSSparseFile ? (FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_SPARSE_FILE) : FILE_ATTRIBUTE_NORMAL)) {

		if (isNTFSSparseFile)
			SetSparseFile();
		return TRUE;
	}
	return FALSE;
}

BOOL CMapViewFileWrite::Open(LPCWSTR lpszFilename, DWORD dwCreationDisposition, BOOL isNTFSSparseFile)
{
	if (CMapViewFile::Open(lpszFilename, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, dwCreationDisposition, isNTFSSparseFile ? (FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_SPARSE_FILE) : FILE_ATTRIBUTE_NORMAL)) {

		if (isNTFSSparseFile)
			SetSparseFile();
		return TRUE;
	}
	return FALSE;
}

BOOL CMapViewFileWrite::Mapping(QWORD qwMaxSize)
{

	UNQWORD uqwMaxSize;
	uqwMaxSize.qwValue = qwMaxSize;

	if(NULL == (hFileMapping = CreateFileMappingA(hFile, NULL, PAGE_READWRITE, uqwMaxSize.dwValueHigh, uqwMaxSize.dwValue, GenerateMapName()))) {
#ifdef _DEBUG
		if(0 != qwMaxSize)
			assert(FALSE);
#endif
		return FALSE;
	}
	return TRUE;
}

LPBYTE CMapViewFileWrite::View(QWORD qdwAddress, DWORD dwSize)
{
	return ViewReal(qdwAddress, dwSize, FILE_MAP_WRITE);
}


LPBYTE CMapViewFileWrite::ReView(LPVOID lpMapAddressOld, QWORD dwAddress, DWORD dwSize)
{
	UnmapView(lpMapAddressOld);
	return View(dwAddress, dwSize);
}


BOOL CMapViewFileWrite::OpenMappingWrite(LPCSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap)
{
	if(!Open(lpFileName, dwCreationDisposition))
		return FALSE;

	if(!Mapping(qdwSizeToMap))
		return FALSE;

	return TRUE;
}

BOOL CMapViewFileWrite::OpenMappingWrite(LPCWSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap)
{
	if(!Open(lpFileName, dwCreationDisposition))
		return FALSE;

	if(!Mapping(qdwSizeToMap))
		return FALSE;

	return TRUE;
}

BOOL CMapViewFileWrite::SetEndOfFile()
{
	UnmapViewAll();
	UnMaping();
	return ::SetEndOfFile(hFile);
}

DWORD CMapViewFileWrite::Write(LPVOID buffer, DWORD dwBytesToWrite)
{
	DWORD	dwFileBytesWrote = 0;

	if(!WriteFile(hFile, buffer, dwBytesToWrite, &dwFileBytesWrote, NULL)) {
		return 0;
	}

	return dwFileBytesWrote;
}

BOOL CMapViewFileWrite::FlushFileBuffers()
{
	return ::FlushFileBuffers(hFile);
}

void CMapViewFileWrite::SetSparseFile()
{
	DWORD dw;
	DeviceIoControl(hFile, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &dw, NULL);
}

////使用MapViewOfFile进行写操作
//BOOL CMapViewFileWrite::Write2(QWORD dwAddress, LPVOID buffer, DWORD dwBytesToWrite)
//{
//	return 1;
//}

