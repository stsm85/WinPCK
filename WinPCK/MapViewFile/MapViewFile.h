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

#include <windows.h>
#include <assert.h>

#define TEST_T 1

#if _MSC_VER >= 1400
#pragma warning ( disable : 4996 )
#endif

#define ENABLE_PCK_PKX_FILE 1

#if !defined(_MAPVIEWFILE_H_)
#define _MAPVIEWFILE_H_

typedef unsigned __int64	QWORD;

typedef union _QWORD{
	QWORD qwValue;
	LONGLONG llwValue;
	struct {
		DWORD dwValue;
		DWORD dwValueHigh;
	};
	struct {
		LONG lValue;
		LONG lValueHigh;
	};
}UNQWORD, *LPUNQWORD;

#define PATH_LOCAL_PREFIX			"\\\\?\\"
#define PATH_UNC_PREFIX				"\\\\?\\UNC"

#define PATHW_LOCAL_PREFIX			L"\\\\?\\"
#define PATHW_UNC_PREFIX			L"\\\\?\\UNC"

#define PATH_LOCAL_PREFIX_LEN		4
#define PATH_UNC_PREFIX_LEN			7

//#ifdef UNICODE
//#define tcscpy_s	wcscpy_s
//#define	tcslen		wcslen
//#else
//#define tcscpy_s	strcpy_s
//#define	tcslen		strlen
//#endif

class CMapViewFile
{
public:
	CMapViewFile();
	virtual ~CMapViewFile();


	LPBYTE	View(QWORD dwAddress, DWORD dwSize = 0);
	virtual LPBYTE	ReView(QWORD dwAddress, DWORD dwSize = 0);

	virtual void	SetFilePointer(QWORD lDistanceToMove, DWORD dwMoveMethod = FILE_BEGIN);

	virtual DWORD	Read(LPVOID buffer, DWORD dwBytesToRead);

	QWORD	GetFileSize();

	void	UnmapView();
	void	UnMaping();
	void	clear();

#if ENABLE_PCK_PKX_FILE

	BOOL	SetPckPackSize(QWORD qwPckSize);
#endif

protected:

	
	BOOL isWinNt();
	void MakeUnlimitedPath(LPWSTR _dst, LPCWSTR	_src, size_t size);
	void MakeUnlimitedPath(LPSTR _dst, LPCSTR _src, size_t size);

#if ENABLE_PCK_PKX_FILE
	void GetPkxName(LPSTR dst, LPCSTR src);
	void GetPkxName(LPWSTR dst, LPCWSTR src);
#endif

	LPBYTE	ViewReal(LPVOID & lpMapAddress, HANDLE hFileMapping, QWORD qwAddress, DWORD dwSize);

public:

	LPBYTE	lpTargetBuffer;

protected:

	HANDLE	hFile;
	HANDLE	hFileMapping;
	LPVOID	lpMapAddress;

#if ENABLE_PCK_PKX_FILE

	HANDLE	hFile2;
	HANDLE	hFileMapping2;
	LPVOID	lpMapAddress2;

	UNQWORD	dwPckSize, dwPkxSize;
	UNQWORD	uqdwMaxPckSize;				//pck文件的最大值

	UNQWORD	uqwFullSize;				//pck+pkx文件的大小，以PCKHEAD中的dwPckSize为准

	BOOL	IsPckFile, hasPkx;

	BOOL	isCrossView;	//是否是跨文件的VIEW
	LPBYTE	lpCrossBuffer, lpCrossAddressPck;

	DWORD	dwViewSizePck, dwViewSizePkx;

	//char	m_szPckFileName[MAX_PATH];
	//wchar_t	m_tszPckFileName[MAX_PATH];

	char	m_szPkxFileName[MAX_PATH];
	wchar_t	m_tszPkxFileName[MAX_PATH];
#endif

	UNQWORD	uqwCurrentPos;		//当前文件指针位置

	BOOL	isWriteMode;
};



class CMapViewFileRead : public CMapViewFile
{
public:
	CMapViewFileRead();
	virtual ~CMapViewFileRead();

#if ENABLE_PCK_PKX_FILE

	BOOL	OpenPck(LPCSTR lpszFilename);
	BOOL	OpenPck(LPCWSTR lpszFilename);
#endif

	BOOL	Open(LPCSTR lpszFilename);
	BOOL	Open(LPCWSTR lpszFilename);

	BOOL	Mapping(LPCSTR lpszNamespace);

protected:


};


class CMapViewFileWrite : public CMapViewFile
{
public:
	CMapViewFileWrite(DWORD);
	virtual ~CMapViewFileWrite();
	
#if ENABLE_PCK_PKX_FILE
	//void	SetMaxSinglePckSize(QWORD qwMaxPckSize);
	BOOL	OpenPck(LPCSTR lpszFilename, DWORD dwCreationDisposition);
	BOOL	OpenPck(LPCWSTR lpszFilename, DWORD dwCreationDisposition);
#endif

	BOOL	Open(LPCSTR lpszFilename, DWORD dwCreationDisposition);
	BOOL	Open(LPCWSTR lpszFilename, DWORD dwCreationDisposition);

	BOOL	Mapping(LPCSTR lpszNamespace, QWORD dwMaxSize);
	void	SetEndOfFile();

	DWORD	Write(LPVOID buffer, DWORD dwBytesToWrite);

	//BOOL	FlushViewOfFile();

protected:

#if ENABLE_PCK_PKX_FILE
	void	OpenPkx(LPCSTR lpszFilename, DWORD dwCreationDisposition);
	void	OpenPkx(LPCWSTR lpszFilename, DWORD dwCreationDisposition);
#endif

public:

protected:
#if ENABLE_PCK_PKX_FILE
	//一个大文件需要分块成2个小文件时，分块的大小
	QWORD	m_Max_PckFile_Size;
#endif

};

#endif //_MAPVIEWFILE_H_