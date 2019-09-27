#pragma once

typedef unsigned __int64	QWORD;
#include "PckStructs.h"
#include "PckClassLog.h"
#include "MapViewFileMultiPck.h"
#include "PckClassBaseFeatures.h"


class CPckClassFileDisk :
	protected virtual CPckClassBaseFeatures
{
public:
	CPckClassFileDisk();
	~CPckClassFileDisk();

protected:
	BOOL		MakeFolderExist(const wchar_t* lpszDirectory);
private:
	BOOL		MakeFolderExistInternal(const wchar_t* lpszDirectory);
protected:
	BOOL		EnumAllFilesByPathList(const vector<wstring> &lpszFilePath, DWORD &_out_FileCount, QWORD &_out_TotalFileSize, vector<FILES_TO_COMPRESS> *lpFileLinkList);

	//重建pck时需要的磁盘空间（目标文件名，待重建的PCK文件大小）
	QWORD	GetPckFilesizeRebuild(const wchar_t * lpszFilename, QWORD qwPckFilesize);

	//重命名时需要的文件的大小
	QWORD	GetPckFilesizeRename(const wchar_t * lpszFilename, QWORD qwCurrentPckFilesize);

	//添加和新建文件时，写入的pck文件大小
	QWORD	GetPckFilesizeByCompressed(const char* lpszFilename, QWORD qwToCompressFilesize, QWORD qwCurrentPckFilesize);
	QWORD	GetPckFilesizeByCompressed(const wchar_t * lpszFilename, QWORD qwToCompressFilesize, QWORD qwCurrentPckFilesize);

private:

	//qwCurrentPckFilesize为已经存在的文件大小，qwToAddSpace是需要扩大的大小，返回值为（qwCurrentPckFilesize + 可以再扩大的最大大小）
	QWORD	GetPckFilesizeByCompressed(QWORD qwDiskFreeSpace, QWORD qwToAddSpace, QWORD qwCurrentPckFilesize);
	//遍历文件夹
	VOID	EnumFile(LPWSTR szFilename, BOOL IsPatition, DWORD &dwFileCount, vector<FILES_TO_COMPRESS> *lpFileLinkList, QWORD &qwTotalFileSize, size_t nLen);

};

