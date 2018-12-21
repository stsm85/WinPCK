//////////////////////////////////////////////////////////////////////
// MapViewFileRead.cpp: 用于映射文件视图（读）
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
{}

CMapViewFileRead::~CMapViewFileRead()
{}

BOOL CMapViewFileRead::Open(LPCSTR lpszFilename)
{
	return CMapViewFile::Open(lpszFilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS);
}

BOOL CMapViewFileRead::Open(LPCWSTR lpszFilename)
{
	return CMapViewFile::Open(lpszFilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS);
}

BOOL CMapViewFileRead::Mapping()
{
	if(NULL == (hFileMapping = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, GenerateMapName()))) {
		assert(FALSE);
		return FALSE;
	}

	return TRUE;
}

LPBYTE CMapViewFileRead::View(QWORD qdwAddress, DWORD dwSize)
{
	return ViewReal(qdwAddress, dwSize, FILE_MAP_READ);
}


LPBYTE CMapViewFileRead::ReView(LPVOID lpMapAddressOld, QWORD dwAddress, DWORD dwSize)
{
	UnmapView(lpMapAddressOld);
	return View(dwAddress, dwSize);
}

BOOL CMapViewFileRead::OpenMappingRead(LPCSTR lpFileName)
{
	if(!(Open(lpFileName)))
		return FALSE;

	if(!(Mapping()))
		return FALSE;

	return TRUE;
}

BOOL CMapViewFileRead::OpenMappingRead(LPCWSTR lpFileName)
{
	if(!(Open(lpFileName)))
		return FALSE;

	if(!(Mapping()))
		return FALSE;

	return TRUE;
}

LPBYTE CMapViewFileRead::OpenMappingViewAllRead(LPCSTR lpFileName)
{
	if(OpenMappingRead(lpFileName))
		return View(0, 0);
	else
		return NULL;
}

LPBYTE CMapViewFileRead::OpenMappingViewAllRead(LPCWSTR lpFileName)
{
	if(OpenMappingRead(lpFileName))
		return View(0, 0);
	else
		return NULL;
}