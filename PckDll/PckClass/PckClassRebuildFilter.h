#pragma once
#include "PckClassLog.h"
#include "PckStructs.h"

class CPckClassRebuildFilter
{
private:
	typedef enum { OP_CheckFile = 0, OP_Protect, OP_Delete, OP_Rmdir, OP_Unknown } SCRIPTOP;

	typedef struct _FILEOPS
	{
		SCRIPTOP op;
		wchar_t	szFilename[MAX_PATH];
		wchar_t	szFilenameBuffer[MAX_PATH];		//把路径中的\或/填0，分开的路径名称分别用lpszSepratedPaths指向
		wchar_t*	lpszSepratedPaths[MAX_PATH];
	}FILEOP;

public:
	CPckClassRebuildFilter();
	~CPckClassRebuildFilter();

	BOOL	ApplyScript(LPCTSTR lpszScriptFile, LPPCK_PATH_NODE lpRootNode);
	BOOL	TestScript(LPCTSTR lpszScriptFile);

	void	StripModelTexture(LPPCKINDEXTABLE lpPckIndexHead, DWORD dwFileCount, LPPCK_PATH_NODE lpRootNode, LPCWSTR lpszPckFilename);
	

private:

	CPckClassLog	m_PckLog;
	const wchar_t *szOperators =
		L"chkfile;"
		 "protect;"
		 "delete;;"
		 "rmdir;;;";

	vector<wstring>		m_ScriptLines;
	vector<FILEOP>		m_FirstFileOp;
	vector<LPPCKINDEXTABLE>	m_EditedNode;

	BOOL	OpenScriptFileAndConvBufToUcs2(LPCTSTR lpszScriptFile);

	BOOL	ApplyScript2IndexList(LPPCK_PATH_NODE lpRootNode);
	void	MarkFilterFlagToNode(LPPCK_PATH_NODE lpNode, SCRIPTOP op);
	void	MarkFilterFlagToFileIndex(LPPCKINDEXTABLE	lpPckIndexTable, SCRIPTOP op);
	LPPCK_PATH_NODE LocationFileIndex(LPWSTR *lpszPaths, LPPCK_PATH_NODE lpNode);
	void	SepratePaths(FILEOP * pFileOp);
	BOOL	ParseOneLine(FILEOP * pFileOp, LPCWSTR lpszLine);
	BOOL	ParseScript(LPCTSTR lpszScriptFile);

	BOOL	Apply(LPPCK_PATH_NODE lpRootNode);
	BOOL	ModelTextureCheck(LPCWSTR lpszFilename);
	void	ResetRebuildFilterInIndexList();

};

