
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
//#include <strsafe.h>


void CPckControlCenter::New()
{
	Close();
	m_lpClassPck = new CPckClass(&cParams);
}

FMTPCK	CPckControlCenter::GetPckTypeFromFilename(LPCTSTR lpszFile)
{
	size_t nFileLength = _tcsnlen(lpszFile, MAX_PATH);

	if(0 == _tcsicmp(lpszFile + nFileLength - 4, TEXT(".pck"))) {
		return FMTPCK_PCK;
	} else if(0 == _tcsicmp(lpszFile + nFileLength - 4, TEXT(".zup"))) {
		return FMTPCK_ZUP;
	}
	return FMTPCK_UNKNOWN;
}

BOOL CPckControlCenter::Open(LPCTSTR lpszFile, BOOL isOpenAfterRestore)
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

		m_PckLog.PrintLogI(TEXT(TEXT_LOG_OPENFILE), lpszFile);

		if(m_lpClassPck->Init(lpszFile)) {

			m_emunFileFormat = emunFileFormat;

			DeleteRestoreData();

			//创建一个备份，用于失败后恢复
			CreateRestoreData();

			m_lpPckRootNode = m_lpClassPck->GetPckPathNode();

			//打开成功，刷新标题
			SendMessage(m_hWndMain, WM_FRESH_MAIN_CAPTION, 1, 0);
			return TRUE;

		} else {
			if((FALSE == isOpenAfterRestore) && RestoreData(lpszFile, emunFileFormat)) {

				return Open(lpszFile, TRUE);
			} else {
				return FALSE;
			}
		}
	}
}

BOOL CPckControlCenter::Open(LPCTSTR lpszFile)
{
	return Open(lpszFile, FALSE);
}

void CPckControlCenter::SetPckVersion(int verID)
{
	//GetSaveFileName返回的nFilterIndex是以1为基数的，此时应该-1
	m_lpClassPck->SetSavePckVersion(verID - 1);
}

int	CPckControlCenter::GetPckVersion()
{
	//GetSaveFileName返回的nFilterIndex是以1为基数的，此时应该+1
	return (m_lpClassPck->GetPckVersion()->id + 1);
}

void CPckControlCenter::Close()
{
	if(NULL != m_lpClassPck) {
		m_PckLog.PrintLogI(TEXT_LOG_CLOSEFILE);

		delete m_lpClassPck;
		m_lpClassPck = NULL;

		m_lpPckRootNode = NULL;
	}

	m_isSearchMode = FALSE;
	m_emunFileFormat = FMTPCK_UNKNOWN;

	//关闭文件，刷新标题
	SendMessage(m_hWndMain, WM_FRESH_MAIN_CAPTION, 0, 0);
}

void CPckControlCenter::CreateRestoreData()
{
	if(FMTPCK_PCK == m_emunFileFormat) {
		if(!m_lpClassPck->GetPckBasicInfo(&m_RestoreInfomation)) {
			m_PckLog.PrintLogE(TEXT_ERROR_GET_RESTORE_DATA);
		}
	}
}

BOOL CPckControlCenter::RestoreData(LPCTSTR lpszFile, FMTPCK emunFileFormat)
{
	BOOL rtn = FALSE;
	if(FMTPCK_PCK == emunFileFormat) {
		if(m_RestoreInfomation.isValid) {

			if(IDYES == MessageBoxA(m_hWndMain, TEXT_ERROR_OPEN_AFTER_UPDATE, TEXT_ERROR, MB_YESNO | MB_ICONHAND)) {
				if(0 == _tcsicmp(m_RestoreInfomation.szFile, lpszFile)) {

					if(!m_lpClassPck->SetPckBasicInfo(&m_RestoreInfomation))
						m_PckLog.PrintLogE(TEXT_ERROR_RESTORING);
					else {
						m_PckLog.PrintLogI(TEXT_LOG_RESTOR_OK);
						rtn = TRUE;
					}
				}
			}
			DeleteRestoreData();
		}
	}
	return rtn;
}

void CPckControlCenter::DeleteRestoreData()
{
	if(m_RestoreInfomation.isValid) {
		if(NULL != m_RestoreInfomation.lpIndexTailBuffer) {

			free(m_RestoreInfomation.lpIndexTailBuffer);
			m_RestoreInfomation.lpIndexTailBuffer = NULL;
		}
		m_RestoreInfomation.isValid = FALSE;
	}
}

BOOL	CPckControlCenter::RenameIndex(LPPCK_PATH_NODE lpNode, char* lpszReplaceString)
{
	return m_lpClassPck->RenameIndex(lpNode, lpszReplaceString);
}

BOOL	CPckControlCenter::RenameIndex(LPPCKINDEXTABLE lpIndex, char* lpszReplaceString)
{
	return m_lpClassPck->RenameIndex(lpIndex, lpszReplaceString);
}

BOOL	CPckControlCenter::RenameNode(LPPCK_PATH_NODE lpNode, char* lpszReplaceString)
{
	return m_lpClassPck->RenameNode(lpNode, lpszReplaceString);
}

//新建、更新pck文件
BOOL	CPckControlCenter::UpdatePckFile(LPCTSTR szPckFile, const vector<tstring> &lpszFilePath, const LPPCK_PATH_NODE lpNodeToInsert)
{
	return m_lpClassPck->UpdatePckFile(szPckFile, lpszFilePath, lpNodeToInsert);
}

//重命名文件
BOOL	CPckControlCenter::RenameFilename()
{
	return m_lpClassPck->RenameFilename();
}

//重建pck文件
BOOL	CPckControlCenter::RebuildPckFile(LPTSTR szRebuildPckFile, BOOL bUseRecompress)
{
	return m_lpClassPck->RebuildPckFile(szRebuildPckFile, bUseRecompress);
}

//解压文件
BOOL	CPckControlCenter::ExtractFiles(LPPCKINDEXTABLE *lpIndexToExtract, int nFileCount)
{
	return m_lpClassPck->ExtractFiles(lpIndexToExtract, nFileCount);
}

BOOL	CPckControlCenter::ExtractFiles(LPPCK_PATH_NODE *lpNodeToExtract, int nFileCount)
{
	return m_lpClassPck->ExtractFiles(lpNodeToExtract, nFileCount);
}

//删除一个节点
VOID	CPckControlCenter::DeleteNode(LPPCKINDEXTABLE lpNode)
{
	return m_lpClassPck->DeleteNode(lpNode);
}

//删除一个节点
VOID	CPckControlCenter::DeleteNode(LPPCK_PATH_NODE lpNode)
{
	return m_lpClassPck->DeleteNode(lpNode);
}

//重置PCK的压缩参数
void	CPckControlCenter::ResetCompressor()
{
	CPckClassZlib::set_level(cParams.dwCompressLevel);
}

//重建时的文件过滤
BOOL	CPckControlCenter::ParseScript(LPCTSTR lpszScriptFile)
{
	return m_lpClassPck->ParseScript(lpszScriptFile);
}
