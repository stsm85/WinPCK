#pragma once
#include "PckClassHeadTailWriter.h"
#include "PckClassIndexWriter.h"
#include "PckClassFileDisk.h"
#include "PckThreadRunner.h"

class CPckClassWriteOperator :
	public virtual CPckClassHeadTailWriter,
	public virtual CPckClassIndexWriter,
	public virtual CPckClassFileDisk
{
public:
	CPckClassWriteOperator();
	~CPckClassWriteOperator();

	friend class CPckThreadRunner;

	//重建pck文件
	virtual BOOL	RebuildPckFile(const wchar_t * lpszScriptFile, const wchar_t * szRebuildPckFile, BOOL bUseRecompress);

protected:
	BOOL	RebuildPckFile(const wchar_t * szRebuildPckFile);
	BOOL	RecompressPckFile(const wchar_t * szRecompressPckFile, int isStripMode = PCK_STRIP_NONE);

public:
#pragma region 游戏精简
	virtual BOOL	StripPck(const wchar_t * lpszStripedPckFile, int flag);
#pragma endregion

#pragma region PckClassAppendFiles.cpp
public:
	//新建、更新pck文件
	virtual BOOL	UpdatePckFile(const wchar_t * szPckFile, const vector<wstring> &lpszFilePath, const PCK_PATH_NODE* lpNodeToInsert);
#pragma endregion
#pragma region PckClassRenamer.cpp

public:
	//重命名文件
	virtual BOOL	RenameFilename();

#pragma endregion

private:
	vector<FILES_TO_COMPRESS>	m_FilesToBeAdded;
};

