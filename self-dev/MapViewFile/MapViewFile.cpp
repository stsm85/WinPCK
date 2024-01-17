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


#define PATH_LOCAL_PREFIX			"\\\\?\\"
#define PATH_UNC_PREFIX				"\\\\?\\UNC"

#define PATHW_LOCAL_PREFIX			L"\\\\?\\"
#define PATHW_UNC_PREFIX			L"\\\\?\\UNC"

#define PATH_LOCAL_PREFIX_LEN		(strlen(PATH_LOCAL_PREFIX))
#define PATH_UNC_PREFIX_LEN			(strlen(PATH_UNC_PREFIX))



CMapViewFile::CMapViewFile() :
	hFile(NULL),
	hFileMapping(NULL),
	m_szDisk{ 0 }
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
#if 0
void CMapViewFile::MakeUnlimitedPath(LPWSTR _dst, LPCWSTR	_src, size_t size)
{
	const WCHAR	*prefix;
	// (isUNC ? 1 : 0) ... PATH_UNC_PREFIX の場合、\\server -> \\?\UNC\server 
	//  にするため、\\server の頭の \ を一つ潰す。
	BOOL	isUNC = ('\\' == *_src) ? _src++, TRUE : FALSE;

	prefix = isUNC ? PATHW_UNC_PREFIX : PATHW_LOCAL_PREFIX;

	wcscpy_s(_dst, size, prefix);
	wcscat_s(_dst, size, _src);

}

void CMapViewFile::MakeUnlimitedPath(LPSTR _dst, LPCSTR _src, size_t size)
{
	const char	*prefix;
	BOOL	isUNC = ('\\' == *_src) ? _src++, TRUE : FALSE;
	prefix = isUNC ? PATH_UNC_PREFIX : PATH_LOCAL_PREFIX;

	strcpy_s(_dst, size, prefix);
	strcat_s(_dst, size, _src);

}
#endif

void CMapViewFile::GetDiskNameFromFilename(fs::path& lpszFilename)
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
#if 0
BOOL CMapViewFile::FileExists(LPCWSTR szFileName)
{
	DWORD dwResult = GetFileAttributesW(szFileName);
	return (dwResult != INVALID_FILE_ATTRIBUTES && !(dwResult & FILE_ATTRIBUTE_DIRECTORY));
}
#endif
BOOL CMapViewFile::Open(const fs::path& lpszFilename, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes)
{

	fs::path absolute_path = fs::absolute(lpszFilename);
	this->GetDiskNameFromFilename(absolute_path);
	
	if(this->FileExists(absolute_path) ||
		((OPEN_EXISTING != dwCreationDisposition) && (TRUNCATE_EXISTING != dwCreationDisposition))) {
		char szFilename[MAX_PATH];
		if(INVALID_HANDLE_VALUE == (this->hFile = ::CreateFileW(absolute_path.wstring().c_str(), dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwFlagsAndAttributes, NULL))) {
			//MakeUnlimitedPath(szFilename, m_szFullFilename, MAX_PATH);
			//if(INVALID_HANDLE_VALUE == (hFile = CreateFileA(szFilename, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwFlagsAndAttributes, NULL))) {
				assert(FALSE);
				return FALSE;
			//}
		}
	} else {
		return FALSE;
	}

	return TRUE;
}
#if 0
BOOL CMapViewFile::Open(LPCWSTR lpszFilename, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes)
{
	wchar_t	m_wszFullFilename[MAX_PATH];

	GetFullPathNameW(lpszFilename, MAX_PATH, m_wszFullFilename, NULL);
	GetDiskNameFromFilename(m_wszFullFilename);

	if(FileExists(m_wszFullFilename) ||
		((OPEN_EXISTING != dwCreationDisposition) && (TRUNCATE_EXISTING != dwCreationDisposition))) {
		WCHAR szFilename[MAX_PATH];

		if(INVALID_HANDLE_VALUE == (hFile = CreateFileW(m_wszFullFilename, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwFlagsAndAttributes, NULL))) {

			MakeUnlimitedPath(szFilename, m_wszFullFilename, MAX_PATH);
			if(INVALID_HANDLE_VALUE == (hFile = CreateFileW(szFilename, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwFlagsAndAttributes, NULL))) {
				assert(FALSE);
				return FALSE;
			}

		}
	} else {
		return FALSE;
	}

	return TRUE;
}
#endif

uint8_t* CMapViewFile::ViewReal(QWORD qwAddress, DWORD dwSize, DWORD dwDesiredAccess)
{
	//文件映射地址必须是64k(0x10000)的整数

	UNQWORD	qwViewAddress;
	qwViewAddress.qwValue = qwAddress;

	DWORD	dwMapViewBlock64KAlignd, dwMapViewBlockLow, dwNumberOfBytesToMap;

	dwMapViewBlock64KAlignd = qwViewAddress.dwValue & 0xffff0000;
	dwMapViewBlockLow = qwViewAddress.dwValue & 0xffff;
	dwNumberOfBytesToMap = dwMapViewBlockLow + dwSize;

	//DWORD dwDesiredAccess = isWriteMode ? FILE_MAP_WRITE : FILE_MAP_READ;

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
