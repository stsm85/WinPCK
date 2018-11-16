#include "pck_dependencies.h"

#ifndef WINPCK_DLL_H
#define WINPCK_DLL_H

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef void *				HANDLE;


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

#ifdef UNICODE
typedef const wchar_t *		LPCTSTR;
typedef wchar_t *			LPTSTR;
#else
typedef const char *		LPCTSTR;
typedef char *				LPTSTR;
#endif

WINPCK_API HANDLE			pck_new();
WINPCK_API void				pck_free(HANDLE handle);

WINPCK_API BOOL				pck_open(HANDLE handle, LPCTSTR lpszFile);
WINPCK_API void				pck_close(HANDLE handle);

//return -1 is invalid 
WINPCK_API int				pck_getVersion(HANDLE handle);
WINPCK_API BOOL				pck_setVersion(HANDLE handle, int verID);
WINPCK_API DWORD			pck_getVersionCount();
WINPCK_API const wchar_t*	pck_getVersionNameById(int verID);


//获取当前配置名称
WINPCK_API LPCTSTR			pck_GetCurrentVersionName(HANDLE handle);

//有效的
WINPCK_API BOOL				pck_IsValidPck(HANDLE handle);

//获取node路径
WINPCK_API BOOL		pck_getNodeRelativePath(wchar_t* _out_szCurrentNodePathString, const PCK_UNIFIED_FILE_ENTRY* lpFileEntry);
WINPCK_API const PCK_UNIFIED_FILE_ENTRY*	pck_getFileEntryByPath(HANDLE handle, wchar_t* _in_szCurrentNodePathString);

//节点查询 
WINPCK_API const PCK_UNIFIED_FILE_ENTRY*	pck_getRootNode(HANDLE handle);
//pck_getFirstNode = (pck_getRootNode)->child
WINPCK_API const PCK_UNIFIED_FILE_ENTRY*	pck_getFirstNode(HANDLE handle);

//pck文件大小
WINPCK_API QWORD	pck_filesize(HANDLE handle);
//pck有效数据区大小
WINPCK_API QWORD	pck_file_data_area_size(HANDLE handle);
//冗余数据大小
WINPCK_API QWORD	pck_file_redundancy_data_size(HANDLE handle);
//pck文件数量
WINPCK_API DWORD	pck_filecount(HANDLE handle);

//从节点中获取数据
WINPCK_API QWORD	pck_getFileSizeInEntry(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry);
WINPCK_API QWORD	pck_getCompressedSizeInEntry(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry);
WINPCK_API DWORD		pck_getFoldersCountInEntry(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry);
WINPCK_API DWORD		pck_getFilesCountInEntry(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry);

//当前节点文件名当前长度
WINPCK_API DWORD		pck_getFilelenBytesOfEntry(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry);
//当前节点文件名最大长度-当前长度
WINPCK_API DWORD		pck_getFilelenLeftBytesOfEntry(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry);


//返回文件在pck文件中压缩数据的偏移值
WINPCK_API QWORD		pck_getFileOffset(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry);

//是否是支持更新的文件
WINPCK_API BOOL			pck_isSupportAddFileToPck(HANDLE handle);

//设置附加信息
WINPCK_API const char*		pck_GetAdditionalInfo(HANDLE handle);
WINPCK_API DWORD			pck_GetAdditionalInfoMaxSize();
//rtn:success=1
WINPCK_API BOOL				pck_SetAdditionalInfo(HANDLE handle, const char *lpszAdditionalInfo);

//重命名一个节点
WINPCK_API BOOL				pck_RenameEntry(HANDLE handle, PCK_UNIFIED_FILE_ENTRY* lpFileEntry, wchar_t* lpszReplaceString);
//提交
WINPCK_API BOOL				pck_RenameSubmit(HANDLE handle);

//预览文件
WINPCK_API BOOL				pck_GetSingleFileData(HANDLE handle, const PCK_UNIFIED_FILE_ENTRY* lpFileEntry, char *_inout_buffer, size_t _in_sizeOfBuffer = 0);

//解压文件
WINPCK_API BOOL				pck_ExtractFilesByEntrys(HANDLE handle, const PCK_UNIFIED_FILE_ENTRY **lpFileEntryArray, int nEntryCount, const wchar_t *lpszDestDirectory);
WINPCK_API BOOL				pck_ExtractAllFiles(HANDLE handle, const wchar_t *lpszDestDirectory);
WINPCK_API BOOL				do_ExtractPartFiles(LPCTSTR lpszFilePathSrc, const wchar_t *lpszDestDirectory, const wchar_t *lpszFileToExtract);
WINPCK_API BOOL				do_ExtractAllFiles(LPCTSTR lpszFilePathSrc, const wchar_t *lpszDestDirectory);

//重建pck文件
WINPCK_API BOOL	pck_ParseScript(HANDLE handle, LPCTSTR lpszScriptFile);
WINPCK_API BOOL	pck_RebuildPckFile(HANDLE handle, LPTSTR szRebuildPckFile, BOOL bUseRecompress);
WINPCK_API BOOL	pck_RebuildPckFileWithScript(HANDLE handle, LPCTSTR lpszScriptFile, LPTSTR szRebuildPckFile, BOOL bUseRecompress);


//新建、更新pck文件
WINPCK_API void	pck_StringArrayReset(HANDLE handle);
WINPCK_API void	pck_StringArrayAppend(HANDLE handle, LPCTSTR lpszFilePath);
WINPCK_API BOOL	pck_UpdatePckFileSubmit(HANDLE handle, LPCTSTR szPckFile, const PCK_UNIFIED_FILE_ENTRY* lpFileEntry);
//添加文件到pck
WINPCK_API BOOL	do_AddFileToPckFile(LPCTSTR lpszFilePathSrc, LPCTSTR szPckFile, const wchar_t *lpszPathInPckToAdd);
//创建新的pck文件
WINPCK_API BOOL	do_CreatePckFile(LPCTSTR lpszFilePathSrc, LPCTSTR szPckFile, int _versionId = 0);

//删除一个节点
WINPCK_API BOOL	pck_DeleteEntry(HANDLE handle, const PCK_UNIFIED_FILE_ENTRY* lpFileEntry);
//提交
WINPCK_API BOOL pck_DeleteEntrySubmit(HANDLE handle);

//查询及目录浏览
//return = searched filecount
WINPCK_API DWORD pck_searchByName(HANDLE handle, const wchar_t* lpszSearchString, void* _in_param, SHOW_LIST_CALLBACK showListCallback);
WINPCK_API DWORD pck_listByNode(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);
WINPCK_API const PCK_UNIFIED_FILE_ENTRY* pck_getUpwardEntry(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry);
WINPCK_API const PCK_UNIFIED_FILE_ENTRY* pck_getDownwardEntry(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry);

//多线程任务
//yes -1, no - 0
WINPCK_API BOOL		pck_isThreadWorking(HANDLE handle);
WINPCK_API void		pck_forceBreakThreadWorking(HANDLE handle);
WINPCK_API BOOL		pck_isLastOptSuccess(HANDLE handle);
WINPCK_API BOOL		pck_getLastErrorMsg(HANDLE handle);

//内存占用
WINPCK_API DWORD	pck_getMTMemoryUsed(HANDLE handle);
WINPCK_API DWORD	pck_getMTMaxMemory(HANDLE handle);
WINPCK_API void		pck_setMTMaxMemory(HANDLE handle, DWORD dwMTMaxMemoryInMB);
WINPCK_API DWORD	pck_getMaxMemoryAllowed();
//线程数
WINPCK_API DWORD	pck_getMaxThreadUpperLimit();
WINPCK_API DWORD	pck_getMaxThread(HANDLE handle);
WINPCK_API void		pck_setMaxThread(HANDLE handle, DWORD dwThread);
//压缩等级
WINPCK_API DWORD		pck_getMaxCompressLevel();
WINPCK_API DWORD		pck_getDefaultCompressLevel();
WINPCK_API DWORD		pck_getCompressLevel(HANDLE handle);
WINPCK_API void			pck_setCompressLevel(HANDLE handle, DWORD dwCompressLevel);
//进度
WINPCK_API DWORD		pck_getUIProgress(HANDLE handle);
WINPCK_API void			pck_setUIProgress(HANDLE handle, DWORD dwUIProgress);
WINPCK_API DWORD		pck_getUIProgressUpper(HANDLE handle);


//添加/新增文件返回结果清单
WINPCK_API DWORD		pck_getUpdateResult_OldFileCount(HANDLE handle);
WINPCK_API DWORD		pck_getUpdateResult_PrepareToAddFileCount(HANDLE handle);
WINPCK_API DWORD		pck_getUpdateResult_ChangedFileCount(HANDLE handle);
WINPCK_API DWORD		pck_getUpdateResult_DuplicateFileCount(HANDLE handle);
WINPCK_API DWORD		pck_getUpdateResult_FinalFileCount(HANDLE handle);

//日志
WINPCK_API void			log_regShowFunc(ShowLogW _ShowLogCallBack);
WINPCK_API const char	log_getLogLevelPrefix(int _loglevel);
WINPCK_API void			log_PrintA(const char chLevel, const char *_fmt, ...);
WINPCK_API void			log_PrintW(const char chLevel, const wchar_t *_fmt, ...);

#ifdef UNICODE
#define log_Print log_PrintW
#else
#define log_Print log_PrintA
#endif

//打开、关闭、复原等事件注册
WINPCK_API void			pck_regMsgFeedback(void* pTag, FeedbackCallback _FeedbackCallBack);

#endif //WINPCK_DLL_H


