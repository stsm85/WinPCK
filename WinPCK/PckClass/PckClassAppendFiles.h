#pragma once
#include "PckClassBodyWriter.h"
#include "PckClassHeadTailWriter.h"
#include "PckClassIndexWriter.h"
#include "PckClassThreadHelper.h"


class CPckClassAppendFiles :
	public virtual CPckClassHeadTailWriter,
	public virtual CPckClassIndexWriter,
	public virtual CPckClassBodyWriter,
	public virtual CPckClassThreadHelper
{
public:
	CPckClassAppendFiles();
	~CPckClassAppendFiles();

	//新建pck文件
	//virtual BOOL	CreatePckFile(LPTSTR szPckFile, LPTSTR szPath);
	//新建、更新pck文件
	virtual BOOL	UpdatePckFile(LPCTSTR szPckFile, vector<tstring> &lpszFilePath, const LPPCK_PATH_NODE lpNodeToInsert);
private:

};

