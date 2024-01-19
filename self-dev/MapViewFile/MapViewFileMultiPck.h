#pragma once
#include <array>
#include "MapViewFileMulti.h"

#ifdef _DEBUG
#define _TEST_MAX_PCK_CELL 0
#else
#define _TEST_MAX_PCK_CELL 0
#endif

#if 0
#define ID_PCK	0
#define ID_PKX	1
#define ID_PKG	2
#define ID_END	3
#endif


class CMapViewFileMultiPck
{
public:
	CMapViewFileMultiPck();
	virtual ~CMapViewFileMultiPck() = default;

	void SetPackageCountAndSuffix(const std::vector<std::string>& name);

protected:

	void initialization_filenames(const fs::path& lpszFilename);
	int m_package_count = 1;

	std::vector<std::string> m_fileSuffix;
	std::vector<fs::path> m_szPckFileName;
	std::vector<UNQWORD> m_uqwPckSize{ 0 };

	//真实文件大小
	UNQWORD m_uqwPckFileSize{ 0 };
	//PCK头结构中的文件大小
	UNQWORD m_uqwPckStructSize{ 0 };

private:

};

class CMapViewFileMultiPckRead : 
	public CMapViewFileMultiRead,
	public CMapViewFileMultiPck
{
public:
	CMapViewFileMultiPckRead() = default;
	virtual ~CMapViewFileMultiPckRead() = default;

	BOOL	OpenPck(const fs::path& lpszFilename);
	BOOL	OpenPckAndMappingRead(const fs::path& lpFileName);

	QWORD	GetFileSize() const;

private:

};

class CMapViewFileMultiPckWrite:
	public CMapViewFileMultiWrite,
	public CMapViewFileMultiPck
{
public:
	CMapViewFileMultiPckWrite(QWORD qwMaxPckSize);
	virtual ~CMapViewFileMultiPckWrite() = default;

	BOOL	OpenPck(const fs::path& lpszFilename, DWORD dwCreationDisposition, BOOL isNTFSSparseFile = FALSE);
	BOOL	OpenPckAndMappingWrite(const fs::path&  lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap, BOOL isNTFSSparseFile = FALSE);

private:

	QWORD	m_Max_PckFile_Size;
};
