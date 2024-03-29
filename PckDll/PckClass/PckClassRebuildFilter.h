#pragma once
#include "PckClassLog.h"
#include "PckStructs.h"

class CPckClassRebuildFilter
{
private:
	enum class SCRIPTOP { OP_CheckFile, OP_Protect, OP_Delete, OP_Rmdir, OP_Unknown };

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

	BOOL	ApplyScript(const wchar_t * lpszScriptFile, LPPCK_PATH_NODE lpRootNode);
	BOOL	TestScript(const wchar_t * lpszScriptFile);

	void	StripModelTexture(LPPCKINDEXTABLE lpPckIndexHead, DWORD dwFileCount, LPPCK_PATH_NODE lpRootNode, LPCWSTR lpszPckFilename);
	

private:
	const wchar_t *szOperators =
		L"chkfile;"
		 "protect;"
		 "delete;;"
		 "rmdir;;;";

	std::wstring m_stored_Script;
	std::vector<std::wstring_view>		m_ScriptLines;
	std::vector<FILEOP>		m_FirstFileOp;
	std::vector<LPPCKINDEXTABLE>	m_EditedNode;

	BOOL	OpenScriptFileAndConvBufToUcs2(const wchar_t * lpszScriptFile);

	BOOL	ApplyScript2IndexList(LPPCK_PATH_NODE lpRootNode);
	void	MarkFilterFlagToNode(LPPCK_PATH_NODE lpNode, SCRIPTOP op);
	void	MarkFilterFlagToFileIndex(LPPCKINDEXTABLE	lpPckIndexTable, SCRIPTOP op);
	LPPCK_PATH_NODE LocationFileIndex(LPWSTR *lpszPaths, LPPCK_PATH_NODE lpNode);
	void	SepratePaths(FILEOP * pFileOp);
	BOOL	ParseOneLine(FILEOP * pFileOp, const std::wstring& lpszLine);
	BOOL	ParseScript(const wchar_t * lpszScriptFile);

	BOOL	Apply(LPPCK_PATH_NODE lpRootNode);
	BOOL	ModelTextureCheck(LPCWSTR lpszFilename);
	void	ResetRebuildFilterInIndexList();

};

