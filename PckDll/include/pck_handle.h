#include "pck_default_vars.h"

#ifndef APSTUDIO_READONLY_SYMBOLS
#include "pck_dependencies.h"
#endif

#ifndef WINPCK_DLL_H
#define WINPCK_DLL_H



typedef enum _PCKRTN
{
	WINPCK_OK = 0,			/* 成功 */
	WINPCK_ERROR,			/* 一般错误 */
	WINPCK_WORKING,			/* 任务运行中 */
	WINPCK_INVALIDPCK,		/* 无效的PCK文件或未打开文件 */
	WINPCK_NOTFOUND,
}PCKRTN;

#ifdef __cplusplus
#define EXTERN_C       extern "C"
#define EXTERN_C_START extern "C" {
#define EXTERN_C_END   }
#else
#define EXTERN_C       extern
#define EXTERN_C_START
#define EXTERN_C_END
#endif


#ifdef _WINDLL	//.dll
#define WINPCK_API EXTERN_C _declspec(dllexport)
#elif defined(_DLL)	//.exe
#define WINPCK_API EXTERN_C _declspec(dllimport)
#else	//other
#define WINPCK_API EXTERN_C
#endif

WINPCK_API LPCSTR		pck_version();
WINPCK_API PCKRTN		pck_open(const wchar_t *  lpszFile);
WINPCK_API PCKRTN		pck_close();

//return -1 is invalid 
WINPCK_API int			pck_getVersion();
WINPCK_API PCKRTN		pck_setVersion(int verID);
WINPCK_API uint32_t		pck_getVersionCount();
WINPCK_API LPCWSTR 		pck_getVersionNameById(int verID);
//AlgorithmId can not be -1, Version must be AFPCK_VERSION_202(0x00020002) or AFPCK_VERSION_203(0x00020003), return value is id , PCK_VERSION_INVALID = -1
WINPCK_API int 			pck_addVersionAlgorithmId(int AlgorithmId, int Version);


//获取当前配置名称
WINPCK_API LPCWSTR 		pck_GetCurrentVersionName();

//有效的
WINPCK_API BOOL			pck_IsValidPck();

//获取node路径
WINPCK_API BOOL			pck_getNodeRelativePath(LPWSTR _out_szCurrentNodePathString, LPCENTRY lpFileEntry);
WINPCK_API LPCENTRY		pck_getFileEntryByPath(LPWSTR _in_szCurrentNodePathString);

//节点查询 
WINPCK_API LPCENTRY		pck_getRootNode();

//pck文件大小
WINPCK_API uint64_t		pck_filesize();
//pck有效数据区大小
WINPCK_API uint64_t		pck_file_data_area_size();
//冗余数据大小
WINPCK_API uint64_t		pck_file_redundancy_data_size();
//pck文件数量
WINPCK_API uint32_t		pck_filecount();

//从节点中获取数据
WINPCK_API uint64_t		pck_getFileSizeInEntry(LPCENTRY lpFileEntry);
WINPCK_API uint64_t		pck_getCompressedSizeInEntry(LPCENTRY lpFileEntry);
WINPCK_API uint32_t		pck_getFoldersCountInEntry(LPCENTRY lpFileEntry);
WINPCK_API uint32_t		pck_getFilesCountInEntry(LPCENTRY lpFileEntry);

//当前节点文件名当前长度
WINPCK_API uint32_t		pck_getFilelenBytesOfEntry(LPCENTRY lpFileEntry);
//当前节点文件名最大长度-当前长度
WINPCK_API uint32_t		pck_getFilelenLeftBytesOfEntry(LPCENTRY lpFileEntry);


//返回文件在pck文件中压缩数据的偏移值
WINPCK_API uint64_t		pck_getFileOffset(LPCENTRY lpFileEntry);

//是否是支持更新的文件
WINPCK_API BOOL			pck_isSupportAddFileToPck();

//设置附加信息
WINPCK_API LPCSTR 		pck_GetAdditionalInfo();
WINPCK_API uint32_t		pck_GetAdditionalInfoMaxSize();
//
WINPCK_API PCKRTN		pck_SetAdditionalInfo(LPCSTR  lpszAdditionalInfo);

//重命名一个节点
WINPCK_API PCKRTN		pck_RenameEntry(LPENTRY lpFileEntry, LPCWSTR lpszReplaceString);
//提交
WINPCK_API PCKRTN		pck_RenameSubmit();
//use it like do_RenameEntry(L"x.pck", L"gfx\\test", L"testabc");
WINPCK_API PCKRTN		do_RenameEntry(LPCWSTR  szPckFile, LPCWSTR lpFullPathToRename, LPCWSTR lpszReplaceString);

//预览文件
WINPCK_API PCKRTN		pck_GetSingleFileData(LPCENTRY lpFileEntry, LPSTR _inout_buffer, size_t _in_sizeOfBuffer = 0);

//解压文件
WINPCK_API PCKRTN		pck_ExtractFilesByEntrys(LPCENTRY* lpFileEntryArray, int nEntryCount, LPCWSTR  lpszDestDirectory);
WINPCK_API PCKRTN		pck_ExtractAllFiles(LPCWSTR  lpszDestDirectory);
WINPCK_API PCKRTN		do_ExtractPartFiles(LPCWSTR  lpszSrcPckFile, LPCWSTR  lpszDestDirectory, LPCWSTR  lpszFileToExtract);
WINPCK_API PCKRTN		do_ExtractAllFiles(LPCWSTR  lpszSrcPckFile, LPCWSTR  lpszDestDirectory);

//重建pck文件
WINPCK_API PCKRTN		pck_TestScript(LPCWSTR  lpszScriptFile);
WINPCK_API PCKRTN		pck_RebuildPckFile(LPCWSTR szRebuildPckFile, BOOL bUseRecompress);
//set lpszScriptFile = NULL to disable Script filter function
WINPCK_API PCKRTN		pck_RebuildPckFileWithScript(LPCWSTR  lpszScriptFile, LPCWSTR szRebuildPckFile, BOOL bUseRecompress);
WINPCK_API PCKRTN		do_RebuildPckFileWithScript(LPCWSTR szSrcPckFile, LPCWSTR  lpszScriptFile, LPCWSTR szDstRebuildPckFile, BOOL bUseRecompress, int level = 9);
//游戏精简
WINPCK_API PCKRTN		pck_StripPck(LPCWSTR szStripedPckFile, int flag);
WINPCK_API PCKRTN		do_StripPck(LPCWSTR szSrcPckFile, LPCWSTR szStripedPckFile, int flag, int level);

//新建、更新pck文件
WINPCK_API void			pck_StringArrayReset();
WINPCK_API void			pck_StringArrayAppend(LPCWSTR  lpszFilePath);
WINPCK_API PCKRTN		pck_UpdatePckFileSubmit(LPCWSTR  szPckFile, LPCENTRY lpFileEntry);
//添加文件到pck
WINPCK_API PCKRTN		do_AddFileToPckFile(LPCWSTR  lpszFilePathSrc, LPCWSTR  szDstPckFile, LPCWSTR  lpszPathInPckToAdd, int level = 9);
//创建新的pck文件
WINPCK_API PCKRTN		do_CreatePckFile(LPCWSTR  lpszFilePathSrc, LPCWSTR  szDstPckFile, int _versionId = 0, int level = 9);

//删除一个节点，通过提交应用更改
WINPCK_API PCKRTN		pck_DeleteEntry(LPCENTRY lpFileEntry);
//提交
WINPCK_API PCKRTN		pck_DeleteEntrySubmit();
//通过pck文件名和要删除的文件名删除pck中对应文件，执行后有提交操作, count 是要删除的文件数量
WINPCK_API PCKRTN		do_DeleteFromPck(LPCWSTR  szSrcPckFile, int count, ...);

//查询及目录浏览
//return = searched filecount
WINPCK_API uint32_t		pck_searchByName(LPCWSTR  lpszSearchString, void* _in_param, SHOW_LIST_CALLBACK showListCallback);
WINPCK_API uint32_t		pck_listByNode(LPCENTRY lpFileEntry, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);
WINPCK_API PCKRTN		do_listPathInPck(LPCWSTR  szSrcPckFile, LPCWSTR  lpszListPath, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);

//多线程任务
//yes -1, no - 0
WINPCK_API BOOL			pck_isThreadWorking();
WINPCK_API void			pck_forceBreakThreadWorking();
WINPCK_API BOOL			pck_isLastOptSuccess();
WINPCK_API BOOL			pck_getLastErrorMsg();

//内存占用
WINPCK_API uint32_t		pck_getMTMemoryUsed();
WINPCK_API uint32_t		pck_getMTMaxMemory();
WINPCK_API void			pck_setMTMaxMemory(uint32_t dwMTMaxMemoryInBytes);
WINPCK_API uint32_t		pck_getMaxMemoryAllowed();
//线程数
WINPCK_API uint32_t		pck_getMaxThreadUpperLimit();
WINPCK_API uint32_t		pck_getMaxThread();
WINPCK_API void			pck_setMaxThread(uint32_t dwThread);
//压缩等级
WINPCK_API uint32_t		pck_getMaxCompressLevel();
WINPCK_API uint32_t		pck_getDefaultCompressLevel();
WINPCK_API uint32_t		pck_getCompressLevel();
WINPCK_API void			pck_setCompressLevel(uint32_t dwCompressLevel);
//进度
WINPCK_API uint32_t		pck_getUIProgress();
WINPCK_API void			pck_setUIProgress(uint32_t dwUIProgress);
WINPCK_API uint32_t		pck_getUIProgressUpper();


//添加/新增文件返回结果清单
WINPCK_API uint32_t		pck_getUpdateResult_OldFileCount();
WINPCK_API uint32_t		pck_getUpdateResult_PrepareToAddFileCount();
WINPCK_API uint32_t		pck_getUpdateResult_ChangedFileCount();
WINPCK_API uint32_t		pck_getUpdateResult_DuplicateFileCount();
WINPCK_API uint32_t		pck_getUpdateResult_FinalFileCount();

//日志
WINPCK_API void			log_regShowFunc(ShowLogW _ShowLogCallBack);

WINPCK_API void			pck_logNA(LPCSTR  _fmt, ...);
WINPCK_API void			pck_logNW(LPCWSTR  _fmt, ...);

WINPCK_API void			pck_logIA(LPCSTR  _fmt, ...);
WINPCK_API void			pck_logIW(LPCWSTR  _fmt, ...);

WINPCK_API void			pck_logWA(LPCSTR  _fmt, ...);
WINPCK_API void			pck_logWW(LPCWSTR  _fmt, ...);

WINPCK_API void			pck_logEA(LPCSTR  _fmt, ...);
WINPCK_API void			pck_logEW(LPCWSTR  _fmt, ...);

WINPCK_API void			pck_logDA(LPCSTR  _fmt, ...);
WINPCK_API void			pck_logDW(LPCWSTR  _fmt, ...);

#ifdef UNICODE
#define pck_logN pck_logNW
#define pck_logI pck_logIW
#define pck_logW pck_logWW
#define pck_logE pck_logEW
#define pck_logD pck_logDW
#else
#define pck_logN pck_logNA
#define pck_logI pck_logIA
#define pck_logW pck_logWA
#define pck_logE pck_logEA
#define pck_logD pck_logDA
#endif

//打开、关闭、复原等事件注册
WINPCK_API void			pck_regMsgFeedback(void* pTag, FeedbackCallback _FeedbackCallBack);

#endif //WINPCK_DLL_H


