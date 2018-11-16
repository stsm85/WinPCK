#include "pck_handle.h"
#include "PckControlCenter.h"
#include <list>
using namespace std;

static list<CPckControlCenter*> all_sessions;

#define SETCLASS(_handle) ((CPckControlCenter*)##_handle)

BOOL checkIfValidHandle(HANDLE handle)
{
	BOOL isFound = FALSE;

	if(all_sessions.empty())
		return FALSE;

	for (list<CPckControlCenter*>::const_iterator iter = all_sessions.begin(); iter != all_sessions.end(); iter++)
	{
		if (handle == *iter)
			return TRUE;
	}
	return FALSE;
}

WINPCK_API HANDLE pck_new()
{

	CPckControlCenter* lpcPckCenter = new CPckControlCenter();
	all_sessions.push_back(lpcPckCenter);

	return lpcPckCenter;
}

WINPCK_API void	pck_free(HANDLE handle)
{

	if (NULL != handle) {
		if (checkIfValidHandle(handle)) {

			delete handle;
			all_sessions.remove(SETCLASS(handle));
		}
	}
}

WINPCK_API BOOL pck_open(HANDLE handle, LPCTSTR lpszFile)
{
	if (!checkIfValidHandle(handle))
		return FALSE;

	return SETCLASS(handle)->Open(lpszFile);
}

WINPCK_API void	pck_close(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return;

	SETCLASS(handle)->New();
}

WINPCK_API int pck_getVersion(HANDLE handle)
{

	if (!checkIfValidHandle(handle))
		return -1;

	return SETCLASS(handle)->GetPckVersion();

}

WINPCK_API BOOL	pck_setVersion(HANDLE handle, int verID)
{
	if (!checkIfValidHandle(handle))
		return FALSE;

	return SETCLASS(handle)->SetPckVersion(verID);
}

WINPCK_API DWORD pck_getVersionCount()
{
	return CPckControlCenter::GetVersionCount();
}

WINPCK_API const wchar_t*	pck_getVersionNameById(int verID)
{
	return CPckControlCenter::GetVersionNameById(verID);
}

//获取当前配置名称
WINPCK_API LPCTSTR pck_GetCurrentVersionName(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return NULL;

	return SETCLASS(handle)->GetCurrentVersionName();
}

//有效的
WINPCK_API int	pck_IsValidPck(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return NULL;

	return SETCLASS(handle)->IsValidPck();
}

//获取node路径
WINPCK_API BOOL	pck_getNodeRelativePath(wchar_t* _out_szCurrentNodePathString, const PCK_UNIFIED_FILE_ENTRY* lpFileEntry)
{

	return CPckControlCenter::GetCurrentNodeString(_out_szCurrentNodePathString, lpFileEntry);
}

WINPCK_API const PCK_UNIFIED_FILE_ENTRY* pck_getFileEntryByPath(HANDLE handle, wchar_t* _in_szCurrentNodePathString)
{
	if (!checkIfValidHandle(handle))
		return NULL;

	return SETCLASS(handle)->GetFileEntryByPath(_in_szCurrentNodePathString);
}

WINPCK_API const PCK_UNIFIED_FILE_ENTRY* pck_getRootNode(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return NULL;

	return SETCLASS(handle)->GetRootNode();
}

WINPCK_API const PCK_UNIFIED_FILE_ENTRY* pck_getFirstNode(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return NULL;

	return SETCLASS(handle)->GetFirstNode();
}

//文件大小
WINPCK_API QWORD	pck_filesize(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return -1;

	return SETCLASS(handle)->GetPckSize();
}

WINPCK_API QWORD	pck_file_data_area_size(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return -1;

	return SETCLASS(handle)->GetPckDataAreaSize();
}

WINPCK_API QWORD	pck_file_redundancy_data_size(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return -1;

	return SETCLASS(handle)->GetPckRedundancyDataSize();
}

WINPCK_API DWORD	pck_filecount(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return -1;

	return SETCLASS(handle)->GetPckFileCount();
}

WINPCK_API QWORD	pck_getFileSizeInEntry(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry)
{
	return CPckControlCenter::GetFileSizeInEntry(lpFileEntry);
}

WINPCK_API QWORD	pck_getCompressedSizeInEntry(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry)
{
	return CPckControlCenter::GetCompressedSizeInEntry(lpFileEntry);
}

WINPCK_API DWORD	pck_getFoldersCountInEntry(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry)
{
	return CPckControlCenter::GetFoldersCountInEntry(lpFileEntry);
}

WINPCK_API DWORD	pck_getFilesCountInEntry(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry)
{
	return CPckControlCenter::GetFilesCountInEntry(lpFileEntry);
}

//当前节点文件名当前长度
WINPCK_API DWORD	pck_getFilelenBytesOfEntry(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry)
{
	return CPckControlCenter::GetFilelenBytesOfEntry(lpFileEntry);
}

//当前节点文件名最大长度-当前长度
WINPCK_API DWORD	pck_getFilelenLeftBytesOfEntry(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry)
{
	return CPckControlCenter::GetFilelenLeftBytesOfEntry(lpFileEntry);
}

WINPCK_API QWORD	pck_getFileOffset(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry)
{
	return CPckControlCenter::GetFileOffset(lpFileEntry);
}

//设置附加信息
WINPCK_API const char* pck_GetAdditionalInfo(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return NULL;

	return SETCLASS(handle)->GetAdditionalInfo();
}

WINPCK_API DWORD	pck_GetAdditionalInfoMaxSize()
{
	return CPckControlCenter::GetAdditionalInfoMaxSize();
}

WINPCK_API BOOL pck_SetAdditionalInfo(HANDLE handle, const char *lpszAdditionalInfo)
{
	if (!checkIfValidHandle(handle))
		return NULL;

	return SETCLASS(handle)->SetAdditionalInfo(lpszAdditionalInfo);
}

//是否是支持更新的文件
WINPCK_API BOOL	pck_isSupportAddFileToPck(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return NULL;

	return SETCLASS(handle)->isSupportAddFileToPck();
}

//重命名一个节点
WINPCK_API BOOL pck_RenameEntry(HANDLE handle, PCK_UNIFIED_FILE_ENTRY* lpFileEntry, wchar_t* lpszReplaceString)
{
	if (!checkIfValidHandle(handle))
		return NULL;

	return SETCLASS(handle)->RenameEntry(lpFileEntry, lpszReplaceString);
}

//重命名文件
WINPCK_API BOOL	pck_RenameSubmit(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return NULL;

	return SETCLASS(handle)->RenameSubmit();
}

//预览文件
WINPCK_API BOOL pck_GetSingleFileData(HANDLE handle, const PCK_UNIFIED_FILE_ENTRY* lpFileEntry, char *_inout_buffer, size_t _in_sizeOfBuffer)
{
	if (!checkIfValidHandle(handle))
		return NULL;

	return SETCLASS(handle)->GetSingleFileData(lpFileEntry, _inout_buffer, _in_sizeOfBuffer);
}

//解压文件
WINPCK_API BOOL	pck_ExtractFilesByEntrys(HANDLE handle, const PCK_UNIFIED_FILE_ENTRY **lpFileEntryArray, int nEntryCount, const wchar_t *lpszDestDirectory)
{
	if (!checkIfValidHandle(handle))
		return NULL;

	return SETCLASS(handle)->ExtractFiles(lpFileEntryArray, nEntryCount, lpszDestDirectory);
}

WINPCK_API BOOL	pck_ExtractAllFiles(HANDLE handle, const wchar_t *lpszDestDirectory)
{
	if (!checkIfValidHandle(handle))
		return NULL;

	return SETCLASS(handle)->ExtractAllFiles(lpszDestDirectory);
}

WINPCK_API BOOL	do_ExtractPartFiles(LPCTSTR lpszFilePathSrc, const wchar_t *lpszDestDirectory, const wchar_t *lpszFileToExtract)
{
	return CPckControlCenter::ExtractFiles(lpszFilePathSrc, lpszDestDirectory, lpszFileToExtract);
}
WINPCK_API BOOL	do_ExtractAllFiles(LPCTSTR lpszFilePathSrc, const wchar_t *lpszDestDirectory)
{
	return CPckControlCenter::ExtractAllFiles(lpszFilePathSrc, lpszDestDirectory);
}

//重建pck文件
WINPCK_API BOOL	pck_ParseScript(HANDLE handle, LPCTSTR lpszScriptFile)
{
	if (!checkIfValidHandle(handle))
		return NULL;

	return SETCLASS(handle)->ParseScript(lpszScriptFile);
}

WINPCK_API BOOL	pck_RebuildPckFile(HANDLE handle, LPTSTR szRebuildPckFile, BOOL bUseRecompress)
{
	if (!checkIfValidHandle(handle))
		return NULL;

	return SETCLASS(handle)->RebuildPckFile(szRebuildPckFile, bUseRecompress);
}

WINPCK_API BOOL	pck_RebuildPckFileWithScript(HANDLE handle, LPCTSTR lpszScriptFile, LPTSTR szRebuildPckFile, BOOL bUseRecompress)
{
	if (!checkIfValidHandle(handle))
		return NULL;

	return SETCLASS(handle)->RebuildPckFileWithScript(lpszScriptFile, szRebuildPckFile, bUseRecompress);
}

//新建、更新pck文件
WINPCK_API void	pck_StringArrayReset(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return;

	return SETCLASS(handle)->StringArrayReset();
}

WINPCK_API void	pck_StringArrayAppend(HANDLE handle, LPCTSTR lpszFilePath)
{
	if (!checkIfValidHandle(handle))
		return;

	return SETCLASS(handle)->StringArrayAppend(lpszFilePath);
}

WINPCK_API BOOL	pck_UpdatePckFileSubmit(HANDLE handle, LPCTSTR szPckFile, const PCK_UNIFIED_FILE_ENTRY* lpFileEntry)
{
	if (!checkIfValidHandle(handle))
		return FALSE;

	return SETCLASS(handle)->UpdatePckFileSubmit(szPckFile, lpFileEntry);
}

//添加文件到pck
WINPCK_API BOOL	do_AddFileToPckFile(LPCTSTR lpszFilePathSrc, LPCTSTR szPckFile, const wchar_t *lpszPathInPckToAdd)
{
	return CPckControlCenter::AddFileToPckFile(lpszFilePathSrc, szPckFile, lpszPathInPckToAdd);
}

//创建新的pck文件
WINPCK_API BOOL	do_CreatePckFile(LPCTSTR lpszFilePathSrc, LPCTSTR szPckFile, int _versionId)
{
	return CPckControlCenter::CreatePckFile(lpszFilePathSrc, szPckFile, _versionId);
}

//删除一个节点
WINPCK_API BOOL	pck_DeleteEntry(HANDLE handle, const PCK_UNIFIED_FILE_ENTRY* lpFileEntry)
{
	if (!checkIfValidHandle(handle))
		return FALSE;

	return SETCLASS(handle)->DeleteEntry(lpFileEntry);
}

//提交
WINPCK_API BOOL pck_DeleteEntrySubmit(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return FALSE;

	return SETCLASS(handle)->DeleteEntrySubmit();
}

//查询
WINPCK_API DWORD pck_searchByName(HANDLE handle, const wchar_t* lpszSearchString, void* _in_param, SHOW_LIST_CALLBACK _showListCallback)
{
	if (!checkIfValidHandle(handle))
		return 0;

	return SETCLASS(handle)->SearchByName(lpszSearchString, _in_param, _showListCallback);
}

WINPCK_API DWORD pck_listByNode(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry, void* _in_param, SHOW_LIST_CALLBACK _showListCallback)
{
	return CPckControlCenter::ListByNode(lpFileEntry, _in_param, _showListCallback);
}

WINPCK_API const PCK_UNIFIED_FILE_ENTRY* pck_getUpwardEntry(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry)
{
	return CPckControlCenter::GetUpwardEntry(lpFileEntry);
}

WINPCK_API const PCK_UNIFIED_FILE_ENTRY* pck_getDownwardEntry(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry)
{
	return CPckControlCenter::GetDownwardEntry(lpFileEntry);
}

WINPCK_API BOOL pck_isThreadWorking(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return FALSE;

	return SETCLASS(handle)->isThreadWorking();
}

WINPCK_API void	pck_forceBreakThreadWorking(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return;

	return SETCLASS(handle)->ForceBreakThreadWorking();
}

WINPCK_API BOOL	pck_isLastOptSuccess(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return FALSE;

	return SETCLASS(handle)->isLastOptSuccess();
}

WINPCK_API BOOL	pck_getLastErrorMsg(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return FALSE;

	return SETCLASS(handle)->GetLastErrorMsg();
}



//内存占用
WINPCK_API DWORD	pck_getMTMemoryUsed(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return 0;

	return SETCLASS(handle)->getMTMemoryUsed();
}

WINPCK_API DWORD	pck_getMTMaxMemory(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return CPckControlCenter::getMaxMemoryAllowed();

	return SETCLASS(handle)->getMTMaxMemory();
}

WINPCK_API void		pck_setMTMaxMemory(HANDLE handle, DWORD dwMTMaxMemory)
{
	if (!checkIfValidHandle(handle))
		return;

	return SETCLASS(handle)->setMTMaxMemory(dwMTMaxMemory);
}

WINPCK_API DWORD	pck_getMaxMemoryAllowed()
{
	return CPckControlCenter::getMaxMemoryAllowed();
}

//线程数
WINPCK_API DWORD	pck_getMaxThreadUpperLimit()
{
	return CPckControlCenter::getMaxThreadUpperLimit();
}

WINPCK_API DWORD	pck_getMaxThread(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return 0;

	return SETCLASS(handle)->getMaxThread();
}

WINPCK_API void		pck_setMaxThread(HANDLE handle, DWORD dwThread)
{
	if (!checkIfValidHandle(handle))
		return;

	return SETCLASS(handle)->setMaxThread(dwThread);
}

//压缩等级
WINPCK_API DWORD	pck_getMaxCompressLevel()
{
	return CPckControlCenter::getMaxCompressLevel();
}

WINPCK_API DWORD	pck_getDefaultCompressLevel()
{
	return CPckControlCenter::getDefaultCompressLevel();
}

WINPCK_API DWORD	pck_getCompressLevel(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return CPckControlCenter::getDefaultCompressLevel();

	return SETCLASS(handle)->getCompressLevel();
}

WINPCK_API void		pck_setCompressLevel(HANDLE handle, DWORD dwCompressLevel)
{
	if (!checkIfValidHandle(handle))
		return;

	return SETCLASS(handle)->setCompressLevel(dwCompressLevel);
}

WINPCK_API DWORD	pck_getUIProgress(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return 0;

	return SETCLASS(handle)->getUIProgress();
}

WINPCK_API void		pck_setUIProgress(HANDLE handle, DWORD dwUIProgress)
{
	if (!checkIfValidHandle(handle))
		return;

	return SETCLASS(handle)->setUIProgress(dwUIProgress);
}

WINPCK_API DWORD	pck_getUIProgressUpper(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return 0;

	return SETCLASS(handle)->getUIProgressUpper();
}

//添加/新增文件返回结果清单
WINPCK_API DWORD	pck_getUpdateResult_OldFileCount(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return 0;

	return SETCLASS(handle)->GetUpdateResult_OldFileCount();
}

WINPCK_API DWORD	pck_getUpdateResult_PrepareToAddFileCount(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return 0;

	return SETCLASS(handle)->GetUpdateResult_PrepareToAddFileCount();
}

WINPCK_API DWORD	pck_getUpdateResult_ChangedFileCount(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return 0;

	return SETCLASS(handle)->GetUpdateResult_ChangedFileCount();
}

WINPCK_API DWORD	pck_getUpdateResult_DuplicateFileCount(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return 0;

	return SETCLASS(handle)->GetUpdateResult_DuplicateFileCount();
}

WINPCK_API DWORD	pck_getUpdateResult_FinalFileCount(HANDLE handle)
{
	if (!checkIfValidHandle(handle))
		return 0;

	return SETCLASS(handle)->GetUpdateResult_FinalFileCount();
}

//日志
WINPCK_API void		log_regShowFunc(ShowLogW _ShowLogW)
{
	CPckControlCenter::regShowFunc(_ShowLogW);
}

WINPCK_API const char	log_getLogLevelPrefix(int _loglevel)
{
	return CPckControlCenter::getLogLevelPrefix(_loglevel);
}

WINPCK_API void		log_PrintA(const char chLevel, const char *_fmt, ...)
{
	va_list	ap;
	va_start(ap, _fmt);
	CPckControlCenter::Print(chLevel, _fmt, ap);
	va_end(ap);
}

WINPCK_API void		log_PrintW(const char chLevel, const wchar_t *_fmt, ...)
{
	va_list	ap;
	va_start(ap, _fmt);
	CPckControlCenter::Print(chLevel, _fmt, ap);
	va_end(ap);
}

//打开、关闭、复原等事件注册
WINPCK_API void		pck_regMsgFeedback(void* pTag, FeedbackCallback _FeedbackCallBack)
{
	CPckControlCenter::regMsgFeedback(pTag, _FeedbackCallBack);
}