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

#include "PckStructs.h"
#include "PckClassLog.h"
#include <vector>

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
	void	Reset(uint32_t dwUIProgressUpper = 1);
#pragma region 打开关闭文件

	void	Close();
public:

	BOOL	Open(const wchar_t * lpszFile);

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
	static uint32_t	GetVersionCount();
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
	uint32_t			GetPckFileCount();
	//实际文件大小
	uint64_t			GetPckSize();
	uint64_t			GetPckDataAreaSize();
	uint64_t			GetPckRedundancyDataSize();

	static uint64_t		GetFileSizeInEntry(LPCENTRY lpFileEntry);
	static uint64_t		GetCompressedSizeInEntry(LPCENTRY lpFileEntry);
	static uint32_t		GetFoldersCountInEntry(LPCENTRY lpFileEntry);
	static uint32_t		GetFilesCountInEntry(LPCENTRY lpFileEntry);

	static size_t		GetFilelenBytesOfEntry(LPCENTRY lpFileEntry);
	static size_t		GetFilelenLeftBytesOfEntry(LPCENTRY lpFileEntry);

	static uint64_t		GetFileOffset(LPCENTRY lpFileEntry);

	//设置附加信息
	const char*			GetAdditionalInfo();
	static uint32_t		GetAdditionalInfoMaxSize();
	BOOL				SetAdditionalInfo(LPCSTR lpszAdditionalInfo);

	//是否是支持更新的文件
	BOOL				isSupportAddFileToPck();

#pragma endregion

	//打开、关闭、复原等事件注册
	static void		regMsgFeedback(void* pTag, FeedbackCallback _FeedbackCallBack);
	static int		DefaultFeedbackCallback(void* pTag, int32_t eventId, size_t wParam, ssize_t lParam);

#pragma region 查询及目录浏览
private:
	static void		DefaultShowFilelistCallback(void* _in_param, int sn, LPCWSTR lpszFilename, int entry_type, uint64_t qwFileSize, uint64_t qwFileSizeCompressed, void* fileEntry);

public:
	uint32_t		SearchByName(LPCWSTR lpszSearchString, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);
	static uint32_t	ListByNode(LPCENTRY lpFileEntry, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);

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
	uint32_t			getMTMemoryUsed();
	//内存值
	void			setMTMaxMemory(uint32_t dwMTMaxMemory);
	uint32_t			getMTMaxMemory();

	//最大内存
	static uint32_t	getMaxMemoryAllowed();

#pragma endregion

#pragma region 线程数
	//线程数
	uint32_t	getMaxThread();
	void	setMaxThread(uint32_t dwThread);
	//线程默认参数
	static uint32_t	getMaxThreadUpperLimit();
#pragma endregion

#pragma region 压缩等级

	//压缩等级
	uint32_t	getCompressLevel();
	void	setCompressLevel(uint32_t dwCompressLevel = Z_DEFAULT_COMPRESS_LEVEL);

	//压缩等级默认参数
	static uint32_t	getMaxCompressLevel();
	static uint32_t	getDefaultCompressLevel();
#pragma endregion

#pragma region 进度相关

	uint32_t	getUIProgress();
	void	setUIProgress(uint32_t dwUIProgress);
	uint32_t	getUIProgressUpper();

#pragma endregion

#pragma region 打印添加/新增文件结果

	uint32_t	GetUpdateResult_OldFileCount();
	uint32_t	GetUpdateResult_PrepareToAddFileCount();
	uint32_t	GetUpdateResult_ChangedFileCount();
	uint32_t	GetUpdateResult_DuplicateFileCount();
	uint32_t	GetUpdateResult_FinalFileCount();

#pragma endregion

private:

	FMTPCK	GetPckTypeFromFilename(const wchar_t * lpszFile);

	LPPCK_PATH_NODE				m_lpPckRootNode;
	std::vector<std::wstring>	lpszFilePathToAdd;	//对添加多个文件时提供数据

	std::wstring				szUpdateResultString;

	PCK_RUNTIME_PARAMS			cParams;
	CPckClass					*m_lpClassPck;

	//格式
	FMTPCK						m_emunFileFormat;

	static FeedbackCallback		pFeedbackCallBack;
	static void*				pTag;

};