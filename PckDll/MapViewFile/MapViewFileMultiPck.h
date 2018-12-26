#pragma once

#include "MapViewFileMulti.h"

#ifdef _DEBUG
#define _TEST_MAX_PCK_CELL 0
#else
#define _TEST_MAX_PCK_CELL 0
#endif

#define ID_PCK	0
#define ID_PKX	1
#define ID_PKG	2
#define ID_END	3


class CMapViewFileMultiPck
{
public:
	CMapViewFileMultiPck();
	~CMapViewFileMultiPck();

	//BOOL	CheckPckPackSize(QWORD qwPckSize);
	
protected:

	void GetPkXName(LPSTR dst, LPCSTR src, int _pckid);
	void GetPkXName(LPWSTR dst, LPCWSTR src, int _pckid);

	char	m_szPckFileName[ID_END][MAX_PATH];
	wchar_t	m_tszPckFileName[ID_END][MAX_PATH];

	UNQWORD m_uqwPckSize[ID_END];
	//真实文件大小
	UNQWORD m_uqwPckFileSize;
	//PCK头结构中的文件大小
	UNQWORD m_uqwPckStructSize;

private:



};


class CMapViewFileMultiPckRead : 
	public CMapViewFileMultiRead,
	public CMapViewFileMultiPck
{
public:
	CMapViewFileMultiPckRead();
	~CMapViewFileMultiPckRead();

	BOOL OpenPck(LPCSTR lpszFilename);
	BOOL OpenPck(LPCWSTR lpszFilename);

	BOOL	OpenPckAndMappingRead(LPCSTR lpFileName);
	BOOL	OpenPckAndMappingRead(LPCWSTR lpFileName);

	//LPBYTE OpenMappingAndViewAllRead(LPCSTR lpFileName);
	//LPBYTE OpenMappingAndViewAllRead(LPCWSTR lpFileName);

	QWORD	GetFileSize();

private:

};

class CMapViewFileMultiPckWrite:
	public CMapViewFileMultiWrite,
	public CMapViewFileMultiPck
{
public:
	CMapViewFileMultiPckWrite(QWORD qwMaxPckSize);
	~CMapViewFileMultiPckWrite();

	BOOL OpenPck(LPCSTR lpszFilename, DWORD dwCreationDisposition, BOOL isNTFSSparseFile = FALSE);
	BOOL OpenPck(LPCWSTR lpszFilename, DWORD dwCreationDisposition, BOOL isNTFSSparseFile = FALSE);

	BOOL	OpenPckAndMappingWrite(LPCSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap, BOOL isNTFSSparseFile = FALSE);
	BOOL	OpenPckAndMappingWrite(LPCWSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap, BOOL isNTFSSparseFile = FALSE);

private:

	QWORD	m_Max_PckFile_Size;
};
