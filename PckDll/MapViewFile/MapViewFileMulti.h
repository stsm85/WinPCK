#pragma once
#include "MapViewFile.h"

#include <vector>
using namespace std;

typedef struct _FILE_CELL
{

	CMapViewFile*	lpMapView;

	QWORD	qwCellAddressBegin;
	QWORD	qwCellAddressEnd;
	QWORD	qwMaxCellSize;
	QWORD	qwCellSize;

	wchar_t	szFilename[MAX_PATH];

}FILE_CELL, *LPFILE_CELL;

typedef struct _CROSS_VIEW
{
	LPBYTE	lpBufferTarget;
	LPBYTE	lpBufferTargetPtr;
	LPVOID	lpMapAddress;
	size_t	size;
	int		iCellID;
}CROSS_VIEW, *LPCROSS_VIEW;



class CMapViewFileMulti
{
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

protected:
	//当前文件指针位置
	UNQWORD	m_uqwCurrentPos;
	//当前已打开文件总大小
	UNQWORD m_uqwFullSize;
	//当前可用文件的最大大小
	UNQWORD m_uqwMaxSize;

	vector<FILE_CELL>	m_file_cell;
	vector<CROSS_VIEW>	m_cross_view;

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

	BOOL	AddFile(LPCSTR lpszFilename, DWORD dwCreationDisposition, QWORD qwMaxSize);
	BOOL	AddFile(LPCWSTR lpszFilename, DWORD dwCreationDisposition, QWORD qwMaxSize);

	BOOL	Mapping(QWORD dwMaxSize);
	LPBYTE	View(QWORD dwAddress, DWORD dwSize);

	BOOL	SetEndOfFile();

	DWORD	Write(LPVOID buffer, DWORD dwBytesToWrite);

private:
	BOOL	AddFile(CMapViewFileWrite *lpWrite, QWORD qwMaxSize, LPCWSTR lpszFilename);
};


