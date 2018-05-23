//////////////////////////////////////////////////////////////////////
// threadproc.cpp: WinPCK 界面线程部分
// 压缩、解压、更新等大用时过程，需多线程调用 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.4.10
// 2012.10.10
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4244 )
#pragma warning ( disable : 4311 )
//#pragma warning ( disable : 4018 )
#pragma warning ( disable : 4005 )

#include "tlib.h"
#include "resource.h"
#include "globals.h"
#include "winmain.h"
#include <process.h>
#include "OpenSaveDlg.h"


//VOID EnumAndDeleteIndex(LPPCK_PATH_NODE lpNode);
VOID GetPckFileNameBySource(TCHAR *dst, TCHAR *src, BOOL isDirectory);

_inline void TInstDlg::EnbaleButtons(int ctlExceptID, BOOL bEnbale)
{

	WORD wToolBarTextList[] = { IDS_STRING_OPEN,
								IDS_STRING_CLOSE,
								IDS_STRING_NEW,
								0,
								IDS_STRING_UNPACK_SELECTED,
								IDS_STRING_UNPACK_ALL,
								0,
								IDS_STRING_ADD,
								IDS_STRING_REBUILD,
								IDS_STRING_INFO,
								IDS_STRING_SEARCH,
								IDS_STRING_COMPRESS_OPT,
								0,
								IDS_STRING_ABOUT,
								IDS_STRING_EXIT };


	WORD ctlMenuIDs[] = { ID_MENU_OPEN,
						ID_MENU_CLOSE,
						ID_MENU_NEW,
						1,
						ID_MENU_UNPACK_SELECTED,
						ID_MENU_UNPACK_ALL,
						1,
						ID_MENU_ADD,
						ID_MENU_REBUILD,
						ID_MENU_INFO,
						ID_MENU_SEARCH,
						ID_MENU_COMPRESS_OPT,
						0 };

	WORD ctlRMenuIDs[] = { ID_MENU_VIEW,
						ID_MENU_UNPACK_SELECTED,
						ID_MENU_RENAME,
						ID_MENU_DELETE,
						ID_MENU_ATTR,
						0 };

	TBBUTTONINFO tbbtninfo = { 0 };
	tbbtninfo.cbSize = sizeof(TBBUTTONINFO);
	tbbtninfo.dwMask = TBIF_IMAGE | /*TBIF_TEXT | */TBIF_LPARAM;

	WORD	*pctlIDs = ctlMenuIDs;
	WORD	*pctlStrings = wToolBarTextList;

	while(0 != *pctlIDs) {

		if(ctlExceptID == *pctlIDs) {
			SendDlgItemMessage(IDC_TOOLBAR, TB_GETBUTTONINFO, ctlExceptID, (LPARAM)&tbbtninfo);

			if(bEnbale) {
				tbbtninfo.dwMask = TBIF_IMAGE | TBIF_TEXT;
				tbbtninfo.iImage = (int)tbbtninfo.lParam;
				tbbtninfo.pszText = GetLoadStr(*pctlStrings);

				EnableButton(ctlExceptID, bEnbale);

			} else {

				tbbtninfo.dwMask = TBIF_IMAGE | TBIF_TEXT | TBIF_LPARAM;
				tbbtninfo.lParam = tbbtninfo.iImage;
				tbbtninfo.iImage = 16;
				tbbtninfo.pszText = GetLoadStr(IDS_STRING_CANCEL);

			}

			SendDlgItemMessage(IDC_TOOLBAR, TB_SETBUTTONINFO, ctlExceptID, (LPARAM)&tbbtninfo);

		} else {
			EnableButton(*pctlIDs, bEnbale);
		}

		pctlStrings++;
		pctlIDs++;
	}


	HMENU	hMenu = ::GetMenu(hWnd);
	HMENU	hSubMenu = ::GetSubMenu(hMenu, 0);
	pctlIDs = ctlMenuIDs;

	while(1 != *pctlIDs) {
		::EnableMenuItem(hSubMenu, *pctlIDs, bEnbale ? MF_ENABLED : MF_GRAYED);
		pctlIDs++;
	}

	hSubMenu = ::GetSubMenu(hMenu, 1);

	pctlIDs++;
	while(1 != *pctlIDs) {
		::EnableMenuItem(hSubMenu, *pctlIDs, bEnbale ? MF_ENABLED : MF_GRAYED);
		pctlIDs++;
	}

	hSubMenu = ::GetSubMenu(hMenu, 2);

	pctlIDs++;
	while(0 != *pctlIDs) {
		::EnableMenuItem(hSubMenu, *pctlIDs, bEnbale ? MF_ENABLED : MF_GRAYED);
		pctlIDs++;
	}

	hMenu = ::LoadMenu(TApp::GetInstance(), (LPCTSTR)IDR_MENU_RCLICK);
	hSubMenu = ::GetSubMenu(hMenu, 0);

	pctlIDs = ctlRMenuIDs;

	while(0 != *pctlIDs) {
		::EnableMenuItem(hSubMenu, *pctlIDs, bEnbale ? MF_ENABLED : MF_GRAYED);
		pctlIDs++;
	}

	return;
}


VOID TInstDlg::UpdatePckFile(VOID *pParam)
{

	TInstDlg	*pThis = (TInstDlg*)pParam;
	BOOL		bDeleteClass = !pThis->m_lpPckCenter->IsValidPck();
	TCHAR		szFilenameToSave[MAX_PATH];
	LPPCK_RUNTIME_PARAMS	lpParams = pThis->lpPckParams;
	BOOL		*lpbThreadRunning = &lpParams->cVarParams.bThreadRunning;
	TCHAR		szPrintf[320];
	double		t1, t2;//计算时间

	if(bDeleteClass)
		pThis->m_lpPckCenter->New();

	*szFilenameToSave = 0;

	if(bDeleteClass) {
		//此时没有打开文件，这时的操作相当于新建文档
		if(1 == pThis->m_DropFileCount) {
			GetPckFileNameBySource(szFilenameToSave, *pThis->m_lpszFilePath, FALSE);
		}

		//选择保存的文件名
		int nSelectFilter = SaveFile(pThis->hWnd, szFilenameToSave, pThis->m_lpPckCenter->GetSaveDlgFilterString());
		if(!nSelectFilter) {
			//pThis->DeleteClass();
			pThis->m_lpPckCenter->Close();

			return;
		}

		//设定目标pck的版本
		pThis->m_lpPckCenter->SetPckVersion(nSelectFilter);

		_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWING), wcsrchr(szFilenameToSave, TEXT('\\')) + 1);
	} else {
		_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWING), wcsrchr(pThis->m_Filename, TEXT('\\')) + 1);
	}

	//开始计时
	t1 = GetTickCount();

	pThis->EnbaleButtons(ID_MENU_ADD, FALSE);


	pThis->SetStatusBarText(4, szPrintf);

	pThis->m_lpPckCenter->Reset(pThis->m_DropFileCount);
	*lpbThreadRunning = TRUE;

	pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

	if(pThis->m_lpPckCenter->UpdatePckFile(szFilenameToSave, pThis->m_lpszFilePath, pThis->m_DropFileCount, pThis->m_currentNodeOnShow)) {
		if(*lpbThreadRunning) {
			//计时结束
			t2 = GetTickCount() - t1;
			pThis->m_lpPckCenter->PrintLogN(GetLoadStr(IDS_STRING_RENEWOK), t2);
			//pThis->SetStatusBarText(4, szPrintf);
		} else {

			pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		}

		if(bDeleteClass) {

			pThis->OpenPckFile(szFilenameToSave, TRUE);
		} else {
			pThis->OpenPckFile(pThis->m_Filename, TRUE);
		}

	} else {

		pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));

		if(bDeleteClass) {
			pThis->m_lpPckCenter->Close();
		}
	}

	pThis->EnbaleButtons(ID_MENU_ADD, TRUE);

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if((!(*lpbThreadRunning)) && pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	*lpbThreadRunning = FALSE;

	//还原Drop状态
	pThis->m_DropFileCount = 0;
	DragAcceptFiles(pThis->hWnd, TRUE);

	//整合报告
	// 打印报告
	// pck包中原有文件 %d 个\r\n
	// 新加入文件 %d 个，其中重复文件名 %d 个\r\n
	// 使用原数据地址 %d 个，新数据地址 %d 个\r\n
	// 通过成的新pck文件共 %d 个文件\r\n

	if(0 != lpParams->cVarParams.dwPrepareToAddFileCount) {

		_stprintf_s(szPrintf,
			TEXT("此更新过程数据如下：\r\n")
			TEXT("PCK 包中原有文件数： %d\r\n")
			TEXT("计划更新文件数： %d\r\n")
			TEXT("实际更新文件数： %d\r\n")
			TEXT("重名文件数： %d\r\n")
			TEXT("重名文件中使用新数据区的数量： %d\r\n")
			TEXT("重名文件中使用老数据区的数量： %d\r\n")
			TEXT("未更新文件数： %d\r\n")
			TEXT("更新后 PCK 包中文件数： %d"),
			lpParams->cVarParams.dwOldFileCount,
			lpParams->cVarParams.dwPrepareToAddFileCount,
			lpParams->cVarParams.dwChangedFileCount,
			lpParams->cVarParams.dwDuplicateFileCount,
			lpParams->cVarParams.dwUseNewDataAreaInDuplicateFileSize,
			lpParams->cVarParams.dwChangedFileCount - lpParams->cVarParams.dwUseNewDataAreaInDuplicateFileSize,
			lpParams->cVarParams.dwPrepareToAddFileCount - lpParams->cVarParams.dwChangedFileCount,
			lpParams->cVarParams.dwFinalFileCount);

		pThis->MessageBox(szPrintf, TEXT("更新报告"), MB_OK | MB_ICONINFORMATION);

		pThis->m_lpPckCenter->PrintLogI(szPrintf);
	}

	return;

}

VOID TInstDlg::RenamePckFile(VOID *pParam)
{

	TInstDlg	*pThis = (TInstDlg*)pParam;
	TCHAR		szPrintf[64];
	double		t1, t2;//计算时间

	_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWING), wcsrchr(pThis->m_Filename, TEXT('\\')) + 1);

	//开始计时
	t1 = GetTickCount();

	pThis->EnbaleButtons(ID_MENU_RENAME, FALSE);

	pThis->SetStatusBarText(4, szPrintf);

	pThis->m_lpPckCenter->Reset();
	pThis->lpPckParams->cVarParams.bThreadRunning = TRUE;

	pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

	if(pThis->m_lpPckCenter->RenameFilename()) {

		//计时结束
		t2 = GetTickCount() - t1;
		_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWOK), t2);

		pThis->SetStatusBarText(4, szPrintf);

		pThis->OpenPckFile(pThis->m_Filename, TRUE);

	} else {
		pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		pThis->m_lpPckCenter->Close();
	}

	pThis->EnbaleButtons(ID_MENU_RENAME, TRUE);

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if(pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	pThis->lpPckParams->cVarParams.bThreadRunning = FALSE;

	return;

}

VOID TInstDlg::RebuildPckFile(VOID	*pParam)
{

	TInstDlg	*pThis = (TInstDlg*)pParam;

	BOOL		bDeleteClass = !pThis->m_lpPckCenter->IsValidPck();

	TCHAR		szFilenameToSave[MAX_PATH];
	TCHAR		szPrintf[288];

	BOOL		*lpbThreadRunning = &pThis->lpPckParams->cVarParams.bThreadRunning;

	double		t1, t2;//计算时间

	if(bDeleteClass) {
		//bDeleteClass = TRUE;
		if(!pThis->OpenPckFile()) {
			return;
		}
	}

	_tcscpy_s(szFilenameToSave, pThis->m_Filename);

	TCHAR		*lpszFileTitle = wcsrchr(szFilenameToSave, TEXT('\\')) + 1;
	_tcscpy(lpszFileTitle, TEXT("Rebuild_"));
	_tcscat_s(szFilenameToSave, wcsrchr(pThis->m_Filename, TEXT('\\')) + 1);

	//弹出选项对话框
	//BOOL(CPckControlCenter::*RebuildOrRecompressPckFile)(LPTSTR);
	//调用对话框
	BOOL  bNeedRecompress;
	TRebuildOptDlg	dlg(pThis->lpPckParams, &bNeedRecompress, pThis);
	DWORD dwCompressLevel = pThis->lpPckParams->dwCompressLevel;
	if(IDCANCEL == dlg.Exec())
		return;

	//RebuildOrRecompressPckFile = bNeedRecompress ? &CPckControlCenter::RecompressPckFile : &CPckControlCenter::RebuildPckFile;

	//选择保存的文件名
	int nSelectFilter = SaveFile(pThis->hWnd, szFilenameToSave, pThis->m_lpPckCenter->GetSaveDlgFilterString(), pThis->m_lpPckCenter->GetPckVersion());
	if(!nSelectFilter) {
		return;
	}

	pThis->m_lpPckCenter->SetPckVersion(nSelectFilter);

	//开始计时
	t1 = GetTickCount();

	pThis->EnbaleButtons(ID_MENU_REBUILD, FALSE);

	_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_REBUILDING), wcsrchr(szFilenameToSave, TEXT('\\')) + 1);
	pThis->SetStatusBarText(4, szPrintf);

	pThis->m_lpPckCenter->Reset(pThis->m_lpPckCenter->GetPckFileCount());
	*lpbThreadRunning = TRUE;

	pThis->SetTimer(WM_TIMER_PROGRESS_100, 100, NULL);

	if(pThis->m_lpPckCenter->RebuildPckFile(szFilenameToSave, bNeedRecompress)) {
		if(*lpbThreadRunning) {
			//计时结束
			t2 = GetTickCount() - t1;

			pThis->m_lpPckCenter->PrintLogN(GetLoadStr(IDS_STRING_REBUILDOK), t2);
			//pThis->SetStatusBarText(4, szPrintf);
		} else {
			pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		}

	} else {

		pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));

	}

	pThis->EnbaleButtons(ID_MENU_REBUILD, TRUE);

	if(bDeleteClass) {
		ListView_DeleteAllItems(pThis->GetDlgItem(IDC_LIST));
		pThis->m_lpPckCenter->Close();
	}

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if((!(*lpbThreadRunning)) && pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	*lpbThreadRunning = FALSE;

	return;

}

VOID TInstDlg::CreateNewPckFile(VOID	*pParam)
{
	TInstDlg	*pThis = (TInstDlg*)pParam;

	BOOL		bDeleteClass = !pThis->m_lpPckCenter->IsValidPck();
	TCHAR		szPathToCompress[MAX_PATH], szFilenameToSave[MAX_PATH];

	TCHAR		szPrintf[64];
	BOOL		*lpbThreadRunning = &pThis->lpPckParams->cVarParams.bThreadRunning;

	double		t1, t2;//计算时间

	//选择目录
	if(!BrowseForFolderByPath(pThis->hWnd, pThis->m_CurrentPath))
		return;

	_tcscpy_s(szPathToCompress, pThis->m_CurrentPath);

	GetPckFileNameBySource(szFilenameToSave, szPathToCompress, TRUE);

	if(bDeleteClass)
		pThis->m_lpPckCenter->New();

	//选择保存的文件名
	int nSelectFilter = SaveFile(pThis->hWnd, szFilenameToSave, pThis->m_lpPckCenter->GetSaveDlgFilterString());
	if(!nSelectFilter)
		return;

	//设定目标pck的版本
	pThis->m_lpPckCenter->SetPckVersion(nSelectFilter);

	//开始计时
	t1 = GetTickCount();

	pThis->EnbaleButtons(ID_MENU_NEW, FALSE);

	_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_COMPING), wcsrchr(szFilenameToSave, TEXT('\\')) + 1);
	pThis->SetStatusBarText(4, szPrintf);

	pThis->m_lpPckCenter->Reset();
	*lpbThreadRunning = TRUE;

	pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

	if(pThis->m_lpPckCenter->CreatePckFile(szFilenameToSave, szPathToCompress)) {

		if(*lpbThreadRunning) {
			//计时结束
			t2 = GetTickCount() - t1;

			pThis->m_lpPckCenter->PrintLogN(GetLoadStr(IDS_STRING_COMPOK), t2);
			//pThis->SetStatusBarText(4, szPrintf);
		} else {
			pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		}

	} else {

		pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
	}

	pThis->EnbaleButtons(ID_MENU_NEW, TRUE);

	if(bDeleteClass)
		pThis->m_lpPckCenter->Close();

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if((!(*lpbThreadRunning)) && pThis->bGoingToExit) {

		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	*lpbThreadRunning = FALSE;

	//还原Drop状态
	pThis->m_DropFileCount = 0;
	return;

}

VOID TInstDlg::ToExtractAllFiles(VOID	*pParam)
{
	TInstDlg	*pThis = (TInstDlg*)pParam;

	TCHAR		szPrintf[64];

	BOOL		*lpbThreadRunning = &pThis->lpPckParams->cVarParams.bThreadRunning;

	double		t1, t2;//计算时间

	//开始计时
	t1 = GetTickCount();

	pThis->EnbaleButtons(ID_MENU_UNPACK_ALL, FALSE);

	_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_EXPING), wcsrchr(pThis->m_Filename, TEXT('\\')) + 1);
	pThis->SetStatusBarText(4, szPrintf);

	pThis->m_lpPckCenter->Reset(pThis->m_lpPckCenter->m_lpPckRootNode->child->dwFilesCount);
	*lpbThreadRunning = TRUE;

	pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

	if(pThis->m_lpPckCenter->ExtractFiles(&pThis->m_lpPckCenter->m_lpPckRootNode, 1)) {
		//计时结束
		t2 = GetTickCount() - t1;
		_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_EXPOK), t2);

		pThis->SetStatusBarText(4, szPrintf);

	} else {
		pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
	}

	pThis->EnbaleButtons(ID_MENU_UNPACK_ALL, TRUE);

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if((!(*lpbThreadRunning)) && pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	*lpbThreadRunning = FALSE;

	return;

}

VOID TInstDlg::ToExtractSelectedFiles(VOID	*pParam)
{
	TInstDlg	*pThis = (TInstDlg*)pParam;

	LVITEM item;

	BOOL		*lpbThreadRunning = &pThis->lpPckParams->cVarParams.bThreadRunning;
	LPDWORD		lpdwUIProgressUpper = &pThis->lpPckParams->cVarParams.dwUIProgressUpper;

	pThis->m_lpPckCenter->Reset(0);


	HWND	hList = pThis->GetDlgItem(IDC_LIST);

	double		t1, t2;//计算时间

	LPPCK_PATH_NODE		*lpNodeToShow, *lpNodeToShowPtr;
	LPPCKINDEXTABLE		*lpIndexToShow, *lpIndexToShowPtr;

	UINT uiSelectCount = ListView_GetSelectedCount(hList);

	if(0 != uiSelectCount) {
		if(NULL != (lpNodeToShow = (LPPCK_PATH_NODE*)malloc(sizeof(LPPCK_PATH_NODE) * uiSelectCount))) {
			TCHAR		szPrintf[64];

			lpIndexToShow = (LPPCKINDEXTABLE*)lpNodeToShow;

			//取lpNodeToShow

			int	nCurrentItemCount = ListView_GetItemCount(hList);

			item.mask = LVIF_PARAM | LVIF_STATE;
			item.iSubItem = 0;
			item.stateMask = LVIS_SELECTED;		// get all state flags

			lpNodeToShowPtr = lpNodeToShow;
			lpIndexToShowPtr = lpIndexToShow;

			uiSelectCount = 0;

			if(pThis->m_lpPckCenter->GetListInSearchMode()) {
				for(item.iItem = 1;item.iItem < nCurrentItemCount;item.iItem++) {
					ListView_GetItem(hList, &item);

					if(item.state & LVIS_SELECTED) {
						*lpIndexToShowPtr = (LPPCKINDEXTABLE)item.lParam;
						(*lpdwUIProgressUpper)++;
						lpIndexToShowPtr++;

						uiSelectCount++;
					}
				}
			} else {
				for(item.iItem = 1;item.iItem < nCurrentItemCount;item.iItem++) {
					ListView_GetItem(hList, &item);

					if(item.state & LVIS_SELECTED) {
						*lpNodeToShowPtr = (LPPCK_PATH_NODE)item.lParam;
						if(NULL == (*lpNodeToShowPtr)->child) {
							(*lpdwUIProgressUpper)++;
						} else {
							(*lpdwUIProgressUpper) += (*lpNodeToShowPtr)->child->dwFilesCount;
						}
						lpNodeToShowPtr++;

						uiSelectCount++;
					}
				}
			}

			if(0 == uiSelectCount)return;

			//开始计时
			t1 = GetTickCount();

			*lpbThreadRunning = TRUE;

			pThis->EnbaleButtons(ID_MENU_UNPACK_SELECTED, FALSE);

			_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_EXPING), wcsrchr(pThis->m_Filename, TEXT('\\')) + 1);
			pThis->SetStatusBarText(4, szPrintf);

			pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

			lpNodeToShowPtr = lpNodeToShow;

			if(pThis->m_lpPckCenter->GetListInSearchMode()) {
				if(pThis->m_lpPckCenter->ExtractFiles(lpIndexToShow, uiSelectCount)) {
					//计时结束
					t2 = GetTickCount() - t1;
					_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_EXPOK), t2);

					pThis->SetStatusBarText(4, szPrintf);
				} else {
					pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
				}
			} else {
				if(pThis->m_lpPckCenter->ExtractFiles(lpNodeToShow, uiSelectCount)) {
					//计时结束
					t2 = GetTickCount() - t1;
					_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_EXPOK), t2);

					pThis->SetStatusBarText(4, szPrintf);
				} else {
					pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
				}

			}

			free(lpNodeToShow);

			pThis->EnbaleButtons(ID_MENU_UNPACK_SELECTED, TRUE);

			pThis->KillTimer(WM_TIMER_PROGRESS_100);
			pThis->RefreshProgress();

			if((!(*lpbThreadRunning)) && pThis->bGoingToExit) {
				pThis->bGoingToExit = FALSE;
				pThis->SendMessage(WM_CLOSE, 0, 0);
			}
		}
	}

	*lpbThreadRunning = FALSE;

	return;

}


VOID TInstDlg::DeleteFileFromPckFile(VOID	*pParam)
{
	TInstDlg	*pThis = (TInstDlg*)pParam;

	LVITEM item;

	BOOL		*lpbThreadRunning = &pThis->lpPckParams->cVarParams.bThreadRunning;

	pThis->m_lpPckCenter->Reset();


	HWND	hList = pThis->GetDlgItem(IDC_LIST);

	double		t1, t2;//计算时间

	LPPCK_PATH_NODE		lpNodeToShow;//, *lpNodeToShowPtr;
	LPPCKINDEXTABLE		lpIndexToShow;//, *lpIndexToShowPtr;

	UINT uiSelectCount = ListView_GetSelectedCount(hList);

	if(0 != uiSelectCount) {

		TCHAR		szPrintf[64];

		int	nCurrentItemCount = ListView_GetItemCount(hList);

		item.mask = LVIF_PARAM | LVIF_STATE;
		item.iSubItem = 0;
		item.stateMask = LVIS_SELECTED;		// get all state flags

		uiSelectCount = 0;


		if(pThis->m_lpPckCenter->GetListInSearchMode()) {
			for(item.iItem = 1;item.iItem < nCurrentItemCount;item.iItem++) {
				ListView_GetItem(hList, &item);

				if(item.state & LVIS_SELECTED) {

					lpIndexToShow = (LPPCKINDEXTABLE)item.lParam;
					lpIndexToShow->bSelected = TRUE;

					uiSelectCount++;
				}
			}
		} else {
			for(item.iItem = 1;item.iItem < nCurrentItemCount;item.iItem++) {
				ListView_GetItem(hList, &item);

				if(item.state & LVIS_SELECTED) {

					lpNodeToShow = (LPPCK_PATH_NODE)item.lParam;

					if(NULL == lpNodeToShow->child) {
						lpNodeToShow->lpPckIndexTable->bSelected = TRUE;
					} else {
						//EnumAndDeleteIndex(lpNodeToShow);
						pThis->m_lpPckCenter->DeleteNode(lpNodeToShow);
					}

					uiSelectCount++;
				}
			}
		}

		if(0 == uiSelectCount)return;

		//开始计时
		t1 = GetTickCount();

		*lpbThreadRunning = TRUE;

		pThis->EnbaleButtons(ID_MENU_DELETE, FALSE);

		_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWING), wcsrchr(pThis->m_Filename, TEXT('\\')) + 1);
		pThis->SetStatusBarText(4, szPrintf);

		pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);


		if(pThis->m_lpPckCenter->RenameFilename()) {
			//计时结束
			t2 = GetTickCount() - t1;
			_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWOK), t2);

			pThis->SetStatusBarText(4, szPrintf);

			pThis->OpenPckFile(pThis->m_Filename, TRUE);

		} else {
			pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		}

		pThis->EnbaleButtons(ID_MENU_DELETE, TRUE);

		pThis->KillTimer(WM_TIMER_PROGRESS_100);
		pThis->RefreshProgress();

		if(pThis->bGoingToExit) {
			pThis->bGoingToExit = FALSE;
			pThis->SendMessage(WM_CLOSE, 0, 0);
		}

	}

	*lpbThreadRunning = FALSE;

	return;

}

//从拖入的源文件名推出预保存的pck文件名
VOID GetPckFileNameBySource(TCHAR *dst, TCHAR *src, BOOL isDirectory)
{
	int szPathToCompressLen;
	_tcscpy(dst, src);

	if(isDirectory) {
		if((szPathToCompressLen = lstrlen(dst)) > 13 && 0 == lstrcmp(dst + szPathToCompressLen - 10, TEXT(".pck.files"))) {
			*(dst + szPathToCompressLen - 10) = 0;
		}
	}

	_tcscat(dst, TEXT(".pck"));
}