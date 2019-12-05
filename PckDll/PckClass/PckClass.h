//////////////////////////////////////////////////////////////////////
// PckClass.h: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 头文件
//
// 此程序由 李秋枫/stsm/liqf 编写，pck结构参考若水的pck结构.txt，并
// 参考了其易语言代码中并于读pck文件列表的部分
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#include <assert.h>
#include "pck_default_vars.h"

#include "PckClassVersionDetect.h"
#include "PckClassWriteOperator.h"

#if !defined(_PCKCLASS_H_)
#define _PCKCLASS_H_

class CPckClass : 
	public virtual CPckClassWriteOperator,
	public virtual CPckClassVersionDetect
{
//函数
public:
	CPckClass(LPPCK_RUNTIME_PARAMS inout);
	virtual ~CPckClass();

	virtual BOOL	Init(const wchar_t * szFile);

#pragma region PckClassExtract.cpp

	BOOL	ExtractFiles(const PCK_UNIFIED_FILE_ENTRY **lpFileEntryArray, int nEntryCount, const wchar_t *lpszDestDirectory);
	BOOL	ExtractAllFiles(const wchar_t *lpszDestDirectory);

private:
	//解压文件
	BOOL	ExtractFiles(const PCKINDEXTABLE **lpIndexToExtract, int nFileCount);
	BOOL	ExtractFiles(const PCK_PATH_NODE **lpNodeToExtract, int nFileCount);

public:
	//预览文件
	virtual BOOL	GetSingleFileData(const PCKINDEXTABLE* const lpPckFileIndexTable, char *buffer, size_t sizeOfBuffer = 0);
protected:
	virtual BOOL	GetSingleFileData(LPVOID lpvoidFileRead, const PCKINDEXTABLE* const lpPckFileIndexTable, char *buffer, size_t sizeOfBuffer = 0);
private:
	//PckClassExtract.cpp
	BOOL	StartExtract(LPPCK_PATH_NODE lpNodeToExtract, LPVOID lpMapAddress);
	BOOL	DecompressFile(const wchar_t * lpszFilename, const PCKINDEXTABLE* lpPckFileIndexTable, LPVOID lpvoidFileRead);
#pragma endregion

#pragma region PckClassMount.cpp
protected:
	//PckClass.cpp
	BOOL	MountPckFile(const wchar_t * szFile);
private:
	void	BuildDirTree();
#pragma endregion




};

#endif