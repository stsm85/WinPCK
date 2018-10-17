//////////////////////////////////////////////////////////////////////
// CPckControlCenter.h: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 头文件,界面与PCK类的数据交互，控制中心
//
// 此程序由 李秋枫/stsm/liqf 编写，pck结构参考若水的pck结构.txt，并
// 参考了其易语言代码中并于读pck文件列表的部分
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.10.10
//////////////////////////////////////////////////////////////////////

#pragma once

#include "globals.h"
#include "PckHeader.h"
#include "PckClassLog.h"


class CPckClass;

class CPckControlCenter
{
	//函数
public:
	CPckControlCenter();
	virtual ~CPckControlCenter();

	LPPCK_RUNTIME_PARAMS	GetParams();

	void	setMainWnd(HWND _hWnd);
	void	init();
	void	Reset(DWORD dwUIProgressUpper = 1);

	void	New();
private:
	BOOL	Open(LPCTSTR lpszFile, BOOL isOpenAfterRestore);
public:
	BOOL	Open(LPCTSTR lpszFile);
	void	Close();

	int		GetPckVersion();
	void	SetPckVersion(int verID);

	//获取当前配置名称
	LPCTSTR	GetCurrentVersionName();

	LPCTSTR	GetSaveDlgFilterString();


	//界面交互
	//当前操作的列表的index
	CONST INT GetListCurrentHotItem();
	void	SetListCurrentHotItem(int _val);
	//列表显示模式 ，文件夹显示or 搜索显示模式
	CONST BOOL GetListInSearchMode();
	void	SetListInSearchMode(BOOL _val);


	///pck 类的交互
	CONST	LPPCKINDEXTABLE		GetPckIndexTable();


	BOOL		IsValidPck();

	//获取node路径
	BOOL		GetCurrentNodeString(wchar_t* szCurrentNodePathString, LPPCK_PATH_NODE lpNode);

	//获取文件数
	DWORD		GetPckFileCount();

	QWORD		GetPckSize();
	QWORD		GetPckDataAreaSize();
	QWORD		GetPckRedundancyDataSize();

	//预览文件
	BOOL		GetSingleFileData(LPVOID lpvoidFileRead, LPPCKINDEXTABLE lpPckFileIndexTable, char *buffer, size_t sizeOfBuffer = 0);

	//设置附加信息
	char*		GetAdditionalInfo();
	BOOL		SetAdditionalInfo();

	BOOL		RenameIndex(LPPCK_PATH_NODE lpNode, wchar_t* lpszReplaceString);
	BOOL		RenameIndex(LPPCKINDEXTABLE lpIndex, wchar_t* lpszReplaceString);

	//重命名一个节点
	BOOL		RenameNode(LPPCK_PATH_NODE lpNode, wchar_t* lpszReplaceString);

	//解压文件
	BOOL		ExtractFiles(LPPCKINDEXTABLE *lpIndexToExtract, int nFileCount);
	BOOL		ExtractFiles(LPPCK_PATH_NODE *lpNodeToExtract, int nFileCount);

	//重置PCK的压缩参数
	void	ResetCompressor();

	//重建pck文件
	BOOL	ParseScript(LPCTSTR lpszScriptFile);
	BOOL	RebuildPckFile(LPTSTR szRebuildPckFile, BOOL bUseRecompress);

	//新建、更新pck文件
	BOOL	UpdatePckFile(LPCTSTR szPckFile, const vector<tstring> &lpszFilePath, const LPPCK_PATH_NODE lpNodeToInsert);

	//是否是支持更新的文件
	BOOL	isSupportAddFileToPck();

	//重命名文件
	BOOL	RenameFilename();

	//删除一个节点
	VOID	DeleteNode(LPPCKINDEXTABLE lpNode);
	VOID	DeleteNode(LPPCK_PATH_NODE lpNode);

private:

	FMTPCK	GetPckTypeFromFilename(LPCTSTR lpszFile);

	void	CreateRestoreData();
	BOOL	RestoreData(LPCTSTR lpszFile, FMTPCK emunFileFormat);
	void	DeleteRestoreData();

public:


	LPPCK_PATH_NODE				m_lpPckRootNode;

private:

	BOOL						m_isSearchMode;

	PCK_RUNTIME_PARAMS			cParams;
	CPckClass					*m_lpClassPck;
	CPckClassLog				m_PckLog;

	RESTORE_INFOS				m_RestoreInfomation;
	//LOG
	HWND						m_hWndMain;

	//格式
	FMTPCK						m_emunFileFormat;

};