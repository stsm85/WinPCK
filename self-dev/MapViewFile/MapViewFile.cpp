//////////////////////////////////////////////////////////////////////
// MapViewFile.cpp: 用于映射文件视图
// 
//
// 此程序由 李秋枫/stsm/liqf 编写，部分代码取自FastCopy
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////
#include "pch_mvf.h"
#include "MapViewFile.h"

#if 0
#define PATH_LOCAL_PREFIX			"\\\\?\\"
#define PATH_UNC_PREFIX				"\\\\?\\UNC"

#define PATHW_LOCAL_PREFIX			L"\\\\?\\"
#define PATHW_UNC_PREFIX			L"\\\\?\\UNC"

#define PATH_LOCAL_PREFIX_LEN		(strlen(PATH_LOCAL_PREFIX))
#define PATH_UNC_PREFIX_LEN			(strlen(PATH_UNC_PREFIX))

#endif

CMapViewFile::CMapViewFile()
{
	this->vMapAddress.clear();
}

CMapViewFile::~CMapViewFile() noexcept
{
	this->clear();
}

void CMapViewFile::clear() 
{
	this->UnmapViewAll();

	if(NULL != this->hFileMapping) {
		::CloseHandle(this->hFileMapping);
		this->hFileMapping = NULL;
	}

	if(NULL != this->hFile && INVALID_HANDLE_VALUE != this->hFile) {
		::CloseHandle(this->hFile);
		this->hFile = NULL;
	}

}

void CMapViewFile::GetDiskNameFromFilename(const fs::path& lpszFilename)
{
	this->m_szDisk = lpszFilename.root_path().string();
}

const char * CMapViewFile::GetFileDiskName()
{
	return this->m_szDisk.c_str();
}


/*****************************************************************************
BOOL FileExists(CONST TCHAR szName[MAX_PATH_EX])
szName	: (IN) string

Return Value:
returns TRUE if szName exists and is not a directory; FALSE otherwise
*****************************************************************************/
BOOL CMapViewFile::FileExists(const fs::path& szFileName)
{
	if (!fs::exists(szFileName))
		return FALSE;

	fs::directory_entry dir(szFileName);
	if (dir.status().type() != fs::file_type::directory)
		return TRUE;
	return FALSE;
}

BOOL CMapViewFile::Open(const fs::path& lpszFilename, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes)
{

	fs::path absolute_path = fs::absolute(lpszFilename);
	this->GetDiskNameFromFilename(absolute_path);
	
	if(this->FileExists(absolute_path) ||
		((OPEN_EXISTING != dwCreationDisposition) && (TRUNCATE_EXISTING != dwCreationDisposition))) {

		if(INVALID_HANDLE_VALUE == (this->hFile = ::CreateFileW(absolute_path.wstring().c_str(), dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwFlagsAndAttributes, NULL))) {
			assert(FALSE);
			return FALSE;
		}
	} else {
		return FALSE;
	}

	return TRUE;
}


uint8_t* CMapViewFile::ViewReal(QWORD qwAddress, DWORD dwSize, DWORD dwDesiredAccess)
{
	//文件映射地址必须是64k(0x10000)的整数

	UNQWORD	qwViewAddress;
	qwViewAddress.qwValue = qwAddress;

	DWORD	dwMapViewBlock64KAlignd, dwMapViewBlockLow, dwNumberOfBytesToMap;

	dwMapViewBlock64KAlignd = qwViewAddress.dwValue & 0xffff0000;
	dwMapViewBlockLow = qwViewAddress.dwValue & 0xffff;
	dwNumberOfBytesToMap = dwMapViewBlockLow + dwSize;

	LPVOID lpMapAddress = MapViewOfFile(
		this->hFileMapping, // Handle to mapping object.
		dwDesiredAccess, // Read/write permission
		qwViewAddress.dwValueHigh, // Max. object size.
		dwMapViewBlock64KAlignd, // Size of hFile.
		dwNumberOfBytesToMap); // Map entire file.

	if(NULL == lpMapAddress)
		return nullptr;
	else {
		this->vMapAddress.push_back(lpMapAddress);
		return ((LPBYTE)lpMapAddress + dwMapViewBlockLow);
	}
}

void CMapViewFile::SetFilePointer(QWORD lDistanceToMove, DWORD dwMoveMethod)
{
	UNQWORD uqwCurrentPos;
	uqwCurrentPos.qwValue = lDistanceToMove;
	
	if (INVALID_SET_FILE_POINTER == ::SetFilePointer(hFile, uqwCurrentPos.dwValue, &uqwCurrentPos.lValueHigh, dwMoveMethod)) 
		throw std::runtime_error("SetFilePointer fail");

}

QWORD CMapViewFile::GetFilePointer()
{
	LARGE_INTEGER uqwCurrentPos, uqwNewCurrentPos;
	uqwCurrentPos.QuadPart = 0;
	uqwNewCurrentPos.QuadPart = 0;

	if(::SetFilePointerEx(hFile, uqwCurrentPos, &uqwNewCurrentPos, FILE_CURRENT)) {
		return uqwNewCurrentPos.QuadPart;
	}
	return INVALID_SET_FILE_POINTER;
}

void CMapViewFile::UnmapView(void* lpTargetAddress)
{

	if(nullptr != lpTargetAddress) {
		auto lpMapAddress = (const void*)(((uintptr_t)lpTargetAddress) & ~(0xffff));
		::UnmapViewOfFile(lpMapAddress);

		auto result = find(vMapAddress.begin(), vMapAddress.end(), lpMapAddress);
		if(result != vMapAddress.end())
			vMapAddress.erase(result);
	}

}

void CMapViewFile::UnmapViewAll()
{
	size_t count = this->vMapAddress.size();
	for(int i = 0;i < count;i++) {

		::UnmapViewOfFile(this->vMapAddress[i]);
	}
	this->vMapAddress.clear();
}

void CMapViewFile::UnMaping()
{
	if(nullptr != this->hFileMapping)
		::CloseHandle(this->hFileMapping);
	this->hFileMapping = nullptr;
}

DWORD CMapViewFile::Read(LPVOID buffer, DWORD dwBytesToRead)
{
	DWORD	dwFileBytesRead = 0;

	if(!::ReadFile(this->hFile, buffer, dwBytesToRead, &dwFileBytesRead, NULL)) {
		return 0;
	}

	return dwFileBytesRead;
}


QWORD CMapViewFile::GetFileSize()
{
	UNQWORD cqwSize;
	cqwSize.dwValue = ::GetFileSize(this->hFile, &cqwSize.dwValueHigh);
	return cqwSize.qwValue;
}

//自动生成CreateFileMappingA时所需要的name，要求同时正在使用的FileMapping的name不可重复
LPCSTR CMapViewFile::GenerateMapName()
{
	//
	static unsigned int dwDupRemover = 0;

	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);

	sprintf_s(this->szFileMappingName, "mapv%x%x%x", GetCurrentThreadId(), counter.LowPart, dwDupRemover++);
#ifdef _DEBUG
	OutputDebugStringA(__FUNCTION__);
	OutputDebugStringA(": ");
	OutputDebugStringA(this->szFileMappingName);
	OutputDebugStringA("\r\n");
#endif
	return this->szFileMappingName;
}
