#pragma once

typedef unsigned __int64	QWORD;

class CPckClassFileDisk
{
public:
	CPckClassFileDisk();
	~CPckClassFileDisk();

	//压缩重建、压缩写入空间不够时扩展空间
	static BOOL		IsNeedExpandWritingFile(
		CMapViewFileWrite *lpWrite,
		QWORD dwWritingAddressPointer,
		QWORD dwFileSizeToWrite,
		QWORD &dwExpectedTotalCompressedFileSize);

	//重建pck
	static QWORD	GetPckFilesizeRebuild(LPCTSTR lpszFilename, QWORD qwPckFilesize);

	//重命名时需要的文件的大小
	static QWORD	GetPckFilesizeRename(LPCTSTR lpszFilename, QWORD qwCurrentPckFilesize);

	//添加和新建文件时，写入的pck文件大小
	static QWORD	GetPckFilesizeByCompressed(LPCSTR lpszFilename, QWORD qwToCompressFilesize, QWORD qwCurrentPckFilesize);
	static QWORD	GetPckFilesizeByCompressed(LPCWSTR lpszFilename, QWORD qwToCompressFilesize, QWORD qwCurrentPckFilesize);
private:
	template <typename T>
	static void GetDriverNameFromFilename(const T* lpszFilename, T lpszDiskName[]);
	static QWORD	GetPckFilesizeByCompressed(QWORD qwDiskFreeSpace, QWORD qwToAddSpace, QWORD qwCurrentPckFilesize);
};

