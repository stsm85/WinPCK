#pragma once
#include "MapViewFile.h"

#include <vector>

#ifdef _DEBUG
#define FLUSH_SIZE_THRESHOLD	(320 * 1024 * 1024)
#else
#define FLUSH_SIZE_THRESHOLD	(32 * 1024 * 1024)
#endif

class CMapViewFileMulti
{
protected:
	typedef struct _FILE_CELL
	{

		CMapViewFile*	lpMapView;

		QWORD	qwCellAddressBegin;
		QWORD	qwCellAddressEnd;
		QWORD	qwMaxCellSize;
		QWORD	qwCellSize;

		wchar_t	szFilename[MAX_PATH];

	}FILE_CELL, *LPFILE_CELL;

private:
	typedef struct _CROSS_VIEW
	{
		LPBYTE	lpBufferTarget;
		LPBYTE	lpBufferTargetPtr;
		LPVOID	lpMapAddress;
		size_t	size;
		int		iCellID;
	}CROSS_VIEW, *LPCROSS_VIEW;

public:
	CMapViewFileMulti();
	~CMapViewFileMulti();

	void	SetFilePointer(LONGLONG lDistanceToMove, DWORD dwMoveMethod = FILE_BEGIN);
	virtual	QWORD	GetFileSize();

	//void	UnmapView(LPVOID lpTargetAddress);
	void	UnmapViewAll();
	void	UnMaping();
	void	clear();

	DWORD	Read(LPVOID buffer, DWORD dwBytesToRead);

	//获取当前文件的磁盘名
	const char*	GetFileDiskName();

	//返回cell数量和大小
	DWORD	GetCellCount();
	DWORD	GetCellSize();

protected:
	//当前文件指针位置
	UNQWORD	m_uqwCurrentPos;
	//当前已打开文件总大小
	UNQWORD m_uqwFullSize;
	//当前可用文件的最大大小
	UNQWORD m_uqwMaxSize;

	std::vector<FILE_CELL>	m_file_cell;
	std::vector<CROSS_VIEW>	m_cross_view;

	int	GetCellIDByPoint(QWORD qwPoint);

	LPBYTE	View(QWORD dwAddress, DWORD dwSize, BOOL isReadOnly);

private:
	BOOL	BuildCrossViewBuffer(LPBYTE lpCrossBuffer, LPBYTE &lpCrossBufferPtr, int cell_id, QWORD qwAddress, DWORD dwSize, BOOL isReadOnly);

};

class CMapViewFileMultiRead : public CMapViewFileMulti
{
public:
	CMapViewFileMultiRead();
	~CMapViewFileMultiRead();

	BOOL	AddFile(LPCSTR lpszFilename);
	BOOL	AddFile(LPCWSTR lpszFilename);

	BOOL	Mapping();
	LPBYTE	View(QWORD dwAddress, DWORD dwSize);

private:

	BOOL	AddFile(CMapViewFileRead *lpRead, LPCWSTR lpszFilename);

};

class CMapViewFileMultiWrite : public CMapViewFileMulti
{

public:
	CMapViewFileMultiWrite();
	~CMapViewFileMultiWrite();

	BOOL	AddFile(LPCSTR lpszFilename, DWORD dwCreationDisposition, QWORD qwMaxSize, BOOL isNTFSSparseFile = FALSE);
	BOOL	AddFile(LPCWSTR lpszFilename, DWORD dwCreationDisposition, QWORD qwMaxSize, BOOL isNTFSSparseFile = FALSE);

	BOOL	Mapping(QWORD dwMaxSize);
	LPBYTE	View(QWORD dwAddress, DWORD dwSize);

	BOOL	SetEndOfFile();

	DWORD	Write(LPVOID buffer, DWORD dwBytesToWrite);

	BOOL	Write2(QWORD dwAddress, const void* buffer, DWORD dwBytesToWrite);

private:

	BOOL	AddFile(CMapViewFileWrite *lpWrite, QWORD qwMaxSize, LPCWSTR lpszFilename);

	//qwCurrentPckFilesize为已经存在的文件大小，qwToAddSpace是需要扩大的大小，返回值为（qwCurrentPckFilesize + 可以再扩大的最大大小）
	QWORD	GetExpanedPckFilesize(QWORD qwDiskFreeSpace, QWORD qwToAddSpace, QWORD qwCurrentPckFilesize);

	//压缩重建、压缩写入空间不够时扩展空间
	BOOL	IsNeedExpandWritingFile(
		QWORD dwWritingAddressPointer,
		QWORD dwFileSizeToWrite);

	BOOL	ViewAndWrite2(QWORD dwAddress, const void *  buffer, DWORD dwSize);
};


