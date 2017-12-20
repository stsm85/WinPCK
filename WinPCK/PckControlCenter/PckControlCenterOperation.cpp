
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
#include <strsafe.h>


void CPckControlCenter::New()
{
	Close();
	m_lpClassPck = new CPckClass(&cParams);

}

BOOL CPckControlCenter::Open(LPCTSTR lpszFile)
{

	//判断文件格式
	FMTPCK emunFileFormat;
	size_t	nFileLength;

	StringCchLength(lpszFile, MAX_PATH, &nFileLength);

	if(0 == lstrcmpi(lpszFile + nFileLength - 4, TEXT(".pck"))){
		emunFileFormat = FMTPCK_PCK;
	}else if(0 == lstrcmpi(lpszFile + nFileLength - 4, TEXT(".zup"))){
		emunFileFormat = FMTPCK_ZUP;
	}

	while(1){

		Close();
		switch(emunFileFormat)
		{
		case FMTPCK_PCK:
			m_lpClassPck = new CPckClass(&cParams);
			break;

		case FMTPCK_ZUP:
			m_lpClassPck = new CZupClass(&cParams);
			break;

		default:
			m_lpClassPck = new CPckClass(&cParams);
			break;
		}

		//*m_lpClassPck->m_lastErrorString = 0;

		PrintLog(LOG_FLAG_INFO, TEXT_LOG_OPENFILE, (TCHAR *)lpszFile);

		if(m_lpClassPck->Init(lpszFile)){
			
			DeleteRestoreData();

			m_lpPckRootNode = m_lpClassPck->GetPckPathNode();

			m_emunFileFormat = emunFileFormat;

			//打开成功，刷新标题
			SendMessage(m_hWndMain, WM_FRESH_MAIN_CAPTION, 1, 0);
			return TRUE;

		}else{
			

			if(hasRestoreData){
				if(IDYES == MessageBoxA(m_hWndMain, TEXT_ERROR_OPEN_AFTER_UPDATE, TEXT_ERROR, MB_YESNO | MB_ICONHAND)){

					RestoreData(lpszFile);
				}else{
					//Close();
					return FALSE;
				}

			}else{

				//Close();
				return FALSE;
			}

		}
	}

}

void CPckControlCenter::SetPckVersion(int verID)
{
	//GetSaveFileName返回的nFilterIndex是以1为基数的，此时应该-1
	m_lpClassPck->SetPckVersion(verID - 1);
}

int	CPckControlCenter::GetPckVersion()
{
	//GetSaveFileName返回的nFilterIndex是以1为基数的，此时应该+1
	return (m_lpClassPck->GetPckVersion()->id + 1);
}

void CPckControlCenter::Close()
{
	if(NULL != m_lpClassPck)
	{
		PrintLog(LOG_FLAG_INFO, TEXT_LOG_CLOSEFILE);

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

	if(m_lpClassPck->GetPckBasicInfo(m_lpszFile4Restore, &m_PckHeadForRestore, m_lpPckFileIndexData, m_dwPckFileIndexDataSize))
	{
		hasRestoreData = TRUE;
	}else{
		PrintLogE(TEXT_ERROR_GET_RESTORE_DATA);
	}

}

void CPckControlCenter::RestoreData(LPCTSTR lpszFile)
{
	if(hasRestoreData)
	{
		if(0 == lstrcmpi(m_lpszFile4Restore, lpszFile)){

			if(!m_lpClassPck->SetPckBasicInfo(&m_PckHeadForRestore, m_lpPckFileIndexData, m_dwPckFileIndexDataSize))
				PrintLogE(TEXT_ERROR_RESTORING);
			else
				PrintLogI(TEXT_LOG_RESTOR_OK);
		}
	}
	DeleteRestoreData();
}

void CPckControlCenter::DeleteRestoreData()
{
	if(NULL != m_lpPckFileIndexData){

		free(m_lpPckFileIndexData);
		m_lpPckFileIndexData = NULL;
	}

	hasRestoreData = FALSE;
}

VOID	CPckControlCenter::RenameIndex(LPPCK_PATH_NODE lpNode, char* lpszReplaceString)
{
	m_lpClassPck->RenameIndex(lpNode, lpszReplaceString);
}

VOID	CPckControlCenter::RenameIndex(LPPCKINDEXTABLE lpIndex, char* lpszReplaceString)
{
	m_lpClassPck->RenameIndex(lpIndex, lpszReplaceString);
}

BOOL	CPckControlCenter::RenameNode(LPPCK_PATH_NODE lpNode, char* lpszReplaceString)
{
	return m_lpClassPck->RenameNode(lpNode, lpszReplaceString);
}

//更新pck文件
BOOL	CPckControlCenter::UpdatePckFile(LPTSTR szPckFile, TCHAR (*lpszFilePath)[MAX_PATH], int nFileCount, LPPCK_PATH_NODE lpNodeToInsert)
{
	return m_lpClassPck->UpdatePckFile(szPckFile, lpszFilePath, nFileCount, lpNodeToInsert);
}

//重命名文件
BOOL	CPckControlCenter::RenameFilename()
{
	//创建一个备份，用于失败后恢复
	CreateRestoreData();

	return m_lpClassPck->RenameFilename();
}

//重建pck文件
BOOL	CPckControlCenter::RebuildPckFile(LPTSTR szRebuildPckFile)
{
	return m_lpClassPck->RebuildPckFile(szRebuildPckFile);
}

//重建pck文件并重压缩数据
BOOL	CPckControlCenter::RecompressPckFile(LPTSTR szRecompressPckFile)
{
	return m_lpClassPck->RecompressPckFile(szRecompressPckFile);
}

//新建pck文件--
BOOL	CPckControlCenter::CreatePckFile(LPTSTR szPckFile, LPTSTR szPath)
{
	return m_lpClassPck->CreatePckFile(szPckFile, szPath);
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
VOID	CPckControlCenter::DeleteNode(LPPCK_PATH_NODE lpNode)
{
	//创建一个备份，用于失败后恢复
	CreateRestoreData();

	return m_lpClassPck->DeleteNode(lpNode);
}

//重置PCK的压缩参数
void	CPckControlCenter::ResetCompressor()
{
	m_lpClassPck->init_compressor();
}

