//////////////////////////////////////////////////////////////////////
// PckClassFileDisk.cpp: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 主要计算写入文件的初始大小，磁盘空间等
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2018.5.31
//////////////////////////////////////////////////////////////////////

//#include "imagehlp.h"
//#pragma comment(lib,"imagehlp.lib")

#include <windows.h>
#include "PckClassFileDisk.h"


//创建时，剩余文件的空间量不够时，添加量
#define	PCK_STEP_ADD_SIZE				(64*1024*1024)
//创建时，剩余文件的空间量小于此值时，扩展数据
#define	PCK_SPACE_DETECT_SIZE			(4*1024*1024)
//创建时，剩余文件的空间量小于此值(PCK_SPACE_DETECT_SIZE)时，扩展数据的值
//#define PCK_RENAME_EXPAND_ADD			(16*1024*1024)

#define ZLIB_AVG_RATIO					0.6

CPckClassFileDisk::CPckClassFileDisk()
{}

CPckClassFileDisk::~CPckClassFileDisk()
{}

#pragma region 磁盘空间大小

template <typename T>
_inline T * __fastcall mystrcpy(T * dest, const T *src)
{
	while((*dest = *src))
		++dest, ++src;
	return dest;
}


template <typename T>
void GetDriverNameFromFilename(const T* lpszFilename, T lpszDiskName[])
{
	lpszDiskName[0] = lpszFilename[0];
	lpszDiskName[1] = lpszFilename[1];
	lpszDiskName[2] = lpszFilename[2];
	lpszDiskName[3] = '\0';
}

//qwCurrentPckFilesize为已经存在的文件大小，qwToAddSpace是需要扩大的大小，返回值为（qwCurrentPckFilesize + 可以再扩大的最大大小）
QWORD CPckClassFileDisk::GetPckFilesizeByCompressed(QWORD qwDiskFreeSpace, QWORD qwToAddSpace, QWORD qwCurrentPckFilesize)
{
	//计算大概需要多大空间qwTotalFileSize
	QWORD	qwTotalFileSizeTemp = qwToAddSpace;

	if(-1 != qwDiskFreeSpace) {

		//如果申请的空间小于磁盘剩余空间，则申请文件空间大小等于剩余磁盘空间
		if(qwDiskFreeSpace < qwTotalFileSizeTemp)
			qwTotalFileSizeTemp = qwDiskFreeSpace;
	}

	return (qwTotalFileSizeTemp + qwCurrentPckFilesize);

}

#if 0
BOOL CPckClassFileDisk::IsNeedExpandWritingFile(
	CMapViewFileMultiPckWrite *lpWrite,
	QWORD dwWritingAddressPointer,
	QWORD dwFileSizeToWrite,
	QWORD &dwExpectedTotalCompressedFileSize)
{
	//判断一下dwAddress的值会不会超过dwTotalFileSizeAfterCompress
	//如果超过，说明文件空间申请的过小，重新申请一下ReCreateFileMapping
	//新文件大小在原来的基础上增加(lpfirstFile->dwFileSize + 1mb) >= 64mb ? (lpfirstFile->dwFileSize + 1mb) :64mb
	//1mb=0x100000
	//64mb=0x4000000
	if((dwWritingAddressPointer + dwFileSizeToWrite + PCK_SPACE_DETECT_SIZE) > dwExpectedTotalCompressedFileSize) {
		
		//打印日志
		//CPckClassLog			m_PckLogFD;

		QWORD qwSizeToExpand = ((dwFileSizeToWrite + PCK_SPACE_DETECT_SIZE) > PCK_STEP_ADD_SIZE ? (dwFileSizeToWrite + PCK_SPACE_DETECT_SIZE) : PCK_STEP_ADD_SIZE);
		ULARGE_INTEGER lpfree;

		if(GetDiskFreeSpaceExA(lpWrite->GetFileDiskName(), NULL, NULL, &lpfree)) {

			qwSizeToExpand = GetPckFilesizeByCompressed(lpfree.QuadPart, qwSizeToExpand, 0);
		} else {
			qwSizeToExpand = GetPckFilesizeByCompressed(-1, qwSizeToExpand, 0);
		}

		if(dwFileSizeToWrite > qwSizeToExpand) {
			//m_PckLogFD.PrintLogW("磁盘空间不足，申请空间：%d，剩余空间：%d", dwFileSizeToWrite, qwSizeToExpand);
			SetErrMsgFlag(PCK_ERR_DISKFULL);
			return FALSE;
		}

		QWORD qwNewExpectedTotalCompressedFileSize = dwExpectedTotalCompressedFileSize + qwSizeToExpand;

		lpWrite->UnMaping();

		if(!lpWrite->Mapping(qwNewExpectedTotalCompressedFileSize)) {

			//m_PckLogFD.PrintLogW(TEXT_VIEWMAP_FAIL);
			SetErrMsgFlag(PCK_ERR_VIEWMAP_FAIL);
			lpWrite->Mapping(dwExpectedTotalCompressedFileSize);
			return FALSE;
		}
		dwExpectedTotalCompressedFileSize = qwNewExpectedTotalCompressedFileSize;
	}
	return TRUE;
}
#endif

//重命名时需要的文件的大小
QWORD CPckClassFileDisk::GetPckFilesizeRename(LPCTSTR lpszFilename, QWORD qwCurrentPckFilesize)
{
	//查看磁盘空间
	TCHAR szDiskName[4];
	ULARGE_INTEGER lpfree;
	GetDriverNameFromFilename(lpszFilename, szDiskName);


	if(GetDiskFreeSpaceEx(szDiskName, NULL, NULL, &lpfree)) {

		return GetPckFilesizeByCompressed(lpfree.QuadPart, PCK_SPACE_DETECT_SIZE, qwCurrentPckFilesize);
	} else {
		return GetPckFilesizeByCompressed(-1, PCK_SPACE_DETECT_SIZE, qwCurrentPckFilesize);
	}

}

QWORD CPckClassFileDisk::GetPckFilesizeRebuild(LPCTSTR lpszFilename, QWORD qwPckFilesize)
{
	//查看磁盘空间
	TCHAR szDiskName[4];
	ULARGE_INTEGER lpfree;
	GetDriverNameFromFilename(lpszFilename, szDiskName);

	QWORD qwToAddSpace = qwPckFilesize + PCK_SPACE_DETECT_SIZE;

	if(GetDiskFreeSpaceEx(szDiskName, NULL, NULL, &lpfree)) {

		return GetPckFilesizeByCompressed(lpfree.QuadPart, qwToAddSpace, 0);
	} else {
		return GetPckFilesizeByCompressed(-1, qwToAddSpace, 0);
	}
}

QWORD CPckClassFileDisk::GetPckFilesizeByCompressed(LPCSTR lpszFilename, QWORD qwToCompressFilesize, QWORD qwCurrentPckFilesize)
{
	//查看磁盘空间
	char szDiskName[4];
	ULARGE_INTEGER lpfree;
	GetDriverNameFromFilename<char>(lpszFilename, szDiskName);

	QWORD qwToAddSpace = qwToCompressFilesize * ZLIB_AVG_RATIO + PCK_SPACE_DETECT_SIZE;

	if(GetDiskFreeSpaceExA(szDiskName, NULL, NULL, &lpfree)) {

		return GetPckFilesizeByCompressed(lpfree.QuadPart, qwToAddSpace, qwCurrentPckFilesize);
	} else {
		return GetPckFilesizeByCompressed(-1, qwToAddSpace, qwCurrentPckFilesize);
	}
}
#pragma endregion
#pragma region Open file,enum
//文件打开、遍历操作等
QWORD CPckClassFileDisk::GetPckFilesizeByCompressed(LPCWSTR lpszFilename, QWORD qwToCompressFilesize, QWORD qwCurrentPckFilesize)
{
	//查看磁盘空间
	wchar_t szDiskName[4];
	ULARGE_INTEGER lpfree;
	GetDriverNameFromFilename<wchar_t>(lpszFilename, szDiskName);

	QWORD qwToAddSpace = qwToCompressFilesize * ZLIB_AVG_RATIO + PCK_SPACE_DETECT_SIZE;

	if(GetDiskFreeSpaceExW(szDiskName, NULL, NULL, &lpfree)) {

		return GetPckFilesizeByCompressed(lpfree.QuadPart, qwToAddSpace, qwCurrentPckFilesize);
	} else {
		return GetPckFilesizeByCompressed(-1, qwToAddSpace, qwCurrentPckFilesize);
	}

}

//////////////////////////以下是程序过程中需要调用的过程///////////////////////////////
VOID CPckClassFileDisk::EnumFile(LPWSTR szFilename, BOOL IsPatition, DWORD &dwFileCount, vector<FILES_TO_COMPRESS> *lpFileLinkList, QWORD &qwTotalFileSize, size_t nLen)
{

	wchar_t		szPath[MAX_PATH], szFile[MAX_PATH];

	size_t nLenBytePath = mystrcpy(szPath, szFilename) - szPath + 1;
	wcscat(szFilename, L"\\*.*");

	HANDLE					hFile;
	WIN32_FIND_DATAW		WFD;

	if((hFile = FindFirstFileW(szFilename, &WFD)) != INVALID_HANDLE_VALUE) {
		if(!IsPatition) {
			FindNextFileW(hFile, &WFD);
			if(!FindNextFileW(hFile, &WFD)) {
				return;
			}
		}

		do {
			if((WFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {

				if((MAX_PATH_PCK_260 - 13) >= mystrcpy(mystrcpy(mystrcpy(szFile, szPath), L"\\"), WFD.cFileName) - szFile) {
					EnumFile(szFile, FALSE, dwFileCount, lpFileLinkList, qwTotalFileSize, nLen);
				}

			} else {

				//if(NULL == pFileinfo)return;
				if(0 != WFD.nFileSizeHigh)continue;

				++dwFileCount;

				lpFileLinkList->push_back(FILES_TO_COMPRESS{ 0 });
				LPFILES_TO_COMPRESS	pFileinfo = &lpFileLinkList->back();

				if(MAX_PATH_PCK_260 < nLenBytePath + wcslen(WFD.cFileName)) {
					mystrcpy(mystrcpy(mystrcpy(pFileinfo->szwFilename, szPath), L"\\"), WFD.cAlternateFileName);
				} else {
					mystrcpy(mystrcpy(mystrcpy(pFileinfo->szwFilename, szPath), L"\\"), WFD.cFileName);
				}

#ifdef _DEBUG
				pFileinfo->id = lpFileLinkList->size();
#endif

				pFileinfo->nFileTitleLen = nLen;
				pFileinfo->nBytesToCopy = MAX_PATH - nLen;

				qwTotalFileSize += (pFileinfo->dwFileSize = WFD.nFileSizeLow);

			}

		} while(FindNextFileW(hFile, &WFD));

		FindClose(hFile);
	}

}

BOOL CPckClassFileDisk::EnumAllFilesByPathList(const vector<tstring> &lpszFilePath, DWORD &_out_FileCount, QWORD &_out_TotalFileSize, vector<FILES_TO_COMPRESS> *lpFileLinkList)
{
	wchar_t		szPathMbsc[MAX_PATH];
	DWORD		dwAppendCount = lpszFilePath.size();

	for(DWORD i = 0; i < dwAppendCount; i++) {

#ifdef UNICODE
		
		//cU2A.GetString(lpszFilePath[i].c_str(), szPathMbsc, MAX_PATH);
		wcscpy_s(szPathMbsc, lpszFilePath[i].c_str());
#else
		CAnsi2Ucs cA2U(CP_ACP);
		cA2U.GetString(lpszFilePath[i].c_str(), szPathMbsc, MAX_PATH);
		//strcpy_s(szPathMbsc, lpszFilePath[i].c_str());
#endif
		size_t nLen = (size_t)(wcsrchr(szPathMbsc, L'\\') - szPathMbsc) + 1;

		if(FILE_ATTRIBUTE_DIRECTORY == (FILE_ATTRIBUTE_DIRECTORY & GetFileAttributesW(szPathMbsc))) {
			//文件夹
			EnumFile(szPathMbsc, FALSE, _out_FileCount, lpFileLinkList, _out_TotalFileSize, nLen);
		} else {

			CMapViewFileRead cFileRead;

			if(!cFileRead.Open(szPathMbsc)) {
				//m_PckLog.PrintLogEL(TEXT_OPENNAME_FAIL, szPathMbsc, __FILE__, __FUNCTION__, __LINE__);

				assert(FALSE);
				return FALSE;
			}

			lpFileLinkList->push_back(FILES_TO_COMPRESS{ 0 });
			LPFILES_TO_COMPRESS	lpfirstFile = &lpFileLinkList->back();

			wcscpy(lpfirstFile->szwFilename, szPathMbsc);

			lpfirstFile->nFileTitleLen = nLen;
			lpfirstFile->nBytesToCopy = MAX_PATH - nLen;

#ifdef _DEBUG
			lpfirstFile->id = lpFileLinkList->size();
#endif
			_out_TotalFileSize += (lpfirstFile->dwFileSize = cFileRead.GetFileSize());
			_out_FileCount++;
		}
	}

	assert(0 != _out_FileCount);
	return TRUE;

}


#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

BOOL CPckClassFileDisk::MakeFolderExistInternal(const wchar_t* lpszDirectory)
{
	wchar_t szUpwardPath[MAX_PATH];
	wcscpy_s(szUpwardPath, lpszDirectory);


	wchar_t *lpLastDir = wcsrchr(szUpwardPath, '\\');

	if (PathFileExistsW(szUpwardPath)) {

		if (PathIsDirectoryW(szUpwardPath)) {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}

	*lpLastDir = 0;

	if (MakeFolderExistInternal(szUpwardPath)) {
		*lpLastDir = '\\';
		if (CreateDirectoryW(szUpwardPath, NULL))
			return TRUE;
	}
	return FALSE;
}

BOOL CPckClassFileDisk::MakeFolderExist(const wchar_t* lpszDirectory)
{

	if ((NULL == lpszDirectory) || (0 == *lpszDirectory))
		return FALSE;

	wchar_t szUpwardPath[MAX_PATH];

	GetFullPathNameW(lpszDirectory, MAX_PATH, szUpwardPath, NULL);
	//wcscpy_s(szUpwardPath, lpszDirectory);
	size_t len = wcslen(szUpwardPath);

	if ('\\' == szUpwardPath[len - 1])
		szUpwardPath[len - 1] = 0;

	return MakeFolderExistInternal(szUpwardPath);
}

#pragma endregion
