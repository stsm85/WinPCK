//////////////////////////////////////////////////////////////////////
// CMapViewFile.cpp: 用于映射文件视图
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#include "MapViewFile.h"



BOOL CMapViewFile::isWinNt()
{
	DWORD dwWinVersion = ::GetVersion();
	return (LOBYTE(LOWORD(dwWinVersion)) >= 4 && dwWinVersion < 0x80000000);
}

void CMapViewFile::MakeUnlimitedPath(WCHAR *_dst, LPCWSTR	_src, size_t size)
{
	WCHAR	*prefix;
	// (isUNC ? 1 : 0) ... PATH_UNC_PREFIX の場合、\\server -> \\?\UNC\server 
	//  にするため、\\server の頭の \ を一つ潰す。
	BOOL	isUNC = ( '\\' == *_src ) ? _src++, TRUE : FALSE;

	prefix		= isUNC ? PATHW_UNC_PREFIX : PATHW_LOCAL_PREFIX;
	
	wcscpy_s(_dst, size, prefix);
	wcscat_s(_dst, size, _src);

}

void CMapViewFile::MakeUnlimitedPath(char *_dst, LPCSTR _src, size_t size)
{
	char	*prefix;
	// (isUNC ? 1 : 0) ... PATH_UNC_PREFIX の場合、\\server -> \\?\UNC\server 
	//  にするため、\\server の頭の \ を一つ潰す。
	BOOL	isUNC = ( '\\' == *_src ) ? _src++, TRUE : FALSE;

	prefix		= isUNC ? PATH_UNC_PREFIX : PATH_LOCAL_PREFIX;
	
	strcpy_s(_dst, size, prefix);
	strcat_s(_dst, size, _src);

}
#ifdef USE_MAX_SINGLE_FILESIZE

void CMapViewFile::GetPkxName(char *dst, LPCSTR src)
{

	strcpy_s(dst, MAX_PATH, src);

	int slen = strlen(dst);

	char *lpszDst = dst + slen - 2;

	//.pck -> .pkx
	*lpszDst++ = 'k';
	*lpszDst = 'x';

}

void CMapViewFile::GetPkxName(LPWSTR dst, LPCWSTR src)
{
	wcscpy_s(dst, MAX_PATH, src);

	int slen = wcslen(dst);

	wchar_t *lpszDst = dst + slen - 2;

	//.pck -> .pkx
	*lpszDst++ = 'k';
	*lpszDst = 'x';
}

#endif

CMapViewFile::CMapViewFile()
{
	hFile  = hFileMapping  = NULL;
	lpMapAddress  = NULL;

#ifdef USE_MAX_SINGLE_FILESIZE
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

	if(NULL != hFileMapping){
		CloseHandle(hFileMapping);
		hFileMapping = NULL;
	}

	if(NULL != hFile && INVALID_HANDLE_VALUE != hFile){
		CloseHandle(hFile);
		hFile = NULL;
	}
#ifdef USE_MAX_SINGLE_FILESIZE
	if(hasPkx){

		if(NULL != hFileMapping2){
			CloseHandle(hFileMapping2);
			hFileMapping2 = NULL;
		}

		if(NULL != hFile2 && INVALID_HANDLE_VALUE != hFile2){
			CloseHandle(hFile2);
			hFile2 = NULL;
		}

		//pkx文件为0时，删除
		if(0 == dwPkxSize){
			if(0 != *m_szPkxFileName)
				DeleteFileA(m_szPkxFileName);
			else if(0 != *m_tszPkxFileName)
				DeleteFile(m_tszPkxFileName);
		}
	}
#endif
}



LPBYTE CMapViewFile::ViewReal(LPVOID & lpMapAddress, HANDLE hFileMapping, DWORD dwAddress, DWORD dwSize)
{
	//文件映射地址必须是64k(0x10000)的整数

	DWORD	dwMapViewBlockHigh, dwMapViewBlockLow, dwNumberOfBytesToMap;

	dwMapViewBlockHigh = dwAddress & 0xffff0000;
	dwMapViewBlockLow = dwAddress & 0xffff;
	dwNumberOfBytesToMap = dwMapViewBlockLow + dwSize;

	DWORD dwDesiredAccess = isWriteMode ? FILE_MAP_WRITE : FILE_MAP_READ;

	lpMapAddress = MapViewOfFile(hFileMapping, // Handle to mapping object.
								dwDesiredAccess, // Read/write permission
								0, // Max. object size.
								dwMapViewBlockHigh, // Size of hFile.
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
#ifdef USE_MAX_SINGLE_FILESIZE
	if(hasPkx){

		//当起始地址大于pck文件时
		if(qdwAddress >= uqdwMaxPckSize.qwValue){
			DWORD	dwAddressPkx = qdwAddress - uqdwMaxPckSize.qwValue;
			return ViewReal(lpMapAddress2, hFileMapping2, dwAddressPkx, dwSize);
		}else{

			QWORD	dwViewEndAT = qdwAddress + dwSize;

			//当View的块全在文件pck内时
			if(dwViewEndAT < uqdwMaxPckSize.qwValue){

				return ViewReal(lpMapAddress, hFileMapping, qdwAddress, dwSize);
			}else{

				//当View的块在文件pck内和pkx内

				if(NULL == (lpCrossBuffer = (LPBYTE) malloc ( dwSize ))){

					return NULL;
				}

				dwViewSizePck = uqdwMaxPckSize.qwValue - qdwAddress;
				dwViewSizePkx = dwSize - dwViewSizePck;
				//dwCrossAddress = dwAddress;

				LPBYTE buf;

				if(NULL == (buf = ViewReal(lpMapAddress, hFileMapping, qdwAddress, dwViewSizePck))){

					free(lpCrossBuffer);
					lpCrossBuffer = NULL;

					return NULL;
				}

				memcpy(lpCrossBuffer, buf, dwViewSizePck);
				//保存pck的buf，在unmap时用于回写数据
				lpCrossAddressPck = buf;

				if(NULL == (buf = ViewReal(lpMapAddress2, hFileMapping2, 0, dwViewSizePkx))){

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
	}else
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

#ifdef USE_MAX_SINGLE_FILESIZE
	if(hasPkx){

		UNQWORD	uqdwSetRealPointerAt;

		switch(dwMoveMethod){
		case FILE_BEGIN:
				uqwCurrentPos.qwValue = lDistanceToMove;
			break;

		case FILE_CURRENT:
				uqwCurrentPos.qwValue += (__int64)lDistanceToMove;
			break;

		case FILE_END:
				uqwCurrentPos.qwValue = uqwFullSize.qwValue + (__int64)lDistanceToMove;
			break;
		}

		if(uqwCurrentPos.qwValue >= uqdwMaxPckSize.qwValue){
			uqdwSetRealPointerAt.qwValue = uqwCurrentPos.qwValue - uqdwMaxPckSize.qwValue;
			
			::SetFilePointer(hFile, uqdwMaxPckSize.dwValue, &uqdwMaxPckSize.lValueHigh, FILE_BEGIN);
			::SetFilePointer(hFile2, uqdwSetRealPointerAt.dwValue, &uqdwSetRealPointerAt.lValueHigh, FILE_BEGIN);
		}else{
			::SetFilePointer(hFile, uqwCurrentPos.dwValue, &uqwCurrentPos.lValueHigh, FILE_BEGIN);
			::SetFilePointer(hFile2, 0, 0, FILE_BEGIN);
		}

	}else
#endif
	{
		//dwPosHigh = HIWORD(lDistanceToMove);
		uqwCurrentPos.qwValue = lDistanceToMove;
		::SetFilePointer(hFile, uqwCurrentPos.dwValue, &uqwCurrentPos.lValueHigh, dwMoveMethod);
	}
}

void CMapViewFile::UnmapView()
{
#ifdef USE_MAX_SINGLE_FILESIZE
	if(hasPkx){

		if(isCrossView){

			if(NULL != lpCrossBuffer){

				//当跨文件View时，写入的数据需要手动回写
				if(isWriteMode){
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
#ifdef USE_MAX_SINGLE_FILESIZE
	if(hasPkx){

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
#ifdef USE_MAX_SINGLE_FILESIZE
	if(hasPkx){

		//DWORD	dwAddressPck, dwAddressPkx;

		//当起始地址大于pck文件时
		if(uqwCurrentPos.qwValue >= uqdwMaxPckSize.qwValue){
			//dwAddressPkx = dwCurrentPos - dwPckSize;

			if(!ReadFile(hFile2, buffer, dwBytesToRead, &dwFileBytesRead, NULL))
			{
				return 0;
			}

		}else{
			//UNQWORD uqwReadEndAT;
			QWORD qwReadEndAT = uqwCurrentPos.qwValue + dwBytesToRead;

			//当Read的块全在文件pck内时
			if(qwReadEndAT < uqdwMaxPckSize.qwValue){

				if(!ReadFile(hFile, buffer, dwBytesToRead, &dwFileBytesRead, NULL))
				{
					return 0;
				}
			}else{

				//当Read的块在文件pck内和pkx内
				//读pck
				DWORD dwReadInPck = uqdwMaxPckSize.qwValue - uqwCurrentPos.qwValue;
				DWORD dwReadInPkx = dwBytesToRead - dwReadInPck;

				if(!ReadFile(hFile, buffer, dwReadInPck, &dwFileBytesRead, NULL))
				{
					return 0;
				}

				dwReadInPck = dwFileBytesRead;
				//读pkx
				if(!ReadFile(hFile2, ((LPBYTE)buffer) + dwReadInPck, dwReadInPkx, &dwFileBytesRead, NULL))
				{
					return 0;
				}

				dwFileBytesRead += dwReadInPck;
			}
		}

		uqwCurrentPos.qwValue += dwFileBytesRead;
	}else
#endif
	{
		if(!ReadFile(hFile, buffer, dwBytesToRead, &dwFileBytesRead, NULL))
		{
			return 0;
		}
	}

	return dwFileBytesRead;
}

QWORD CMapViewFile::GetFileSize()
{
	UNQWORD cqwSize;
#ifdef USE_MAX_SINGLE_FILESIZE
	if(!IsPckFile){
#endif
		cqwSize.dwValue = ::GetFileSize(hFile, &cqwSize.dwValueHigh);
		return cqwSize.qwValue;
#ifdef USE_MAX_SINGLE_FILESIZE
	}else
		return uqwFullSize.qwValue;
#endif
}