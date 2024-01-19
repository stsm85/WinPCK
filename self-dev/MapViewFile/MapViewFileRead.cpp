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
#include "pch_mvf.h"
#include "MapViewFile.h"

CMapViewFileRead::CMapViewFileRead()
{}

CMapViewFileRead::~CMapViewFileRead() noexcept
{}

BOOL CMapViewFileRead::Open(const fs::path& lpszFilename)
{
	return CMapViewFile::Open(lpszFilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS);
}

BOOL CMapViewFileRead::Mapping()
{
	if(NULL == (this->hFileMapping = ::CreateFileMappingA(this->hFile, NULL, PAGE_READONLY, 0, 0, GenerateMapName()))) {
		assert(FALSE);
		return FALSE;
	}

	return TRUE;
}

LPBYTE CMapViewFileRead::View(QWORD qdwAddress, DWORD dwSize)
{
	return this->ViewReal(qdwAddress, dwSize, FILE_MAP_READ);
}


LPBYTE CMapViewFileRead::ReView(LPVOID lpMapAddressOld, QWORD dwAddress, DWORD dwSize)
{
	this->UnmapView(lpMapAddressOld);
	return this->View(dwAddress, dwSize);
}

BOOL CMapViewFileRead::OpenMappingRead(const fs::path& lpFileName)
{
	if(!(this->Open(lpFileName)))
		return FALSE;

	if(!(this->Mapping()))
		return FALSE;

	return TRUE;
}

LPBYTE CMapViewFileRead::OpenMappingViewAllRead(const fs::path& lpFileName)
{
	if(this->OpenMappingRead(lpFileName))
		return this->View(0, 0);
	else
		return nullptr;
}
