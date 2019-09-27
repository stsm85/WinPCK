
//////////////////////////////////////////////////////////////////////
// PckControlCenterOperation.cpp: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 头文件,界面与PCK类的数据交互，控制中心
//
// 此程序由 李秋枫/stsm/liqf 编写，pck结构参考若水的pck结构.txt，并
// 参考了其易语言代码中并于读pck文件列表的部分
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.10.10
//////////////////////////////////////////////////////////////////////

#include "PckControlCenter.h"
#include "ZupClass.h"
#include "PckClassZlib.h"
#include "PckClassLog.h"
#include "PckClassRebuildFilter.h"


void CPckControlCenter::New()
{
	Close();
	m_lpClassPck = new CPckClass(&cParams);
}

#pragma region 打开关闭文件

FMTPCK	CPckControlCenter::GetPckTypeFromFilename(const wchar_t * lpszFile)
{
	size_t nFileLength = wcsnlen(lpszFile, MAX_PATH);

	if(0 == wcsicmp(lpszFile + nFileLength - 4, L".pck")) {
		return FMTPCK_PCK;
	} else if(0 == wcsicmp(lpszFile + nFileLength - 4, L".zup")) {
		return FMTPCK_ZUP;
	}
	return FMTPCK_UNKNOWN;
}

BOOL CPckControlCenter::Open(const wchar_t * lpszFile)
{
	//判断文件格式
	FMTPCK emunFileFormat = GetPckTypeFromFilename(lpszFile);

	while(1) {

		Close();
		switch(emunFileFormat) {
		case FMTPCK_ZUP:
			m_lpClassPck = new CZupClass(&cParams);
			break;
		case FMTPCK_PCK:
		default:
			m_lpClassPck = new CPckClass(&cParams);
			break;
		}

		Logger.i(UCSTEXT(TEXT_LOG_OPENFILE), lpszFile);

		if(m_lpClassPck->Init(lpszFile)) {

			m_emunFileFormat = emunFileFormat;

			m_lpPckRootNode = m_lpClassPck->GetPckPathNode();

			//打开成功，刷新标题
			pFeedbackCallBack(pTag, PCK_FILE_OPEN_SUCESS, NULL, (ssize_t)(m_lpClassPck->GetPckVersion()->name));
			return TRUE;

		} else {

			return FALSE;
		}
	}
}

void CPckControlCenter::Close()
{
	if(NULL != m_lpClassPck) {

		if(IsValidPck())
			Logger.i(TEXT_LOG_CLOSEFILE);

		delete m_lpClassPck;
		m_lpClassPck = NULL;

		m_lpPckRootNode = NULL;
	}

	m_emunFileFormat = FMTPCK_UNKNOWN;

	//清空参数
	Reset();

	//关闭文件，刷新标题
	pFeedbackCallBack(pTag, PCK_FILE_CLOSE, NULL, NULL);
}

#pragma endregion

#pragma region 重命名节点

BOOL CPckControlCenter::RenameEntry(LPENTRY lpFileEntry, LPCWSTR lpszReplaceString)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	int entryType = lpFileEntry->entryType;
	if (PCK_ENTRY_TYPE_NODE == entryType) {

		return m_lpClassPck->RenameIndex((LPPCK_PATH_NODE)lpFileEntry, lpszReplaceString);
	}
	else if (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & entryType)) {

		return m_lpClassPck->RenameNode((LPPCK_PATH_NODE)lpFileEntry, lpszReplaceString);
	}
	else if (PCK_ENTRY_TYPE_INDEX == entryType)
	{
		return m_lpClassPck->RenameIndex((LPPCKINDEXTABLE)lpFileEntry, lpszReplaceString);
	}
	else {
		return FALSE;
	}
}
BOOL CPckControlCenter::RenameSubmit()
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->RenameFilename();
}

#pragma endregion

#pragma region 预览解压文件

//预览文件
BOOL CPckControlCenter::GetSingleFileData(LPCENTRY lpFileEntry, char *buffer, size_t sizeOfBuffer)
{
	if ((NULL == m_lpClassPck) || (NULL == lpFileEntry))
		return FALSE;

	const PCKINDEXTABLE* lpPckFileIndexTable = NULL;
	if (PCK_ENTRY_TYPE_INDEX == lpFileEntry->entryType)
		lpPckFileIndexTable = (LPPCKINDEXTABLE)lpFileEntry;
	else
		lpPckFileIndexTable = ((LPPCK_PATH_NODE)lpFileEntry)->lpPckIndexTable;

	return m_lpClassPck->GetSingleFileData(lpPckFileIndexTable, buffer, sizeOfBuffer);
}

//解压文件
BOOL CPckControlCenter::ExtractFiles(const PCK_UNIFIED_FILE_ENTRY **lpFileEntryArray, int nEntryCount, LPCWSTR lpszDestDirectory)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->ExtractFiles(lpFileEntryArray, nEntryCount, lpszDestDirectory);
}

BOOL CPckControlCenter::ExtractAllFiles(LPCWSTR lpszDestDirectory)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->ExtractAllFiles(lpszDestDirectory);
}
#pragma endregion

#pragma region 重建pck文件
//重建pck文件
BOOL CPckControlCenter::TestScript(LPCWSTR lpszScriptFile)
{
	CPckClassRebuildFilter cScriptFilter;

	return cScriptFilter.TestScript(lpszScriptFile);
}

BOOL CPckControlCenter::RebuildPckFile(LPCWSTR lpszScriptFile, LPCWSTR szRebuildPckFile, BOOL bUseRecompress)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->RebuildPckFile(lpszScriptFile, szRebuildPckFile, bUseRecompress);
}

#pragma endregion

#pragma region 游戏精简
BOOL CPckControlCenter::StripPck(LPCWSTR lpszStripedPckFile, int flag)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->StripPck(lpszStripedPckFile, flag);
	
}

#pragma endregion

#pragma region 多个文件列表进行压缩时使用的vector操作
void CPckControlCenter::StringArrayReset()
{
	lpszFilePathToAdd.clear();
}

void CPckControlCenter::StringArrayAppend(LPCWSTR lpszFilePath)
{
	lpszFilePathToAdd.push_back(lpszFilePath);
}

#pragma endregion

#pragma region 新建/更新pck文件

BOOL CPckControlCenter::UpdatePckFileSubmit(LPCWSTR szPckFile, LPCENTRY lpFileEntry)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	BOOL rtn = FALSE;

	if (0 != lpszFilePathToAdd.size()) {
		rtn = m_lpClassPck->UpdatePckFile(szPckFile, lpszFilePathToAdd, (const PCK_PATH_NODE*)lpFileEntry);
		StringArrayReset();
	}
	return rtn;
}

#pragma region 删除节点
//删除一个节点
BOOL CPckControlCenter::DeleteEntry(LPCENTRY lpFileEntry)
{
	if ((NULL == m_lpClassPck) || (NULL == lpFileEntry))
		return FALSE;

	if (PCK_ENTRY_TYPE_INDEX == lpFileEntry->entryType)
		m_lpClassPck->DeleteNode((LPPCKINDEXTABLE)lpFileEntry);
	else if (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & lpFileEntry->entryType))
		m_lpClassPck->DeleteNode((LPPCK_PATH_NODE)lpFileEntry);
	else
		m_lpClassPck->DeleteNode(((LPPCK_PATH_NODE)lpFileEntry)->lpPckIndexTable);
	return TRUE;
}

//提交
BOOL CPckControlCenter::DeleteEntrySubmit()
{
	return RenameSubmit();
}

#pragma endregion


#pragma region 版本相关

BOOL CPckControlCenter::SetPckVersion(int verID)
{
	return m_lpClassPck->SetSavePckVersion(verID);
}

int	CPckControlCenter::GetPckVersion()
{
	return m_lpClassPck->GetPckVersion()->id;
}

//获取当前配置名称
LPCWSTR	CPckControlCenter::GetCurrentVersionName()
{
	if (NULL == m_lpClassPck)
		return NULL;

	return m_lpClassPck->GetPckVersion()->name;
}

uint32_t	CPckControlCenter::GetVersionCount()
{
	return CPckClassVersionDetect::GetPckVersionCount();
}

LPCWSTR	CPckControlCenter::GetVersionNameById(int verID)
{
	return CPckClassVersionDetect::GetPckVersionNameById(verID);
}

int CPckControlCenter::AddVersionAlgorithmId(int AlgorithmId, int Version)
{
	return CPckClassVersionDetect::AddPckVersion(AlgorithmId, Version);
}

#pragma endregion
