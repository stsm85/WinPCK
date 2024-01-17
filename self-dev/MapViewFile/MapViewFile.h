#pragma once
//////////////////////////////////////////////////////////////////////
// MapViewFile.h: 用于映射文件视图
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#define TEST_T 1
/*
#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5

#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
*/

#if _MSC_VER >= 1400
#pragma warning ( disable : 4996 )
#endif

#ifdef _WIN64
typedef unsigned __int64  uintptr_t;
#else
typedef unsigned int uintptr_t;
#endif

#if !defined(_MAPVIEWFILE_H_)
#define _MAPVIEWFILE_H_

typedef unsigned __int64	QWORD;

//LARGE_INTEGER
typedef union _QWORD
{
	QWORD qwValue;
	LONGLONG llwValue;
	struct
	{
		DWORD dwValue;
		DWORD dwValueHigh;
	};
	struct
	{
		LONG lValue;
		LONG lValueHigh;
	};
}UNQWORD, *LPUNQWORD;


class CMapViewFile
{
public:
	CMapViewFile();
	virtual ~CMapViewFile() noexcept;

	BOOL FileExists(const fs::path& szName);
	//BOOL FileExists(LPCWSTR szName);

	BOOL	Open(const fs::path& lpszFilename, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes);
	//BOOL	Open(LPCWSTR lpszFilename, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes);

	void	SetFilePointer(QWORD lDistanceToMove, DWORD dwMoveMethod = FILE_BEGIN);
	QWORD	GetFilePointer();

	DWORD	Read(LPVOID buffer, DWORD dwBytesToRead);

	QWORD	GetFileSize();

	virtual LPBYTE	View(QWORD dwAddress, DWORD dwSize) = 0;
	virtual LPBYTE	ReView(LPVOID lpMapAddressOld, QWORD dwAddress, DWORD dwSize) = 0;

	void	UnmapView(LPVOID lpTargetAddress);
	void	UnmapViewAll();
	void	UnMaping();
	void	clear();

	//获取当前文件的磁盘名
	const char*	GetFileDiskName();

	//强制缓存写入磁盘
	virtual BOOL	FlushFileBuffers() = 0;
	
protected:
	////自动生成CreateFileMappingA时所需要的name
	LPCSTR	GenerateMapName();

	//void MakeUnlimitedPath(LPWSTR _dst, LPCWSTR	_src, size_t size);
	//void MakeUnlimitedPath(LPSTR _dst, LPCSTR _src, size_t size);

	void GetDiskNameFromFilename(fs::path& lpszFilename);

	uint8_t*	ViewReal(QWORD qwAddress, DWORD dwSize, DWORD dwDesiredAccess);

protected:

	HANDLE	hFile;
	HANDLE	hFileMapping;
	//LPVOID	lpMapAddress;
	std::vector<const void*> vMapAddress;


	//文件对应的磁盘
	std::string	m_szDisk;

	//用于存放MapName
	char szFileMappingName[32];

private:
/*
创建为NTFS稀疏文件
流程为：CreateFile->SetSparseFile->CreateFileMapping->MapViewOfFile->UnmapViewOfFile->CloseHandle
*/
	virtual void	SetSparseFile() = 0;
};



class CMapViewFileRead : public CMapViewFile
{
public:
	CMapViewFileRead();
	virtual ~CMapViewFileRead() noexcept;

	BOOL	Open(fs::path lpszFilename);
	//BOOL	Open(LPCWSTR lpszFilename);

	BOOL	Mapping();

	virtual LPBYTE	View(QWORD dwAddress, DWORD dwSize) override;
	virtual LPBYTE	ReView(LPVOID lpMapAddressOld, QWORD dwAddress, DWORD dwSize) override;

	BOOL	OpenMappingRead(fs::path lpFileName);
	//BOOL	OpenMappingRead(LPCWSTR lpFileName);

	LPBYTE OpenMappingViewAllRead(fs::path lpFileName);
	//LPBYTE OpenMappingViewAllRead(LPCWSTR lpFileName);

	virtual BOOL	FlushFileBuffers() override { return TRUE; };

protected:

private:
	virtual void	SetSparseFile() override {};

};


class CMapViewFileWrite : public CMapViewFile
{
public:
	CMapViewFileWrite();
	virtual ~CMapViewFileWrite() noexcept;

	BOOL	Open(const fs::path& lpszFilename, DWORD dwCreationDisposition, BOOL isNTFSSparseFile = FALSE);
	//BOOL	Open(LPCWSTR lpszFilename, DWORD dwCreationDisposition, BOOL isNTFSSparseFile = FALSE);

	BOOL	Mapping(QWORD dwMaxSize);

	virtual LPBYTE	View(QWORD dwAddress, DWORD dwSize) override;
	virtual LPBYTE	ReView(LPVOID lpMapAddressOld, QWORD dwAddress, DWORD dwSize) override;

	BOOL	SetEndOfFile();

	DWORD	Write(LPVOID buffer, DWORD dwBytesToWrite);


	BOOL	OpenMappingWrite(fs::path lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap);
	//BOOL	OpenMappingWrite(LPCWSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap);

	virtual BOOL	FlushFileBuffers() override;

private:

	virtual void	SetSparseFile() override;
	//BOOL	Write2(QWORD dwAddress, LPVOID buffer, DWORD dwBytesToWrite);


};

#endif //_MAPVIEWFILE_H_