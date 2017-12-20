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
#ifdef USE_MAX_SINGLE_FILESIZE
CMapViewFileWrite::CMapViewFileWrite(DWORD dwMaxPckSize)
{
#ifdef _DEBUG
	m_Max_PckFile_Size = 0x6400000;	//100MB
#else
	m_Max_PckFile_Size = dwMaxPckSize;
#endif
	dwViewSizePck = dwViewSizePkx = 0;

	*m_szPckFileName = 0;
	*m_tszPckFileName = 0;

	*m_szPkxFileName = 0;
	*m_tszPkxFileName = 0;
	isWriteMode = TRUE;
}
#else
CMapViewFileWrite::CMapViewFileWrite()
{
	isWriteMode = TRUE;
}
#endif

//CMapViewFileWrite::CMapViewFileWrite(DWORD dwMaxPckSize)
//{
//
//#ifdef USE_MAX_SINGLE_FILESIZE
//	dwViewSizePck = dwViewSizePkx = 0;
//
//	*m_szPckFileName = 0;
//	*m_tszPckFileName = 0;
//
//	*m_szPkxFileName = 0;
//	*m_tszPkxFileName = 0;
//
//	m_Max_PckFile_Size = 0;
//#endif
//
//	isWriteMode = TRUE;
//}

CMapViewFileWrite::~CMapViewFileWrite()
{
}

#ifdef USE_MAX_SINGLE_FILESIZE

//void CMapViewFileWrite::SetMaxSinglePckSize(QWORD qwMaxPckSize)
//{
//	m_Max_PckFile_Size = qwMaxPckSize;
//}

BOOL CMapViewFileWrite::OpenPck(LPCSTR lpszFilename, DWORD dwCreationDisposition)
{

	IsPckFile = TRUE;

	if(Open(lpszFilename, dwCreationDisposition)){

		dwPkxSize = 0;
		GetPkxName(m_szPkxFileName, lpszFilename);
		dwPckSize = ::GetFileSize(hFile, NULL);

		OpenPkx(m_szPkxFileName, OPEN_EXISTING);

		uqwFullSize.qwValue = dwPckSize + dwPkxSize;

	}else{
		return FALSE;
	}

	return TRUE;

}

BOOL CMapViewFileWrite::OpenPck(LPCTSTR lpszFilename, DWORD dwCreationDisposition)
{

	IsPckFile = TRUE;

	if(Open(lpszFilename, dwCreationDisposition)){

		dwPkxSize = 0;
		GetPkxName(m_tszPkxFileName, lpszFilename);
		dwPckSize = ::GetFileSize(hFile, NULL);

		OpenPkx(m_tszPkxFileName, OPEN_EXISTING);

		uqwFullSize.qwValue = dwPckSize + dwPkxSize;

	}else{
		return FALSE;
	}

	return TRUE;

}

void CMapViewFileWrite::OpenPkx(LPCSTR lpszFilename, DWORD dwCreationDisposition)
{
	//char szFilename[MAX_PATH];

	//GetPkxName(szFilename, lpszFilename);
	HANDLE hFilePck = hFile;

	if(Open(lpszFilename, dwCreationDisposition)){

		hFile2 = hFile;
		hasPkx = TRUE;
		uqdwMaxPckSize.qwValue = dwPckSize;

		dwPkxSize = ::GetFileSize(hFile2, NULL);
	}else{
		//assert(m_Max_PckFile_Size);
		uqdwMaxPckSize.qwValue = m_Max_PckFile_Size;
	}

	hFile = hFilePck;
}

void CMapViewFileWrite::OpenPkx(LPCWSTR lpszFilename, DWORD dwCreationDisposition)
{
	//TCHAR szFilename[MAX_PATH];
	//GetPkxName(szFilename, lpszFilename);
	HANDLE hFilePck = hFile;

	if(Open(lpszFilename, dwCreationDisposition)){

		hFile2 = hFile;
		hasPkx = TRUE;
		uqdwMaxPckSize.qwValue = dwPckSize;

		dwPkxSize = ::GetFileSize(hFile2, NULL);

	}else{
		//assert(m_Max_PckFile_Size);
		uqdwMaxPckSize.qwValue = m_Max_PckFile_Size;
	}

	hFile = hFilePck;
}

#endif

BOOL CMapViewFileWrite::Open(LPCSTR lpszFilename, DWORD dwCreationDisposition)
{
	char szFilename[MAX_PATH];

	//IsPckFile = IsPckFilename(lpszFilename);

	if(INVALID_HANDLE_VALUE == (hFile = CreateFileA(lpszFilename, GENERIC_WRITE | GENERIC_READ, NULL, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL)))
	{
		if(isWinNt())
		{
			MakeUnlimitedPath(szFilename, lpszFilename, MAX_PATH);
			if(INVALID_HANDLE_VALUE == (hFile = CreateFileA(szFilename, GENERIC_READ, FILE_SHARE_READ, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL)))
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

BOOL CMapViewFileWrite::Open(LPCWSTR lpszFilename, DWORD dwCreationDisposition)
{
	WCHAR szFilename[MAX_PATH];

	//IsPckFile = IsPckFilename(lpszFilename);

	if(INVALID_HANDLE_VALUE == (hFile = CreateFileW(lpszFilename, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL)))
	{
		if(isWinNt())
		{
			MakeUnlimitedPath(szFilename, lpszFilename, MAX_PATH);
			if(INVALID_HANDLE_VALUE == (hFile = CreateFileW(szFilename, GENERIC_READ, FILE_SHARE_READ, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL)))
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

BOOL CMapViewFileWrite::Mapping(LPCSTR lpszNamespace, QWORD qwMaxSize)
{
#ifdef USE_MAX_SINGLE_FILESIZE
	if(IsPckFile && (uqdwMaxPckSize.qwValue < qwMaxSize)){

		if(NULL == (hFileMapping = CreateFileMappingA(hFile, NULL, PAGE_READWRITE, 0, (DWORD)uqdwMaxPckSize.qwValue, lpszNamespace))){
			return FALSE;
		}

		dwPckSize = uqdwMaxPckSize.qwValue;

		if(!hasPkx){
			if(0 != *m_szPkxFileName)
				OpenPkx(m_szPkxFileName, OPEN_ALWAYS);
			else if(0 != *m_tszPkxFileName)
				OpenPkx(m_tszPkxFileName, OPEN_ALWAYS);
			else{
			
				UnMaping();
				return FALSE;
			}
		}

		dwPkxSize = qwMaxSize - uqdwMaxPckSize.qwValue;
		//dwPckSize = dwMaxPckSize;

		//if(dwMaxPckSize < dwPkxSize){
		//	int a= 1;
		//	if(dwMaxPckSize < dwMaxSize){
		//		a=2;
		//	}
		//}

		uqwFullSize.qwValue = qwMaxSize;

		char szNamespace_2[16];
		memcpy(szNamespace_2, lpszNamespace, 16);
		strcat_s(szNamespace_2, 16, "_2");

		if(NULL == (hFileMapping2 = CreateFileMappingA(hFile2, NULL, PAGE_READWRITE, 0, (DWORD)dwPkxSize, szNamespace_2))){
			return FALSE;
		}


	}else
#endif	
	{
		UNQWORD uqwMaxSize;
		uqwMaxSize.qwValue = qwMaxSize;

		if(NULL == (hFileMapping = CreateFileMappingA(hFile, NULL, PAGE_READWRITE, uqwMaxSize.dwValueHigh, uqwMaxSize.dwValue, lpszNamespace))){
			return FALSE;
		}	
	}
	return TRUE;
}

void CMapViewFileWrite::SetEndOfFile()
{
#ifdef USE_MAX_SINGLE_FILESIZE
	if(hasPkx){
		if(::SetEndOfFile(hFile2))
		dwPkxSize = ::SetFilePointer(hFile2, 0, 0, FILE_CURRENT);
	}
#endif
	if(::SetEndOfFile(hFile))
		dwPckSize = ::SetFilePointer(hFile, 0, 0, FILE_CURRENT);

}

DWORD CMapViewFileWrite::Write(LPVOID buffer, DWORD dwBytesToWrite)
{
	DWORD	dwFileBytesWrote = 0;
#ifdef USE_MAX_SINGLE_FILESIZE
	if(hasPkx){

		//DWORD	dwAddressPck, dwAddressPkx;

		//当起始地址大于pck文件时
		if(uqwCurrentPos.qwValue >= uqdwMaxPckSize.qwValue){
			//dwAddressPkx = dwCurrentPos - dwPckSize;

			if(!WriteFile(hFile2, buffer, dwBytesToWrite, &dwFileBytesWrote, NULL))
			{
				return 0;
			}

		}else{
			//UNQWORD	uqwReadEndAT;
			QWORD qwReadEndAT = uqwCurrentPos.qwValue + dwFileBytesWrote;

			//当Read的块全在文件pck内时
			if(qwReadEndAT < uqdwMaxPckSize.qwValue){

				if(!WriteFile(hFile, buffer, dwBytesToWrite, &dwFileBytesWrote, NULL))
				{
					return 0;
				}

			}else{

				//当Read的块在文件pck内和pkx内
				//写pck
				DWORD dwWriteInPck = (DWORD)(uqdwMaxPckSize.qwValue - uqwCurrentPos.qwValue);
				DWORD dwWriteInPkx = dwBytesToWrite - dwWriteInPck;

				if(!WriteFile(hFile, buffer, dwWriteInPck, &dwFileBytesWrote, NULL))
				{
					return 0;
				}

				dwWriteInPck = dwFileBytesWrote;
				//写pkx
				if(!WriteFile(hFile2, ((LPBYTE)buffer) + dwWriteInPck, dwWriteInPkx, &dwFileBytesWrote, NULL))
				{
					return 0;
				}

				dwFileBytesWrote += dwWriteInPck;
			}
		}

		uqwCurrentPos.qwValue += dwFileBytesWrote;
	}else
#endif
	{

		if(!WriteFile(hFile, buffer, dwBytesToWrite, &dwFileBytesWrote, NULL))
		{
			return 0;
		}
	}
	return dwFileBytesWrote;
}

