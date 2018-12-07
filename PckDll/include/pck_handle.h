#ifndef APSTUDIO_READONLY_SYMBOLS
#include "pck_dependencies.h"
#endif

#ifndef WINPCK_DLL_H
#define WINPCK_DLL_H

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;

typedef const wchar_t * 	CLPWSTR;
typedef wchar_t *			LPWSTR;
typedef const char * 		CLPSTR;
typedef char *				LPSTR;

typedef PCK_UNIFIED_FILE_ENTRY*			LPPUFE;
typedef const PCK_UNIFIED_FILE_ENTRY*	CLPPUFE;

#define WINPCK_VERSION        "1.30.0.1"
#define WINPCK_VERSION_NUMBER  1,30,0,1

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

WINPCK_API CLPSTR		pck_version();
WINPCK_API PCKRTN		pck_open(CLPWSTR  lpszFile);
WINPCK_API PCKRTN		pck_close();

//return -1 is invalid 
WINPCK_API int			pck_getVersion();
WINPCK_API PCKRTN		pck_setVersion(int verID);
WINPCK_API DWORD		pck_getVersionCount();
WINPCK_API CLPWSTR 		pck_getVersionNameById(int verID);
//AlgorithmId can not be -1, Version must be AFPCK_VERSION_202(0x00020002) or AFPCK_VERSION_203(0x00020003), return value is id , PCK_VERSION_INVALID = -1
WINPCK_API int 			pck_addVersionAlgorithmId(int AlgorithmId, int Version);


//获取当前配置名称
WINPCK_API CLPWSTR 		pck_GetCurrentVersionName();

//有效的
WINPCK_API BOOL			pck_IsValidPck();

//获取node路径
WINPCK_API BOOL			pck_getNodeRelativePath(LPWSTR _out_szCurrentNodePathString, CLPPUFE lpFileEntry);
WINPCK_API CLPPUFE		pck_getFileEntryByPath(LPWSTR _in_szCurrentNodePathString);

//节点查询 
WINPCK_API CLPPUFE		pck_getRootNode();

//pck文件大小
WINPCK_API QWORD		pck_filesize();
//pck有效数据区大小
WINPCK_API QWORD		pck_file_data_area_size();
//冗余数据大小
WINPCK_API QWORD		pck_file_redundancy_data_size();
//pck文件数量
WINPCK_API DWORD		pck_filecount();

//从节点中获取数据
WINPCK_API QWORD		pck_getFileSizeInEntry(CLPPUFE lpFileEntry);
WINPCK_API QWORD		pck_getCompressedSizeInEntry(CLPPUFE lpFileEntry);
WINPCK_API DWORD		pck_getFoldersCountInEntry(CLPPUFE lpFileEntry);
WINPCK_API DWORD		pck_getFilesCountInEntry(CLPPUFE lpFileEntry);

//当前节点文件名当前长度
WINPCK_API DWORD		pck_getFilelenBytesOfEntry(CLPPUFE lpFileEntry);
//当前节点文件名最大长度-当前长度
WINPCK_API DWORD		pck_getFilelenLeftBytesOfEntry(CLPPUFE lpFileEntry);


//返回文件在pck文件中压缩数据的偏移值
WINPCK_API QWORD		pck_getFileOffset(CLPPUFE lpFileEntry);

//是否是支持更新的文件
WINPCK_API BOOL			pck_isSupportAddFileToPck();

//设置附加信息
WINPCK_API CLPSTR 		pck_GetAdditionalInfo();
WINPCK_API DWORD		pck_GetAdditionalInfoMaxSize();
//
WINPCK_API PCKRTN		pck_SetAdditionalInfo(CLPSTR  lpszAdditionalInfo);//

//重命名一个节点
WINPCK_API PCKRTN		pck_RenameEntry(LPPUFE lpFileEntry, LPWSTR lpszReplaceString);//
//提交
WINPCK_API PCKRTN		pck_RenameSubmit();

//预览文件
WINPCK_API PCKRTN		pck_GetSingleFileData(CLPPUFE lpFileEntry, LPSTR _inout_buffer, size_t _in_sizeOfBuffer = 0);

//解压文件
WINPCK_API PCKRTN		pck_ExtractFilesByEntrys(CLPPUFE* lpFileEntryArray, int nEntryCount, CLPWSTR  lpszDestDirectory);
WINPCK_API PCKRTN		pck_ExtractAllFiles(CLPWSTR  lpszDestDirectory);
WINPCK_API PCKRTN		do_ExtractPartFiles(CLPWSTR  lpszFilePathSrc, CLPWSTR  lpszDestDirectory, CLPWSTR  lpszFileToExtract);
WINPCK_API PCKRTN		do_ExtractAllFiles(CLPWSTR  lpszFilePathSrc, CLPWSTR  lpszDestDirectory);

//重建pck文件
WINPCK_API PCKRTN		pck_ParseScript(CLPWSTR  lpszScriptFile);
WINPCK_API PCKRTN		pck_RebuildPckFile(CLPWSTR szRebuildPckFile, BOOL bUseRecompress);
//set lpszScriptFile = NULL to disable Script filter function
WINPCK_API PCKRTN		pck_RebuildPckFileWithScript(CLPWSTR  lpszScriptFile, CLPWSTR szRebuildPckFile, BOOL bUseRecompress);
WINPCK_API PCKRTN		do_RebuildPckFileWithScript(CLPWSTR szSrcPckFile, CLPWSTR  lpszScriptFile, CLPWSTR szDstRebuildPckFile, BOOL bUseRecompress, int level = 9);

//新建、更新pck文件
WINPCK_API void			pck_StringArrayReset();
WINPCK_API void			pck_StringArrayAppend(CLPWSTR  lpszFilePath);
WINPCK_API PCKRTN		pck_UpdatePckFileSubmit(CLPWSTR  szPckFile, CLPPUFE lpFileEntry);
//添加文件到pck
WINPCK_API PCKRTN		do_AddFileToPckFile(CLPWSTR  lpszFilePathSrc, CLPWSTR  szDstPckFile, CLPWSTR  lpszPathInPckToAdd);
//创建新的pck文件
WINPCK_API PCKRTN		do_CreatePckFile(CLPWSTR  lpszFilePathSrc, CLPWSTR  szDstPckFile, int _versionId = 0);

//删除一个节点，通过提交应用更改
WINPCK_API PCKRTN		pck_DeleteEntry(CLPPUFE lpFileEntry);
//提交
WINPCK_API PCKRTN		pck_DeleteEntrySubmit();
//通过pck文件名和要删除的文件名删除pck中对应文件，执行后有提交操作, count 是要删除的文件数量
WINPCK_API PCKRTN		do_DeleteFromPck(CLPWSTR  szSrcPckFile, int count, ...);

//查询及目录浏览
//return = searched filecount
WINPCK_API DWORD		pck_searchByName(CLPWSTR  lpszSearchString, void* _in_param, SHOW_LIST_CALLBACK showListCallback);
WINPCK_API DWORD		pck_listByNode(CLPPUFE lpFileEntry, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);
WINPCK_API DWORD		do_listPathInPck(CLPWSTR  szSrcPckFile, CLPWSTR  lpszListPath, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);

//多线程任务
//yes -1, no - 0
WINPCK_API BOOL			pck_isThreadWorking();
WINPCK_API void			pck_forceBreakThreadWorking();
WINPCK_API BOOL			pck_isLastOptSuccess();
WINPCK_API BOOL			pck_getLastErrorMsg();

//内存占用
WINPCK_API DWORD		pck_getMTMemoryUsed();
WINPCK_API DWORD		pck_getMTMaxMemory();
WINPCK_API void			pck_setMTMaxMemory(DWORD dwMTMaxMemoryInMB);
WINPCK_API DWORD		pck_getMaxMemoryAllowed();
//线程数
WINPCK_API DWORD		pck_getMaxThreadUpperLimit();
WINPCK_API DWORD		pck_getMaxThread();
WINPCK_API void			pck_setMaxThread(DWORD dwThread);
//压缩等级
WINPCK_API DWORD		pck_getMaxCompressLevel();
WINPCK_API DWORD		pck_getDefaultCompressLevel();
WINPCK_API DWORD		pck_getCompressLevel();
WINPCK_API void			pck_setCompressLevel(DWORD dwCompressLevel);
//进度
WINPCK_API DWORD		pck_getUIProgress();
WINPCK_API void			pck_setUIProgress(DWORD dwUIProgress);
WINPCK_API DWORD		pck_getUIProgressUpper();


//添加/新增文件返回结果清单
WINPCK_API DWORD		pck_getUpdateResult_OldFileCount();
WINPCK_API DWORD		pck_getUpdateResult_PrepareToAddFileCount();
WINPCK_API DWORD		pck_getUpdateResult_ChangedFileCount();
WINPCK_API DWORD		pck_getUpdateResult_DuplicateFileCount();
WINPCK_API DWORD		pck_getUpdateResult_FinalFileCount();

//日志
WINPCK_API void			log_regShowFunc(ShowLogW _ShowLogCallBack);
WINPCK_API const char	log_getLogLevelPrefix(int _loglevel);
WINPCK_API void			log_PrintA(const char chLevel, CLPSTR  _fmt, ...);
WINPCK_API void			log_PrintW(const char chLevel, CLPWSTR  _fmt, ...);

#ifdef UNICODE
#define log_Print log_PrintW
#else
#define log_Print log_PrintA
#endif

//打开、关闭、复原等事件注册
WINPCK_API void			pck_regMsgFeedback(void* pTag, FeedbackCallback _FeedbackCallBack);

#endif //WINPCK_DLL_H


