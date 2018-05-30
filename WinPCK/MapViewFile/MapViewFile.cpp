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

BOOL CMapViewFile::isWinNt()
{
	DWORD dwWinVersion = ::GetVersion();
	return (LOBYTE(LOWORD(dwWinVersion)) >= 4 && dwWinVersion < 0x80000000);
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

/*****************************************************************************
BOOL FileExists(CONST TCHAR szName[MAX_PATH_EX])
szName	: (IN) string

Return Value:
returns TRUE if szName exists and is not a directory; FALSE otherwise
*****************************************************************************/
BOOL CMapViewFile::FileExists(LPCSTR szName)
{
	DWORD dwResult = GetFileAttributesA(szName);
	return (dwResult != INVALID_FILE_ATTRIBUTES && !(dwResult & FILE_ATTRIBUTE_DIRECTORY));
}

BOOL CMapViewFile::FileExists(LPCWSTR szName)
{
	DWORD dwResult = GetFileAttributesW(szName);
	return (dwResult != INVALID_FILE_ATTRIBUTES && !(dwResult & FILE_ATTRIBUTE_DIRECTORY));
}

BOOL CMapViewFile::Open(HANDLE &hFile, LPCSTR lpszFilename, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes)
{
	if(FileExists(lpszFilename) ||
		((OPEN_EXISTING != dwCreationDisposition) && (TRUNCATE_EXISTING != dwCreationDisposition))) {
		char szFilename[MAX_PATH];
		if(INVALID_HANDLE_VALUE == (hFile = CreateFileA(lpszFilename, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwFlagsAndAttributes, NULL))) {
			if(isWinNt()) {
				MakeUnlimitedPath(szFilename, lpszFilename, MAX_PATH);
				if(INVALID_HANDLE_VALUE == (hFile = CreateFileA(szFilename, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwFlagsAndAttributes, NULL))) {
					assert(FALSE);
					return FALSE;
				}
			} else {
				assert(FALSE);
				return FALSE;
			}
		}
	} else {
		return FALSE;
	}

	return TRUE;
}

BOOL CMapViewFile::Open(HANDLE &hFile, LPCWSTR lpszFilename, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes)
{
	if(FileExists(lpszFilename) ||
		((OPEN_EXISTING != dwCreationDisposition) && (TRUNCATE_EXISTING != dwCreationDisposition))) {
		WCHAR szFilename[MAX_PATH];

		if(INVALID_HANDLE_VALUE == (hFile = CreateFileW(lpszFilename, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwFlagsAndAttributes, NULL))) {
			if(isWinNt()) {
				MakeUnlimitedPath(szFilename, lpszFilename, MAX_PATH);
				if(INVALID_HANDLE_VALUE == (hFile = CreateFileW(szFilename, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwFlagsAndAttributes, NULL))) {
					assert(FALSE);
					return FALSE;
				}
			} else {
				assert(FALSE);
				return FALSE;
			}
		}
	} else {
		return FALSE;
	}

	return TRUE;
}

#if ENABLE_PCK_PKX_FILE

void CMapViewFile::GetPkxName(LPSTR dst, LPCSTR src)
{

	strcpy_s(dst, MAX_PATH, src);
	int slen = strlen(dst);
	char *lpszDst = dst + slen - 2;

	//.pck -> .pkx
	*(__int16*)lpszDst = *(__int16*)"kx";


}

void CMapViewFile::GetPkxName(LPWSTR dst, LPCWSTR src)
{
	wcscpy_s(dst, MAX_PATH, src);
	int slen = wcslen(dst);
	wchar_t *lpszDst = dst + slen - 2;

	//.pck -> .pkx
	*(__int32*)lpszDst = *(__int32*)L"kx";

}

#endif

CMapViewFile::CMapViewFile()
{
	hFile = hFileMapping = NULL;
	lpMapAddress = NULL;

#if ENABLE_PCK_PKX_FILE
	hFile2 = hFileMapping2 = NULL;
	IsPckFile = hasPkx = isCrossView = FALSE;
	lpMapAddress2 = NULL;
	lpCrossBuffer = NULL;
#endif
	uqwCurrentPos.qwValue = 0;

}

CMapViewFile::~CMapViewFile()
{
	clear();
}

void CMapViewFile::clear()
{


	UnmapView();
	//UnMaping();

	if(NULL != hFileMapping) {
		CloseHandle(hFileMapping);
		hFileMapping = NULL;
	}

	if(NULL != hFile && INVALID_HANDLE_VALUE != hFile) {
		CloseHandle(hFile);
		hFile = NULL;
	}
#if ENABLE_PCK_PKX_FILE
	if(hasPkx) {

		if(NULL != hFileMapping2) {
			CloseHandle(hFileMapping2);
			hFileMapping2 = NULL;
		}

		if(NULL != hFile2 && INVALID_HANDLE_VALUE != hFile2) {
			CloseHandle(hFile2);
			hFile2 = NULL;
		}

		//pkx文件为0时，删除
		if(0 == dwPkxSize.qwValue) {
			if(0 != *m_szPkxFileName)
				DeleteFileA(m_szPkxFileName);
			else if(0 != *m_tszPkxFileName)
				DeleteFileW(m_tszPkxFileName);
		}
	}
#endif
}



LPBYTE CMapViewFile::ViewReal(LPVOID & lpMapAddress, HANDLE hFileMapping, QWORD qwAddress, DWORD dwSize)
{
	//文件映射地址必须是64k(0x10000)的整数

	UNQWORD	qwViewAddress;
	qwViewAddress.qwValue = qwAddress;

	DWORD	dwMapViewBlock64KAlignd, dwMapViewBlockLow, dwNumberOfBytesToMap;

	dwMapViewBlock64KAlignd = qwViewAddress.dwValue & 0xffff0000;
	dwMapViewBlockLow = qwViewAddress.dwValue & 0xffff;
	dwNumberOfBytesToMap = dwMapViewBlockLow + dwSize;

	DWORD dwDesiredAccess = isWriteMode ? FILE_MAP_WRITE : FILE_MAP_READ;

	lpMapAddress = MapViewOfFile(hFileMapping, // Handle to mapping object.
		dwDesiredAccess, // Read/write permission
		qwViewAddress.dwValueHigh, // Max. object size.
		dwMapViewBlock64KAlignd, // Size of hFile.
		dwNumberOfBytesToMap); // Map entire file.
	if(NULL == lpMapAddress)
		return (lpTargetBuffer = NULL);
	else
		return (lpTargetBuffer = ((LPBYTE)lpMapAddress + dwMapViewBlockLow));
}

LPBYTE CMapViewFile::View(QWORD qdwAddress, DWORD dwSize)
{

	//当dwSize=0时，VIEW整个文件，
	//当hasPkx=1时，dwSize=0不会发生
#if ENABLE_PCK_PKX_FILE
	if(hasPkx) {

		//当起始地址大于pck文件时
		if(qdwAddress >= uqdwMaxPckSize.qwValue) {
			QWORD	dwAddressPkx = qdwAddress - uqdwMaxPckSize.qwValue;
			return ViewReal(lpMapAddress2, hFileMapping2, dwAddressPkx, dwSize);
		} else {

			QWORD	dwViewEndAT = qdwAddress + dwSize;

			//当View的块全在文件pck内时
			if(dwViewEndAT < uqdwMaxPckSize.qwValue) {

				return ViewReal(lpMapAddress, hFileMapping, qdwAddress, dwSize);
			} else {

				//当View的块在文件pck内和pkx内

				if(NULL == (lpCrossBuffer = (LPBYTE)malloc(dwSize))) {

					return NULL;
				}

				dwViewSizePck = (DWORD)(uqdwMaxPckSize.qwValue - qdwAddress);
				dwViewSizePkx = dwSize - dwViewSizePck;
				//dwCrossAddress = dwAddress;

				LPBYTE buf;

				if(NULL == (buf = ViewReal(lpMapAddress, hFileMapping, qdwAddress, dwViewSizePck))) {

					free(lpCrossBuffer);
					lpCrossBuffer = NULL;

					return NULL;
				}

				memcpy(lpCrossBuffer, buf, dwViewSizePck);
				//保存pck的buf，在unmap时用于回写数据
				lpCrossAddressPck = buf;

				if(NULL == (buf = ViewReal(lpMapAddress2, hFileMapping2, 0, dwViewSizePkx))) {

					free(lpCrossBuffer);
					lpCrossBuffer = NULL;

					UnmapView();

					return NULL;
				}

				isCrossView = TRUE;

				memcpy(lpCrossBuffer + dwViewSizePck, buf, dwViewSizePkx);

				return lpCrossBuffer;
			}
		}
	} else
#endif
	{
		return ViewReal(lpMapAddress, hFileMapping, qdwAddress, dwSize);
	}
}


LPBYTE CMapViewFile::ReView(QWORD dwAddress, DWORD dwSize)
{
	UnmapView();
	return View(dwAddress, dwSize);
}

void CMapViewFile::SetFilePointer(QWORD lDistanceToMove, DWORD dwMoveMethod)
{
	DWORD rtn;
#if ENABLE_PCK_PKX_FILE
	if(hasPkx) {

		UNQWORD	uqdwSetRealPointerAt;

		switch(dwMoveMethod) {
		case FILE_BEGIN:
			uqwCurrentPos.qwValue = lDistanceToMove;
			break;

		case FILE_CURRENT:
			uqwCurrentPos.qwValue += (LONGLONG)lDistanceToMove;
			break;

		case FILE_END:
			uqwCurrentPos.qwValue = uqwFullSize.qwValue + (LONGLONG)lDistanceToMove;
			break;
		}

		if(uqwCurrentPos.qwValue >= uqdwMaxPckSize.qwValue) {
			uqdwSetRealPointerAt.qwValue = uqwCurrentPos.qwValue - uqdwMaxPckSize.qwValue;


			rtn = ::SetFilePointer(hFile, uqdwMaxPckSize.dwValue, &uqdwMaxPckSize.lValueHigh, FILE_BEGIN);
			assert(INVALID_SET_FILE_POINTER != rtn);
			rtn = ::SetFilePointer(hFile2, uqdwSetRealPointerAt.dwValue, &uqdwSetRealPointerAt.lValueHigh, FILE_BEGIN);
			assert(INVALID_SET_FILE_POINTER != rtn);
		} else {
			rtn = ::SetFilePointer(hFile, uqwCurrentPos.dwValue, &uqwCurrentPos.lValueHigh, FILE_BEGIN);
			assert(INVALID_SET_FILE_POINTER != rtn);
			rtn = ::SetFilePointer(hFile2, 0, 0, FILE_BEGIN);
			assert(INVALID_SET_FILE_POINTER != rtn);
		}

	} else
#endif
	{
		//dwPosHigh = HIWORD(lDistanceToMove);
		uqwCurrentPos.qwValue = lDistanceToMove;
		rtn = ::SetFilePointer(hFile, uqwCurrentPos.dwValue, &uqwCurrentPos.lValueHigh, dwMoveMethod);
		assert(INVALID_SET_FILE_POINTER != rtn);
	}
}

void CMapViewFile::UnmapView()
{
#if ENABLE_PCK_PKX_FILE
	if(hasPkx) {

		if(isCrossView) {

			if(NULL != lpCrossBuffer) {

				//当跨文件View时，写入的数据需要手动回写
				if(isWriteMode) {
					memcpy(lpCrossAddressPck, lpCrossBuffer, dwViewSizePck);
					memcpy(lpMapAddress2, lpCrossBuffer + dwViewSizePck, dwViewSizePkx);
				}
				free(lpCrossBuffer);
			}
			isCrossView = FALSE;
		}

		if(NULL != lpMapAddress2)
			UnmapViewOfFile(lpMapAddress2);
		lpMapAddress2 = NULL;
	}
#endif
	if(NULL != lpMapAddress)
		UnmapViewOfFile(lpMapAddress);

	lpMapAddress = NULL;
}

void CMapViewFile::UnMaping()
{
#if ENABLE_PCK_PKX_FILE
	if(hasPkx) {

		if(NULL != hFileMapping2)
			CloseHandle(hFileMapping2);
		hFileMapping2 = NULL;

	}
#endif
	if(NULL != hFileMapping)
		CloseHandle(hFileMapping);
	hFileMapping = NULL;
}

DWORD CMapViewFile::Read(LPVOID buffer, DWORD dwBytesToRead)
{
	DWORD	dwFileBytesRead = 0;
#if ENABLE_PCK_PKX_FILE
	if(hasPkx) {

		//当起始地址大于pck文件时
		if(uqwCurrentPos.qwValue >= uqdwMaxPckSize.qwValue) {

			if(!ReadFile(hFile2, buffer, dwBytesToRead, &dwFileBytesRead, NULL)) {
				return 0;
			}

		} else {
			QWORD qwReadEndAT = uqwCurrentPos.qwValue + dwBytesToRead;

			//当Read的块全在文件pck内时
			if(qwReadEndAT < uqdwMaxPckSize.qwValue) {

				if(!ReadFile(hFile, buffer, dwBytesToRead, &dwFileBytesRead, NULL)) {
					return 0;
				}
			} else {

				//当Read的块在文件pck内和pkx内
				//读pck
				DWORD dwReadInPck = (DWORD)(uqdwMaxPckSize.qwValue - uqwCurrentPos.qwValue);
				DWORD dwReadInPkx = dwBytesToRead - dwReadInPck;

				if(!ReadFile(hFile, buffer, dwReadInPck, &dwFileBytesRead, NULL)) {
					return 0;
				}

				dwReadInPck = dwFileBytesRead;
				//读pkx
				if(!ReadFile(hFile2, ((LPBYTE)buffer) + dwReadInPck, dwReadInPkx, &dwFileBytesRead, NULL)) {
					return 0;
				}

				dwFileBytesRead += dwReadInPck;
			}
		}

		uqwCurrentPos.qwValue += dwFileBytesRead;
	} else
#endif
	{
		if(!ReadFile(hFile, buffer, dwBytesToRead, &dwFileBytesRead, NULL)) {
			return 0;
		}
	}

	return dwFileBytesRead;
}


#if ENABLE_PCK_PKX_FILE

QWORD CMapViewFile::GetFileSize()
{
	UNQWORD cqwSize;

	if(!IsPckFile) {
		cqwSize.dwValue = ::GetFileSize(hFile, &cqwSize.dwValueHigh);
		return cqwSize.qwValue;
	} else
		return uqwFullSize.qwValue;
}

#else

QWORD CMapViewFile::GetFileSize()
{
	UNQWORD cqwSize;
	cqwSize.dwValue = ::GetFileSize(hFile, &cqwSize.dwValueHigh);
	return cqwSize.qwValue;
}

#endif

#if ENABLE_PCK_PKX_FILE
BOOL CMapViewFile::SetPckPackSize(QWORD qwPckSize)
{
	if(uqwFullSize.qwValue < qwPckSize) {
		//pck标注的文件大小比实际文件要大，文件不全
		return FALSE;
	} else if(uqwFullSize.qwValue > qwPckSize) {
		//pck标注的文件大小比实际文件要小，重新指定文件大小
		uqwFullSize.qwValue = qwPckSize;
		if(hasPkx) {
			dwPkxSize.qwValue = qwPckSize - dwPckSize.qwValue;
		}
	}

	return TRUE;
}
#endif

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