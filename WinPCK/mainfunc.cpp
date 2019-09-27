//////////////////////////////////////////////////////////////////////
// mainfunc.cpp: WinPCK 界面线程部分
// 打开并显示pck文件、查找文件、记录浏览位置 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.4.10
// 2012.10.10.OK
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4995 )
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4005 )

#include "tlib.h"
#include "resource.h"
#include "winmain.h"
#include <shlwapi.h>
#include "OpenSaveDlg.h"
#include <stdio.h>
#include "StopWatch.h"

BOOL TInstDlg::OpenPckFile(wchar_t *lpszFileToOpen, BOOL isReOpen)
{

	CStopWatch	timer;
	wchar_t	szString[64];
	int		iListTopView;

	m_currentNodeOnShow = NULL;

	if(0 != *lpszFileToOpen && lpszFileToOpen != m_Filename) {
		wcscpy_s(m_Filename, MAX_PATH, lpszFileToOpen);
	}

	if(!isReOpen) {

		*m_FolderBrowsed = 0;
	} else {
		//记录位置
		iListTopView = ListView_GetTopIndex(GetDlgItem(IDC_LIST));
	}

	if(0 != *lpszFileToOpen || OpenSingleFile(hWnd, m_Filename, TEXT_FILE_FILTER)) {
		timer.start();

		//转换文件名格式 
		if(WINPCK_OK == pck_open(m_Filename)) {
			timer.stop();
			swprintf_s(szString, 64, GetLoadStrW(IDS_STRING_OPENOK), timer.getElapsedTime());
			//SetStatusBarText(4, szString);
			SetStatusBarInfo(szString);

			//SetStatusBarText(0, _tcsrchr(m_Filename, TEXT('\\')) + 1);
			SetStatusBarTitle(wcsrchr(m_Filename, L'\\') + 1);
			//SetStatusBarText(3, TEXT(""));
			ClearStatusBarProgress();

			//_stprintf_s(szString, 64, GetLoadStr(IDS_STRING_OPENFILESIZE), pck_filesize());
			//SetStatusBarText(1, szString);
			SetStatusBarFileSize(pck_filesize());

			//_stprintf_s(szString, 64, GetLoadStr(IDS_STRING_OPENFILECOUNT), pck_filecount());
			//SetStatusBarText(2, szString);
			SetStatusBarFileCount(pck_filecount());

			if(isReOpen) {
				ShowPckFiles(pck_getFileEntryByPath(m_FolderBrowsed));
				ListView_EnsureVisible(GetDlgItem(IDC_LIST), iListTopView, NULL);
				ListView_EnsureVisible(GetDlgItem(IDC_LIST), iListTopView + ListView_GetCountPerPage(GetDlgItem(IDC_LIST)) - 1, NULL);
			} else
				ShowPckFiles(pck_getRootNode());

			return TRUE;
		} else {
			//SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
			SetStatusBarInfo(GetLoadStrW(IDS_STRING_PROCESS_ERROR));
			pck_close();
			return FALSE;
		}

	}
	//SetStatusBarText(4, GetLoadStr(IDS_STRING_OPENFAIL));
	SetStatusBarInfo(GetLoadStrW(IDS_STRING_OPENFAIL));
	return FALSE;

}

void ShowFilelistCallback(void* _in_param, int sn, const wchar_t *lpszFilename, int entry_type, unsigned __int64 qwFileSize, unsigned __int64 qwFileSizeCompressed, void* fileEntry)
{
	TInstDlg * const pThis = (TInstDlg*)_in_param;

	const HWND	hList = pThis->GetDlgItem(IDC_LIST);

	if (PCK_ENTRY_TYPE_DOTDOT != (PCK_ENTRY_TYPE_DOTDOT & entry_type)) {
		wchar_t	szClearTextSize[CHAR_NUM_LEN], szCipherTextSize[CHAR_NUM_LEN];
		wchar_t	szCompressionRatio[CHAR_NUM_LEN];

		if (0 == qwFileSize)
			wcscpy(szCompressionRatio, L"-");
		else
			swprintf_s(szCompressionRatio, CHAR_NUM_LEN, L"%.1f%%", qwFileSizeCompressed / (float)qwFileSize * 100.0);

		pThis->InsertList(hList, sn,
			LVIF_PARAM | LVIF_IMAGE, (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & entry_type)) ? IMGLIST_FOLDER : IMGLIST_FILE,
			fileEntry, 4,
			lpszFilename,
			StrFormatByteSizeW(qwFileSize, szClearTextSize, CHAR_NUM_LEN),
			StrFormatByteSizeW(qwFileSizeCompressed, szCipherTextSize, CHAR_NUM_LEN),
			szCompressionRatio);

	}
	else {
		//PCK_ENTRY_TYPE_DOTDOT 时只能是文件夹
		pThis->InsertList(hList, sn,
			LVIF_PARAM | LVIF_IMAGE, IMGLIST_FOLDER,
			fileEntry, 1,
			lpszFilename
		);
	}
#ifdef _DEBUG
	wchar_t szPrintLine[1024];
	swprintf(szPrintLine, L"%s\t%s\t%llu\t%llu\r\n", lpszFilename, (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & entry_type)) ? L"Folder" : L"File", qwFileSize, qwFileSizeCompressed);
	OutputDebugStringW(szPrintLine);
#endif

}

VOID TInstDlg::SearchPckFiles()
{
	if(0 == pck_filecount())return;

	HWND	hList = GetDlgItem(IDC_LIST);

	//显示查找文字
	wchar_t	szPrintf[64];
	swprintf_s(szPrintf, 64, GetLoadStrW(IDS_STRING_SEARCHING), m_szStrToSearch);
	SendDlgItemMessageW(IDC_STATUS, SB_SETTEXTW, 4, (LPARAM)szPrintf);

	ListView_DeleteAllItems(hList);

	//清除浏览记录
	*m_FolderBrowsed = 0;

	::SendMessage(hList, WM_SETREDRAW, FALSE, 0);

	DWORD dwFoundCount = pck_searchByName(m_szStrToSearch, this, ShowFilelistCallback);

	::SendMessage(hList, WM_SETREDRAW, TRUE, 0);

	swprintf_s(szPrintf, 64, GetLoadStrW(IDS_STRING_SEARCHOK), m_szStrToSearch, dwFoundCount);
	SendDlgItemMessageW(IDC_STATUS, SB_SETTEXTW, 4, (LPARAM)szPrintf);

}

VOID TInstDlg::ShowPckFiles(const PCK_UNIFIED_FILE_ENTRY*	lpNodeToShow)
{
	HWND	hList = GetDlgItem(IDC_LIST);

	if (NULL == lpNodeToShow) {
		MessageBoxW(L"Node Not Found!!!\r\nShow Root Node");
		lpNodeToShow = pck_getRootNode();
	}

	m_currentNodeOnShow = lpNodeToShow;

	ListView_DeleteAllItems(hList);

	::SendMessage(hList, WM_SETREDRAW, FALSE, 0);

	pck_listByNode(lpNodeToShow, this, ShowFilelistCallback);

	::SendMessage(hList, WM_SETREDRAW, TRUE, 0);

}
