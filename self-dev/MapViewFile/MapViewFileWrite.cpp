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
#include "pch_mvf.h"
#include "MapViewFile.h"


CMapViewFileWrite::CMapViewFileWrite()
{}

CMapViewFileWrite::~CMapViewFileWrite() noexcept
{}

BOOL CMapViewFileWrite::Open(const fs::path& lpszFilename, DWORD dwCreationDisposition, BOOL isNTFSSparseFile)
{
	if (CMapViewFile::Open(std::ref(lpszFilename), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, dwCreationDisposition, isNTFSSparseFile ? (FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_SPARSE_FILE) : FILE_ATTRIBUTE_NORMAL)) {

		if (isNTFSSparseFile)
			this->SetSparseFile();
		return TRUE;
	}
	return FALSE;
}
#if 0
BOOL CMapViewFileWrite::Open(LPCWSTR lpszFilename, DWORD dwCreationDisposition, BOOL isNTFSSparseFile)
{
	if (CMapViewFile::Open(lpszFilename, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, dwCreationDisposition, isNTFSSparseFile ? (FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_SPARSE_FILE) : FILE_ATTRIBUTE_NORMAL)) {

		if (isNTFSSparseFile)
			SetSparseFile();
		return TRUE;
	}
	return FALSE;
}
#endif

BOOL CMapViewFileWrite::Mapping(QWORD qwMaxSize)
{

	UNQWORD uqwMaxSize;
	uqwMaxSize.qwValue = qwMaxSize;

	if(NULL == (this->hFileMapping = ::CreateFileMappingA(this->hFile, NULL, PAGE_READWRITE, uqwMaxSize.dwValueHigh, uqwMaxSize.dwValue, GenerateMapName()))) {
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
	return this->ViewReal(qdwAddress, dwSize, FILE_MAP_WRITE);
}


LPBYTE CMapViewFileWrite::ReView(LPVOID lpMapAddressOld, QWORD dwAddress, DWORD dwSize)
{
	this->UnmapView(lpMapAddressOld);
	return this->View(dwAddress, dwSize);
}


BOOL CMapViewFileWrite::OpenMappingWrite(fs::path lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap)
{
	if(!this->Open(std::ref(lpFileName), dwCreationDisposition))
		return FALSE;

	if(!this->Mapping(qdwSizeToMap))
		return FALSE;

	return TRUE;
}

#if 0
BOOL CMapViewFileWrite::OpenMappingWrite(LPCWSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap)
{
	if(!Open(lpFileName, dwCreationDisposition))
		return FALSE;

	if(!Mapping(qdwSizeToMap))
		return FALSE;

	return TRUE;
}
#endif

BOOL CMapViewFileWrite::SetEndOfFile()
{
	this->UnmapViewAll();
	this->UnMaping();
	return ::SetEndOfFile(this->hFile);
}

DWORD CMapViewFileWrite::Write(LPVOID buffer, DWORD dwBytesToWrite)
{
	DWORD	dwFileBytesWrote = 0;

	if(!::WriteFile(this->hFile, buffer, dwBytesToWrite, &dwFileBytesWrote, NULL)) {
		return 0;
	}

	return dwFileBytesWrote;
}

BOOL CMapViewFileWrite::FlushFileBuffers()
{
	return ::FlushFileBuffers(this->hFile);
}

void CMapViewFileWrite::SetSparseFile()
{
	DWORD dw;
	::DeviceIoControl(this->hFile, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &dw, NULL);
}

////使用MapViewOfFile进行写操作
//BOOL CMapViewFileWrite::Write2(QWORD dwAddress, LPVOID buffer, DWORD dwBytesToWrite)
//{
//	return 1;
//}

