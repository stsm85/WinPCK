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

#include "pck_dependencies.h"

typedef struct _PCK_PATH_NODE * LPPCK_PATH_NODE;
typedef struct _PCK_RUNTIME_PARAMS * LPPCK_RUNTIME_PARAMS;

#if 0
#ifdef _WINDLL	//.dll
#define EXPORT_CLASS _declspec(dllexport) 
#elif defined(_DLL)	//.exe
#define EXPORT_CLASS _declspec(dllimport)
#else	//other
#define EXPORT_CLASS 
#endif
#else
#define EXPORT_CLASS 
#endif

class CPckClass;
class CPckClassLog;

class EXPORT_CLASS CPckControlCenter
{
	//函数
public:
	CPckControlCenter();
	virtual ~CPckControlCenter();

	void	New();

private:

	void	init();
	void	uninit();
	void	Reset(DWORD dwUIProgressUpper = 1);
#pragma region 打开关闭文件

	//BOOL	Open(LPCTSTR lpszFile, BOOL isOpenAfterRestore);
	void	Close();
public:

	BOOL	Open(LPCTSTR lpszFile);

#pragma endregion

#pragma region 重命名节点

	//重命名一个节点
	BOOL	RenameEntry(LPENTRY lpFileEntry, LPCWSTR lpszReplaceString);
	//提交
	BOOL	RenameSubmit();

#pragma endregion

#pragma region 预览解压文件

	//预览文件
	BOOL		GetSingleFileData(LPCENTRY lpFileEntry, char *buffer, size_t sizeOfBuffer = 0);

	//解压文件
	BOOL		ExtractFiles(const PCK_UNIFIED_FILE_ENTRY **lpFileEntryArray, int nEntryCount, LPCWSTR lpszDestDirectory);
	BOOL		ExtractAllFiles(LPCWSTR lpszDestDirectory);

#pragma endregion

#pragma region 重建pck文件
	//重建pck文件
	BOOL	TestScript(LPCWSTR lpszScriptFile);
	BOOL	RebuildPckFile(LPCWSTR lpszScriptFile, LPCWSTR szRebuildPckFile, BOOL bUseRecompress);

#pragma endregion

#pragma region 游戏精简
	BOOL	StripPck(LPCWSTR lpszStripedPckFile, int flag);
#pragma endregion



#pragma region create or update pck file

	//多个文件列表进行压缩时使用的vector操作
	void	StringArrayReset();
	void	StringArrayAppend(LPCWSTR lpszFilePath);
	BOOL	UpdatePckFileSubmit(LPCWSTR szPckFile, LPCENTRY lpFileEntry);

#pragma endregion

#pragma region 删除节点
	//删除一个节点
	BOOL	DeleteEntry(LPCENTRY lpFileEntry);
	//提交
	BOOL	DeleteEntrySubmit();

#pragma endregion


#pragma region 版本相关

	int		GetPckVersion();
	BOOL	SetPckVersion(int verID);
	//获取当前配置名称
	LPCWSTR	GetCurrentVersionName();
	static DWORD	GetVersionCount();
	static LPCWSTR	GetVersionNameById(int verID);
	static int		AddVersionAlgorithmId(int AlgorithmId, int Version);

#pragma endregion

#pragma region 节点属性操作

	LPCENTRY GetRootNode();
	//获取node路径
	static BOOL			GetCurrentNodeString(LPWSTR szCurrentNodePathString, LPCENTRY lpFileEntry);
	LPCENTRY			GetFileEntryByPath(LPCWSTR _in_szCurrentNodePathString);
#pragma endregion

#pragma region pck文件属性

	BOOL				IsValidPck();

	//获取文件数
	DWORD				GetPckFileCount();
	//实际文件大小
	QWORD				GetPckSize();
	QWORD				GetPckDataAreaSize();
	QWORD				GetPckRedundancyDataSize();

	static QWORD		GetFileSizeInEntry(LPCENTRY lpFileEntry);
	static QWORD		GetCompressedSizeInEntry(LPCENTRY lpFileEntry);
	static DWORD		GetFoldersCountInEntry(LPCENTRY lpFileEntry);
	static DWORD		GetFilesCountInEntry(LPCENTRY lpFileEntry);

	static size_t		GetFilelenBytesOfEntry(LPCENTRY lpFileEntry);
	static size_t		GetFilelenLeftBytesOfEntry(LPCENTRY lpFileEntry);

	static QWORD		GetFileOffset(LPCENTRY lpFileEntry);

	//设置附加信息
	const char*			GetAdditionalInfo();
	static DWORD		GetAdditionalInfoMaxSize();
	BOOL				SetAdditionalInfo(LPCSTR lpszAdditionalInfo);

	//是否是支持更新的文件
	BOOL				isSupportAddFileToPck();

#pragma endregion

	//打开、关闭、复原等事件注册
	static void		regMsgFeedback(void* pTag, FeedbackCallback _FeedbackCallBack);
	static int		DefaultFeedbackCallback(void* pTag, int eventId, WPARAM wParam, LPARAM lParam);

#pragma region 查询及目录浏览
private:
	static void		DefaultShowFilelistCallback(void* _in_param, int sn, LPCWSTR lpszFilename, int entry_type, unsigned __int64 qwFileSize, unsigned __int64 qwFileSizeCompressed, void* fileEntry);

public:
	DWORD			SearchByName(LPCWSTR lpszSearchString, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);
	static DWORD	ListByNode(LPCENTRY lpFileEntry, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);

#pragma endregion

#pragma region 线程控制
	//线程运行参数
	BOOL			isThreadWorking();
	void			ForceBreakThreadWorking();
#pragma endregion

	//error no
	int				GetLastErrorMsg();
	BOOL			isLastOptSuccess();

#pragma region 内存占用
	//内存占用
	DWORD			getMTMemoryUsed();
	//内存值
	void			setMTMaxMemory(DWORD dwMTMaxMemory);
	DWORD			getMTMaxMemory();

	//最大内存
	static DWORD	getMaxMemoryAllowed();

#pragma endregion

#pragma region 线程数
	//线程数
	DWORD	getMaxThread();
	void	setMaxThread(DWORD dwThread);
	//线程默认参数
	static DWORD	getMaxThreadUpperLimit();
#pragma endregion

#pragma region 压缩等级

	//压缩等级
	DWORD	getCompressLevel();
	void	setCompressLevel(DWORD dwCompressLevel = Z_DEFAULT_COMPRESS_LEVEL);

	//压缩等级默认参数
	static DWORD	getMaxCompressLevel();
	static DWORD	getDefaultCompressLevel();
#pragma endregion

#pragma region 进度相关

	DWORD	getUIProgress();
	void	setUIProgress(DWORD dwUIProgress);
	DWORD	getUIProgressUpper();

#pragma endregion


	//日志
	static void	regShowFunc(ShowLogW _ShowLogW);
	static const char	getLogLevelPrefix(int _loglevel);
	static void	Print(const char chLevel, LPCSTR _fmt, ...);
	static void	Print(const char chLevel, LPCWSTR _fmt, ...);
	static void	Print(const char chLevel, LPCSTR _fmt, va_list ap);
	static void	Print(const char chLevel, LPCWSTR _fmt, va_list ap);
#pragma endregion



#pragma region 打印添加/新增文件结果

	DWORD	GetUpdateResult_OldFileCount();
	DWORD	GetUpdateResult_PrepareToAddFileCount();
	DWORD	GetUpdateResult_ChangedFileCount();
	DWORD	GetUpdateResult_DuplicateFileCount();
	DWORD	GetUpdateResult_FinalFileCount();

#pragma endregion

private:

	FMTPCK	GetPckTypeFromFilename(LPCTSTR lpszFile);

	LPPCK_PATH_NODE				m_lpPckRootNode;
	vector<wstring>				lpszFilePathToAdd;	//对添加多个文件时提供数据

	wstring						szUpdateResultString;

	PCK_RUNTIME_PARAMS			cParams;
	CPckClass					*m_lpClassPck;
	CPckClassLog				*m_lpPckLog;

	//格式
	FMTPCK						m_emunFileFormat;

	static FeedbackCallback		pFeedbackCallBack;
	static void*				pTag;

};