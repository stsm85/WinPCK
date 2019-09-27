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

#include "MapViewFile.h"
#include <stdio.h>
//#include <algorithm>

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
	vMapAddress.clear();
}

CMapViewFile::~CMapViewFile() throw()
{
	clear();
}

void CMapViewFile::clear() 
{
	UnmapViewAll();

	if(NULL != hFileMapping) {
		CloseHandle(hFileMapping);
		hFileMapping = NULL;
	}

	if(NULL != hFile && INVALID_HANDLE_VALUE != hFile) {
		CloseHandle(hFile);
		hFile = NULL;
	}

}

void CMapViewFile::MakeUnlimitedPath(LPWSTR _dst, LPCWSTR	_src, size_t size)
{
	WCHAR	*prefix;
	// (isUNC ? 1 : 0) ... PATH_UNC_PREFIX の場合、\\server -> \\?\UNC\server 
	//  にするため、\\server の頭の \ を一つ潰す。
	BOOL	isUNC = ('\\' == *_src) ? _src++, TRUE : FALSE;

	prefix = isUNC ? PATHW_UNC_PREFIX : PATHW_LOCAL_PREFIX;

	wcscpy_s(_dst, size, prefix);
	wcscat_s(_dst, size, _src);

}

void CMapViewFile::MakeUnlimitedPath(LPSTR _dst, LPCSTR _src, size_t size)
{
	char	*prefix;
	BOOL	isUNC = ('\\' == *_src) ? _src++, TRUE : FALSE;
	prefix = isUNC ? PATH_UNC_PREFIX : PATH_LOCAL_PREFIX;

	strcpy_s(_dst, size, prefix);
	strcat_s(_dst, size, _src);

}


template <typename T>
void CMapViewFile::GetDiskNameFromFilename(T* lpszFilename)
{
	m_szDisk[0] = lpszFilename[0];
	m_szDisk[1] = lpszFilename[1];
	m_szDisk[2] = lpszFilename[2];
	m_szDisk[3] = '\0';
}

const char * CMapViewFile::GetFileDiskName()
{
	return m_szDisk;
}


/*****************************************************************************
BOOL FileExists(CONST TCHAR szName[MAX_PATH_EX])
szName	: (IN) string

Return Value:
returns TRUE if szName exists and is not a directory; FALSE otherwise
*****************************************************************************/
BOOL CMapViewFile::FileExists(LPCSTR szFileName)
{
	DWORD dwResult = GetFileAttributesA(szFileName);
	return (dwResult != INVALID_FILE_ATTRIBUTES && !(dwResult & FILE_ATTRIBUTE_DIRECTORY));
}

BOOL CMapViewFile::FileExists(LPCWSTR szFileName)
{
	DWORD dwResult = GetFileAttributesW(szFileName);
	return (dwResult != INVALID_FILE_ATTRIBUTES && !(dwResult & FILE_ATTRIBUTE_DIRECTORY));
}

BOOL CMapViewFile::Open(LPCSTR lpszFilename, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes)
{
	char m_szFullFilename[MAX_PATH];

	GetFullPathNameA(lpszFilename, MAX_PATH, m_szFullFilename, NULL);
	GetDiskNameFromFilename(m_szFullFilename);

	if(FileExists(m_szFullFilename) ||
		((OPEN_EXISTING != dwCreationDisposition) && (TRUNCATE_EXISTING != dwCreationDisposition))) {
		char szFilename[MAX_PATH];
		if(INVALID_HANDLE_VALUE == (hFile = CreateFileA(m_szFullFilename, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwFlagsAndAttributes, NULL))) {
			MakeUnlimitedPath(szFilename, m_szFullFilename, MAX_PATH);
			if(INVALID_HANDLE_VALUE == (hFile = CreateFileA(szFilename, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwFlagsAndAttributes, NULL))) {
				assert(FALSE);
				return FALSE;
			}
		}
	} else {
		return FALSE;
	}

	return TRUE;
}

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
		hFileMapping, // Handle to mapping object.
		dwDesiredAccess, // Read/write permission
		qwViewAddress.dwValueHigh, // Max. object size.
		dwMapViewBlock64KAlignd, // Size of hFile.
		dwNumberOfBytesToMap); // Map entire file.

	if(NULL == lpMapAddress)
		return NULL;
	else {
		vMapAddress.push_back(lpMapAddress);
		return ((LPBYTE)lpMapAddress + dwMapViewBlockLow);
	}
}

LPBYTE CMapViewFile::View(QWORD dwAddress, DWORD dwSize)
{
	throw std::exception("programe can not reach here");
	return NULL;
}

void CMapViewFile::SetFilePointer(QWORD lDistanceToMove, DWORD dwMoveMethod)
{
	UNQWORD uqwCurrentPos;
	uqwCurrentPos.qwValue = lDistanceToMove;
	
	if (INVALID_SET_FILE_POINTER == ::SetFilePointer(hFile, uqwCurrentPos.dwValue, &uqwCurrentPos.lValueHigh, dwMoveMethod)) 
		throw std::exception("SetFilePointer fail");

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

	if(NULL != lpTargetAddress) {
		void* lpMapAddress = (void*)(((uintptr_t)lpTargetAddress) & ~(0xffff));
		UnmapViewOfFile(lpMapAddress);

		std::vector<void*>::const_iterator result = find(vMapAddress.begin(), vMapAddress.end(), lpMapAddress);
		if(result != vMapAddress.end())
			vMapAddress.erase(result);
	}

}

void CMapViewFile::UnmapViewAll()
{
	size_t count = vMapAddress.size();
	for(int i = 0;i < count;i++) {

		UnmapViewOfFile(vMapAddress[i]);
	}
	vMapAddress.clear();
}

void CMapViewFile::UnMaping()
{
	if(NULL != hFileMapping)
		CloseHandle(hFileMapping);
	hFileMapping = NULL;
}

DWORD CMapViewFile::Read(LPVOID buffer, DWORD dwBytesToRead)
{
	DWORD	dwFileBytesRead = 0;

	if(!ReadFile(hFile, buffer, dwBytesToRead, &dwFileBytesRead, NULL)) {
		return 0;
	}

	return dwFileBytesRead;
}


QWORD CMapViewFile::GetFileSize()
{
	UNQWORD cqwSize;
	cqwSize.dwValue = ::GetFileSize(hFile, &cqwSize.dwValueHigh);
	return cqwSize.qwValue;
}

//自动生成CreateFileMappingA时所需要的name，要求同时正在使用的FileMapping的name不可重复
LPCSTR CMapViewFile::GenerateMapName()
{
	//
	static unsigned int dwDupRemover = 0;

	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);

	sprintf_s(szFileMappingName, "mapv%x%x%x", GetCurrentThreadId(), counter.LowPart, dwDupRemover++);
#ifdef _DEBUG
	OutputDebugStringA(__FUNCTION__);
	OutputDebugStringA(": ");
	OutputDebugStringA(szFileMappingName);
	OutputDebugStringA("\r\n");
#endif
	return szFileMappingName;
}