//////////////////////////////////////////////////////////////////////
// CMapViewFileRead.cpp: 用于映射文件视图（读）
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2014.4.24
//////////////////////////////////////////////////////////////////////

#include "MapViewFile.h"

CMapViewFileRead::CMapViewFileRead()
{
	isWriteMode = FALSE;
}

CMapViewFileRead::~CMapViewFileRead()
{
}

#ifdef USE_MAX_SINGLE_FILESIZE

BOOL CMapViewFileRead::OpenPck(LPCSTR lpszFilename)
{

	IsPckFile = TRUE;

	if(Open(lpszFilename)){

		//strcpy_s(m_szPckFilename, MAX_PATH, lpszFilename);

		dwPkxSize = 0;
		dwPckSize = ::GetFileSize(hFile, NULL);

		//if(MAX_PCKFILE_SIZE <= dwPckSize){

			//char szFilename[MAX_PATH];

			GetPkxName(m_szPkxFileName, lpszFilename);

			HANDLE hFilePck = hFile;

			if(Open(m_szPkxFileName)){

				hFile2 = hFile;
				hasPkx = TRUE;
				uqdwMaxPckSize.qwValue = dwPckSize;

				dwPkxSize = ::GetFileSize(hFile2, NULL);

			}

			hFile = hFilePck;

		//}

		uqwFullSize.qwValue = dwPckSize + dwPkxSize;

	}else{
		return FALSE;
	}

	return TRUE;
}

BOOL CMapViewFileRead::OpenPck(LPCWSTR lpszFilename)
{

	IsPckFile = TRUE;

	if(Open(lpszFilename)){

		dwPkxSize = 0;
		dwPckSize = ::GetFileSize(hFile, NULL);

		//if(MAX_PCKFILE_SIZE <= dwPckSize){

			//TCHAR szFilename[MAX_PATH];

			GetPkxName(m_tszPkxFileName, lpszFilename);

			HANDLE hFilePck = hFile;
			

			if(Open(m_tszPkxFileName)){

				hFile2 = hFile;
				hasPkx = TRUE;
				uqdwMaxPckSize.qwValue = dwPckSize;

				dwPkxSize = ::GetFileSize(hFile2, NULL);

			}

			hFile = hFilePck;

		//}

		uqwFullSize.qwValue = dwPckSize + dwPkxSize;

	}else{
		return FALSE;
	}

	return TRUE;

}
#endif

BOOL CMapViewFileRead::Open(LPCSTR lpszFilename)
{
	char szFilename[MAX_PATH];

	if(INVALID_HANDLE_VALUE == (hFile = CreateFileA(lpszFilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL)))
	{
		if(isWinNt())
		{
			MakeUnlimitedPath(szFilename, lpszFilename, MAX_PATH);
			if(INVALID_HANDLE_VALUE == (hFile = CreateFileA(szFilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL)))
				return FALSE;
		}else{
			return FALSE;
		}
	}
#ifdef USE_MAX_SINGLE_FILESIZE
	strcpy_s(m_szPckFileName, MAX_PATH, lpszFilename);
#endif
	return TRUE;
}

BOOL CMapViewFileRead::Open(LPCWSTR lpszFilename)
{
	WCHAR szFilename[MAX_PATH];

	if(INVALID_HANDLE_VALUE == (hFile = CreateFileW(lpszFilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL)))
	{
		if(isWinNt())
		{
			MakeUnlimitedPath(szFilename, lpszFilename, MAX_PATH);
			if(INVALID_HANDLE_VALUE == (hFile = CreateFileW(szFilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL)))
				return FALSE;
		}else{
			return FALSE;
		}

	}
#ifdef USE_MAX_SINGLE_FILESIZE
	wcscpy_s(m_tszPckFileName, MAX_PATH, lpszFilename);
#endif
	return TRUE;
}

BOOL CMapViewFileRead::Mapping(LPCSTR lpszNamespace)
{
	if(NULL == (hFileMapping = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, lpszNamespace))){

		return FALSE;
	}
#ifdef USE_MAX_SINGLE_FILESIZE
	if(hasPkx){
		char szNamespace_2[16];
		memcpy(szNamespace_2, lpszNamespace, 16);
		strcat_s(szNamespace_2, 16, "_2");

		if(NULL == (hFileMapping2 = CreateFileMappingA(hFile2, NULL, PAGE_READONLY, 0, 0, szNamespace_2))){

			return FALSE;
		}

	}
#endif
	return TRUE;

}
