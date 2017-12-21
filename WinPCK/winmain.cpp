//////////////////////////////////////////////////////////////////////
// winmain.cpp: WinPCK 界面线程部分
// 界面类的初始化，消息循环等
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4244 )
#pragma warning ( disable : 4311 )
//#pragma warning ( disable : 4018 )
#pragma warning ( disable : 4005 )


#ifdef __NO_LTL_LIB
#include "_msvcrt.cpp"
#endif

#include "tlib.h"
#include "resource.h"
#include "globals.h"
#include "winmain.h"
#include <process.h>
#include <shlobj.h>
#include <strsafe.h>


BOOL GetWndPath(HWND hWnd, TCHAR * szPath);

/*
	WinMain
*/
int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR cmdLine, int nCmdShow)
{
	return	TInstApp(hI, cmdLine, nCmdShow).Run();
}


TInstApp::TInstApp(HINSTANCE _hI, LPSTR _cmdLine, int _nCmdShow) : TApp(_hI, _cmdLine, _nCmdShow)
{
}

TInstApp::~TInstApp()
{
}

void TInstApp::InitWindow(void)
{
/*	if (IsWinNT() && TIsWow64()) {
		DWORD	val = 0;
		TWow64DisableWow64FsRedirection(&val);
	}
*/
	TDlg *maindlg = new TInstDlg(cmdLine);
	mainWnd = maindlg;
	maindlg->InitRichEdit2();
	maindlg->Create();



}

TInstDlg::TInstDlg(LPSTR cmdLine) : TDlg(IDD_MAIN)//, staticText(this)
{
	//char	cEndChar;//, cFilename[MAX_PATH];
	//char	*lpcmdLine = cmdLine;

	////strcpy(cmdLine, "I:\\1.pck I:\\2.pck");
	////strcpy(cmdLine, "\"I:\\复件 1.pck\" \"I:\\复件 2.pck\"");
	////strcpy(cmdLine, "I:\\1aaaa.pck");

	//*m_Filename = 0;

	//if('\"' == *lpcmdLine)
	//{
	//	cEndChar = '\"';
	//	lpcmdLine++;
	//}
	//else
	//	cEndChar = ' ';

	//char	*lpcmdLinePtr = lpcmdLine;

	//while(*lpcmdLinePtr && (cEndChar != *lpcmdLinePtr))
	//{
	//	lpcmdLinePtr++;
	//}
	//*lpcmdLinePtr = 0;

	//DWORD dwAttr = GetFileAttributesA(lpcmdLine);

	//if(0 == (FILE_ATTRIBUTE_DIRECTORY & dwAttr))
	//	MultiByteToWideChar(CP_ACP, 0, lpcmdLine, -1, m_Filename, MAX_PATH);

}

TInstDlg::~TInstDlg()
{
}

/*
	儊僀儞僟僀傾儘僌梡 WM_INITDIALOG 張棟儖乕僠儞
*/
BOOL TInstDlg::EvCreate(LPARAM lParam)
{

	//InstallExceptionFilter(THIS_NAME, "%s\r\n异常信息已被保存到:\r\n%s\r\n");

	GetWindowRect(&rect);
	int		cx = ::GetSystemMetrics(SM_CXFULLSCREEN), cy = ::GetSystemMetrics(SM_CYFULLSCREEN);
	int		xsize = rect.right - rect.left, ysize = rect.bottom - rect.top;

	::SetClassLong(hWnd, GCL_HICON,
					(LONG_PTR)::LoadIcon(TApp::GetInstance(), (LPCTSTR)IDI_ICON_APP));
	MoveWindow((cx - xsize)/2, (cy - ysize)/2, xsize, ysize, TRUE);
	Show();

	//界面和数据初始化

	OleInitialize(0);

	//TSetPrivilege(SE_DEBUG_NAME, TRUE);
	//TSetPrivilege(SE_BACKUP_NAME, TRUE);
	//TSetPrivilege(SE_SYSTEM_ENVIRONMENT_NAME, TRUE);
	//TSetPrivilege(SE_CREATE_TOKEN_NAME, TRUE);
	//TSetPrivilege(SE_LOCK_MEMORY_NAME, TRUE);
	//TSetPrivilege(SE_SECURITY_NAME, TRUE);
	//TSetPrivilege(SE_RESTORE_NAME, TRUE);
	//TSetPrivilege(SE_CREATE_SYMBOLIC_LINK_NAME, TRUE);
	//TSetPrivilege(SE_AUDIT_NAME, TRUE);
	//TSetPrivilege(SE_UNDOCK_NAME, TRUE);
	//TSetPrivilege(SE_CREATE_GLOBAL_NAME, TRUE);
	//TSetPrivilege(SE_SYSTEM_PROFILE_NAME, TRUE);
	//TSetPrivilege(SE_LOAD_DRIVER_NAME, TRUE);

	SetWindowTextA( THIS_MAIN_CAPTION );

	//初始化公共控件
	initCommctrls();

	//初始化数据
	initParams();

	//初始化工具栏
	initToolbar();

	//初始化日志窗口
	InitLogWindow();

	//初始化浏览路径
	initArgument();

	//快捷键
	hAccel = LoadAccelerators(TApp::GetInstance(), MAKEINTRESOURCE(IDR_ACCELERATOR1));

	return	TRUE;
}


BOOL TInstDlg::EvClose()
{

	if(bGoingToExit)return FALSE;

	SetStatusBarText(0, GetLoadStr(IDS_STRING_EXITING));

	if(lpPckParams->cVarParams.bThreadRunning)
	{
		if(IDNO == MessageBox(GetLoadStr(IDS_STRING_ISEXIT), GetLoadStr(IDS_STRING_ISEXITTITLE), MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON2))return FALSE;

		bGoingToExit = TRUE;
		lpPckParams->cVarParams.bThreadRunning = FALSE;
		return FALSE;
	}

	ShowWindow(SW_HIDE);

	OleUninitialize();

	ImageList_Destroy(m_imageList);

	

#ifdef _USE_CUSTOMDRAW_
	for(int i = 0;i<HB_COUNT;i++)
	{
		DeleteObject(hBrushs[i]);
	}

	for(UINT i=0;i<2;i++)
	{
		DestroyIcon(hIconList[i]);
	}
#endif

	//DeleteClass();
	delete m_lpPckCenter;

	delete logdlg;

	::PostQuitMessage(0);
	return FALSE;
}


BOOL TInstDlg::EvTimer(WPARAM timerID, TIMERPROC proc)
{
	switch(timerID)
	{
	case WM_TIMER_PROGRESS_100:
		RefreshProgress();
		break;
	}
	return	FALSE;
}
/*
	儊僀儞僟僀傾儘僌梡 WM_COMMAND 張棟儖乕僠儞
*/
BOOL TInstDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	
	switch (wID) {
	case IDOK:

		return	TRUE;

	case IDCANCEL:

		return	TRUE;

	//case IDC_BUTTON_OPEN:
	case ID_OPEN_PCK:
	case ID_MENU_OPEN:

		OpenPckFile();

		break;

	//case IDC_BUTTON_UNPACK_ALL:
	case ID_MENU_UNPACK_ALL:

		if(m_lpPckCenter->IsValidPck())
		{
			if(lpPckParams->cVarParams.bThreadRunning)
			{
				lpPckParams->cVarParams.bThreadRunning = FALSE;
				//::EnableWindow((HWND)hwndCtl, FALSE);
				EnableButton(wID, FALSE);
			}else{
				if(BrowseForFolderByPath(m_CurrentPath))
				{
					//mt_MaxMemoryCount = 0;
					lpPckParams->cVarParams.dwMTMemoryUsed = 0;
					SetCurrentDirectory(m_CurrentPath);
					_beginthread(ToExtractAllFiles, 0, this);
				}
			}
		}
		break;

	//case ID_MENU_UNPACK_SELECTED_R:
	//		if(bThreadRunning)break;

	case ID_MENU_UNPACK_SELECTED:
		if(m_lpPckCenter->IsValidPck())
		{
			if(lpPckParams->cVarParams.bThreadRunning)
			{
				lpPckParams->cVarParams.bThreadRunning = FALSE;
				//::EnableWindow((HWND)hwndCtl, FALSE);
				EnableButton(wID, FALSE);
			}else{
				if(BrowseForFolderByPath(m_CurrentPath))
				{
					//mt_MaxMemoryCount = 0;
					lpPckParams->cVarParams.dwMTMemoryUsed = 0;
					SetCurrentDirectory(m_CurrentPath);
					_beginthread(ToExtractSelectedFiles, 0, this);
				}
			}
		}
		break;

	//case IDC_BUTTON_CLOSE:
	case ID_CLOSE_PCK:
	case ID_MENU_CLOSE:
		if(lpPckParams->cVarParams.bThreadRunning)
			lpPckParams->cVarParams.bThreadRunning = FALSE;

		ListView_DeleteAllItems(GetDlgItem(IDC_LIST));

		m_lpPckCenter->Close();
		break;

	//case IDC_BUTTON_INFO:
	case ID_MENU_INFO:

		if(m_lpPckCenter->IsValidPck())
		{
			TInfoDlg	dlg(m_lpPckCenter->GetAdditionalInfo(), this);
			if (dlg.Exec() == TRUE)
			{
				m_lpPckCenter->SetAdditionalInfo();
			}
		}

		break;
	//case IDC_BUTTON_SEARCH:
	case ID_MENU_SEARCH:

		if(m_lpPckCenter->IsValidPck())
		{
			TSearchDlg	dlg(m_szStrToSearch, this);
			if (dlg.Exec() == TRUE)
			{
				SearchPckFiles();
			}
		}

		break;

	//case IDC_BUTTON_NEW:
	case ID_CREATE_NEWPCK:
	case ID_MENU_NEW:

		if(lpPckParams->cVarParams.bThreadRunning)
		{
			lpPckParams->cVarParams.bThreadRunning = FALSE;
			//::EnableWindow((HWND)hwndCtl, FALSE);
			EnableButton(wID, FALSE);
		}else{
			//mt_MaxMemoryCount = mt_MaxMemory;
			//lpPckParams->cVarParams.dwMTMemoryUsed = lpPckParams->dwMTMaxMemory;

			_beginthread(CreateNewPckFile, 0, this);
		}
		break;

	//case IDC_BUTTON_ADD:
	case ID_ADD_FILE_TO_PCK:
	case ID_MENU_ADD:

			if(lpPckParams->cVarParams.bThreadRunning)
			{
				lpPckParams->cVarParams.bThreadRunning = FALSE;
				//::EnableWindow((HWND)hwndCtl, FALSE);
				EnableButton(wID, FALSE);
			}else{
				AddFiles();
			}	

		break;

	//case IDC_BUTTON_REBUILD:
	case ID_MENU_REBUILD:
		if(lpPckParams->cVarParams.bThreadRunning)
		{
			lpPckParams->cVarParams.bThreadRunning = FALSE;
			//::EnableWindow((HWND)hwndCtl, FALSE);
			EnableButton(wID, FALSE);
		}else{

			//mt_MaxMemoryCount = 0;
			lpPckParams->cVarParams.dwMTMemoryUsed = 0;
			_beginthread(RebuildPckFile, 0, this);
		}

		break;

	case ID_MENU_COMPRESS_OPT:
		{
			TCompressOptDlg	dlg(lpPckParams, this);
			DWORD dwCompressLevel = lpPckParams->dwCompressLevel;
			dlg.Exec();
			if(dwCompressLevel != lpPckParams->dwCompressLevel)
				if(lpPckParams->lpPckControlCenter->IsValidPck())
					lpPckParams->lpPckControlCenter->ResetCompressor();

		}
		break;

	case ID_LISTVIEW_RENAME:
	case ID_MENU_RENAME:

		{

			if(lpPckParams->cVarParams.bThreadRunning)break;
			HWND	hList = GetDlgItem(IDC_LIST);
			::SetWindowLong(hList, GWL_STYLE, ::GetWindowLong(hList, GWL_STYLE) | LVS_EDITLABELS);
			ListView_EditLabel(hList, m_lpPckCenter->GetListCurrentHotItem());



		}
		break;

	case ID_LISTVIEW_DELETE:
	case ID_MENU_DELETE:

		if(lpPckParams->cVarParams.bThreadRunning)break;

		if(IDNO == MessageBox(GetLoadStr(IDS_STRING_ISDELETE), GetLoadStr(IDS_STRING_ISDELETETITLE), MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON2))break;
		//mt_MaxMemoryCount = 0;
		lpPckParams->cVarParams.dwMTMemoryUsed = 0;
		_beginthread(DeleteFileFromPckFile, 0, this);

		break;

	case ID_LISTVIEW_SELECT_ALL:
	case ID_MENU_SELECTALL:
		{
			LVITEM item = {0};

			item.mask = LVIF_STATE;
			item.stateMask = item.state = LVIS_SELECTED;

			HWND hList = GetDlgItem(IDC_LIST);

			int	nItemCount = ListView_GetItemCount(hList);

			for(item.iItem=1;item.iItem<nItemCount;item.iItem++)
			{
				ListView_SetItem(hList, &item);
			}
		}
		break;

	case ID_LISTVIEW_SELECT_REV:
	case ID_MENU_SELECTORP:
		{
			LVITEM item = {0};

			item.mask = LVIF_STATE;
			item.stateMask = LVIS_SELECTED;

			HWND hList = GetDlgItem(IDC_LIST);

			int	nItemCount = ListView_GetItemCount(hList);

			for(item.iItem=1;item.iItem<nItemCount;item.iItem++)
			{
				ListView_GetItem(hList, &item);
				item.state = LVIS_SELECTED == item.state ? 0 : LVIS_SELECTED;
				ListView_SetItem(hList, &item);
			}
		}
		break;

	case ID_LISTVIEW_ATTR:
	case ID_MENU_ATTR:
		
		if(lpPckParams->cVarParams.bThreadRunning)
			break;

		ViewFileAttribute();

		break;

	case ID_MENU_EXIT:
		SendMessage(WM_CLOSE, 0, 0);
		break;

	case ID_MENU_VIEW:

		if(lpPckParams->cVarParams.bThreadRunning)
			break;

		ViewFile();

		break;
	case ID_MENU_SETUP:

		AddSetupReg();

		break;

	case ID_MENU_UNSETUP:

		DeleteSetupReg();
		break;

	case ID_MENU_ABOUT:
		{

			//BrowseForFolderByPath(m_Filename);
			MessageBoxA(		THIS_MAIN_CAPTION
								"\r\n"
								THIS_DESC
								"\r\n\r\n"
								THIS_AUTHOR 
								"\r\n\r\n"
								THIS_UESDLIB,
								"关于 "
								THIS_NAME ,
								MB_OK | MB_ICONASTERISK);
		}
		break;

	case ID_MENU_LOG:

		logdlg->Show();

		break;

	case ID_LISTVIEW_ENTER:

		DbClickListView(m_lpPckCenter->GetListCurrentHotItem());
		break;

	case ID_LISTVIEW_BACK:

		DbClickListView(0);
		break;

	case ID_LISTVIEW_POPMENU:

		PopupRightMenu(m_lpPckCenter->GetListCurrentHotItem());
		break;

	}

	return	FALSE;
}



VOID TInstDlg::SetStatusBarText(int	iPart, LPCSTR	lpszText )
{
	SendDlgItemMessageA(IDC_STATUS, SB_SETTEXTA, iPart, (LPARAM)lpszText);
	SendDlgItemMessageA(IDC_STATUS, SB_SETTIPTEXTA, iPart, (LPARAM)lpszText);
}

VOID TInstDlg::SetStatusBarText(int	iPart, LPCWSTR	lpszText )
{
	SendDlgItemMessageW(IDC_STATUS, SB_SETTEXTW, iPart, (LPARAM)lpszText);
	SendDlgItemMessageW(IDC_STATUS, SB_SETTIPTEXTW, iPart, (LPARAM)lpszText);
}

//__inline	void TInstDlg::ShowDebug(TCHAR *fmt,...)
//{
//	TCHAR	szString[1024];
//
//	va_list	ap;
//	va_start(ap, fmt);
//	_vsnwprintf(szString, 1024, fmt, ap);
//	va_end(ap);
//
//	SendDlgItemMessage(IDC_EDIT_INFO, EM_SETSEL, -2, -1);
//	SendDlgItemMessage(IDC_EDIT_INFO, EM_REPLACESEL, (WPARAM) 0, (LPARAM) szString);
//
//}

void TInstDlg::InsertList(CONST HWND hWndList, CONST INT iIndex, CONST UINT uiMask, CONST INT iImage, CONST LPVOID lParam, CONST INT nColCount, ...)
{
	
	if(0 == nColCount)return;

	LVITEMA	item;

	ZeroMemory(&item, sizeof(LVITEMA));


	va_list	ap;
	va_start(ap, nColCount);


	item.iItem = iIndex;			//从0开始
	item.iImage = iImage;
	item.iSubItem = 0;
	item.mask = LVIF_TEXT | uiMask;
	item.pszText = va_arg( ap, char* );
	item.lParam = (LPARAM)lParam;
	
	//ListView_InsertItem(hWndList,&item);
	::SendMessageA(hWndList, LVM_INSERTITEMA, 0, (LPARAM)&item);

	item.mask = LVIF_TEXT;

	for(item.iSubItem=1;item.iSubItem<nColCount;item.iSubItem++)
	{
		//item.iItem = iIndex;			//从0开始
		//item.iSubItem = i;
		item.pszText = va_arg( ap, char* );

		//item.pszText = _ultow(i, szID, 10);
		//ListView_SetItem(hWndList,&item);
		::SendMessageA(hWndList, LVM_SETITEMA, 0, (LPARAM)&item);
	}

	va_end(ap);
}

BOOL TInstDlg::InitListView(CONST HWND hWndListView, LPTSTR *lpszText, int *icx, int *ifmt)
{ 

#ifndef _USE_CUSTOMDRAW_
	HICON hiconItem;     // icon for list-view items 
#endif

	HIMAGELIST hSmall;   // image list for other views 
	LVCOLUMN lvcolumn;

	//full row select
	ListView_SetExtendedListViewStyle(hWndListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	int *lpicx = icx;
	int *lpifmt = ifmt;

	lvcolumn.iSubItem = -1;
	while(-1 != *(lpicx))
	{
		lvcolumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvcolumn.fmt = *(lpifmt++);
		lvcolumn.cx = *(lpicx++);
		lvcolumn.pszText = *(lpszText++);
		lvcolumn.iSubItem++;
		//lvcolumn.iOrder = 0;
		if(ListView_InsertColumn(hWndListView, lvcolumn.iSubItem, & lvcolumn) == -1)
			return FALSE;
	}

#ifdef _USE_CUSTOMDRAW_
	hSmall = ImageList_Create(16, 16, ILC_COLOR32, 1, 0);

#else
	hSmall = ImageList_Create(16, 16, ILC_COLOR32, 1, 2);

	hiconItem = LoadIcon(TApp::GetInstance(), MAKEINTRESOURCE(IDI_ICON_DIR)); 
	ImageList_AddIcon(hSmall, hiconItem); 
	DestroyIcon(hiconItem); 

	hiconItem = LoadIcon(TApp::GetInstance(), MAKEINTRESOURCE(IDI_ICON_FILE)); 
	ImageList_AddIcon(hSmall, hiconItem); 
	//ImageList_AddIcon(hSmall, hiconItem); 
	DestroyIcon(hiconItem); 
#endif
	ListView_SetImageList(hWndListView, hSmall, LVSIL_SMALL); 

	return TRUE; 
}


BOOL TInstDlg::EvNotify(UINT ctlID, NMHDR *pNmHdr)
{

	LPPCK_PATH_NODE		lpNodeToShow;
	LPPCKINDEXTABLE		lpIndexToShow;

	switch(ctlID)
	{


	case IDC_LIST:
		//Debug(L"lv:LVN_FIRST-%d,all:NM_FIRST-%d\r\n", LVN_FIRST-pNmHdr->code, NM_FIRST - pNmHdr->code);

		int iCurrentHotItem = ((LPNMLISTVIEW)pNmHdr)->iItem;

		switch (pNmHdr->code)
		{

		case LVN_ITEMCHANGED:
		case NM_CLICK:

			if (-1 != iCurrentHotItem){
				m_lpPckCenter->SetListCurrentHotItem(iCurrentHotItem);
			}
			break;

		case NM_RCLICK:
			if(-1 != iCurrentHotItem)
			{
				PopupRightMenu(iCurrentHotItem);
			}

			break;
		case NM_DBLCLK:
			
			if(-1 != iCurrentHotItem)
			{
				DbClickListView(iCurrentHotItem);
			}

			break;

		case LVN_BEGINDRAG:
			
			SetCapture();
			SetCursor(m_hCursorAllow);
			m_isSearchWindow = TRUE;
			
			break;

		case LVN_BEGINLABELEDIT:

			if(NULL == ((NMLVDISPINFO*) pNmHdr)->item.lParam)return TRUE;
			//isSearchMode = 2 == ((NMLVDISPINFO*) pNmHdr)->item.iImage ? TRUE : FALSE;

			size_t nLen, nAllowMaxLength;

			if(m_lpPckCenter->GetListInSearchMode())
			{
				lpIndexToShow = (LPPCKINDEXTABLE)((NMLVDISPINFO*) pNmHdr)->item.lParam;
				StringCchLengthA(lpIndexToShow->cFileIndex.szFilename, MAX_PATH_PCK_260, &nLen);
				nAllowMaxLength = MAX_PATH_PCK_260 - 2;
			}else{

				lpNodeToShow = (LPPCK_PATH_NODE)((NMLVDISPINFO*) pNmHdr)->item.lParam;
				if(NULL == lpNodeToShow->child)
				{
					StringCchLengthA(lpNodeToShow->lpPckIndexTable->cFileIndex.szFilename, MAX_PATH_PCK_260, &nLen);
					nAllowMaxLength = MAX_PATH_PCK_260 - nLen + strlen(lpNodeToShow->szName) - 2;
				}else{
					nAllowMaxLength = MAX_PATH_PCK_260 - 2;
				}
			}

			HWND	hEdit;

			if(NULL == (hEdit = ListView_GetEditControl(pNmHdr->hwndFrom)))return TRUE;

			::SendMessage(hEdit, EM_LIMITTEXT, nAllowMaxLength, 0);

			break;

		case LVN_ENDLABELEDIT:

			::SetWindowLong(pNmHdr->hwndFrom, GWL_STYLE, ::GetWindowLong(pNmHdr->hwndFrom, GWL_STYLE) & (LVS_EDITLABELS ^ 0xffffffff));

			char	szEditedText[MAX_PATH_PCK_260];
			memset(szEditedText, 0, MAX_PATH_PCK_260);

			if(NULL != ((NMLVDISPINFO*) pNmHdr)->item.pszText)
			{
				if(0 == *((NMLVDISPINFO*) pNmHdr)->item.pszText)return FALSE;

				//isSearchMode = 2 == ((NMLVDISPINFO*) pNmHdr)->item.iImage ? TRUE : FALSE;

				WideCharToMultiByte(CP_ACP, 0, ((NMLVDISPINFO*) pNmHdr)->item.pszText, -1, szEditedText, MAX_PATH_PCK_260, "_", 0);

				if(m_lpPckCenter->GetListInSearchMode())
				{
					lpIndexToShow = (LPPCKINDEXTABLE)((NMLVDISPINFO*) pNmHdr)->item.lParam;

					if(0 == strcmp(lpIndexToShow->cFileIndex.szFilename, szEditedText))
						return FALSE;
				}else{
					lpNodeToShow = (LPPCK_PATH_NODE)((NMLVDISPINFO*) pNmHdr)->item.lParam;

					if(0 == strcmp(lpNodeToShow->szName, szEditedText))
						return FALSE;
				}

				char*	lpszInvalid;
				if(m_lpPckCenter->GetListInSearchMode())
				{
					lpszInvalid = (char*)TEXT_INVALID_PATHCHAR + 2;
				}else{
					lpszInvalid = TEXT_INVALID_PATHCHAR;
				}

				while(0 != *lpszInvalid)
				{
					if(NULL != strchr(szEditedText, *lpszInvalid))
					{
						char szPrintf[64];
						StringCchPrintfA(szPrintf, 64, GetLoadStrA(IDS_STRING_INVALIDFILENAME), lpszInvalid);
						MessageBoxA(szPrintf, "提示", MB_OK | MB_ICONASTERISK);									
						return FALSE;
					}

					lpszInvalid++;
				}


				//size_t	nBytesAllowToWrite;
				if(m_lpPckCenter->GetListInSearchMode())
				{
					m_lpPckCenter->RenameIndex(lpIndexToShow, szEditedText);
					//lpszInvalid = lpIndexToShow->cFileIndex.szFilename;
					//nBytesAllowToWrite = MAX_PATH_PCK;
				}else{
					if(NULL == lpNodeToShow->child)
						m_lpPckCenter->RenameIndex(lpNodeToShow, szEditedText);
					else
					{
						if(!m_lpPckCenter->RenameNode(lpNodeToShow, szEditedText))
						{

							MessageBox(GetLoadStr(IDS_STRING_RENAMEERROR), GetLoadStr(IDS_STRING_ERROR), MB_OK | MB_ICONERROR);
							OpenPckFile(m_Filename, TRUE);
							return FALSE;
						}
					}
				}

				//调用修改
				//mt_MaxMemoryCount = 0;
				lpPckParams->cVarParams.dwMTMemoryUsed = 0;
				_beginthread(RenamePckFile, 0, this);

				return TRUE;
			}
			break;

		}

		break;


	}
	return FALSE;

}

BOOL TInstDlg::EventButton(UINT uMsg, int nHitTest, POINTS pos)
{
	switch(uMsg)
	{
	case WM_LBUTTONUP:

		if (m_isSearchWindow)
		{
			TCHAR szPath[MAX_PATH];

			//DoMouseUp();
			SetCursor(m_hCursorOld);
			ReleaseCapture();

			m_isSearchWindow = FALSE;

			if(IsValidWndAndGetPath(szPath, TRUE))
			{

				if(m_lpPckCenter->IsValidPck())
				{
					if(!lpPckParams->cVarParams.bThreadRunning)
					{

						//mt_MaxMemoryCount = 0;
						lpPckParams->cVarParams.dwMTMemoryUsed = 0;
						SetCurrentDirectory(szPath);
						_beginthread(ToExtractSelectedFiles, 0, this);

					}
				}

			}

		}

		break;
	}

	return TRUE;
}

BOOL TInstDlg::EvMouseMove(UINT fwKeys, POINTS pos)
{
	if (m_isSearchWindow)
	{
		//TCHAR szPath[MAX_PATH];

		if(IsValidWndAndGetPath(0, FALSE))
			SetCursor(m_hCursorAllow);
		else
			SetCursor(m_hCursorNo);

	}
	return TRUE;
}

BOOL TInstDlg::EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight)
{
	HWND	hList = GetDlgItem(IDC_LIST);
	HWND	hPrgs = GetDlgItem(IDC_PROGRESS);

	::SetWindowPos(GetDlgItem(IDC_TOOLBAR), NULL, 0, 0, nWidth, 58,SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW); 

	::SetWindowPos(hPrgs, NULL, 0, nHeight - 36, nWidth, 13, SWP_NOZORDER); 


	::SetWindowPos(hList, NULL, 0, 0, nWidth, nHeight - 97, SWP_NOZORDER | SWP_NOMOVE); 
	ListView_SetColumnWidth(hList, 0, nWidth - 257);

	::SetWindowPos(GetDlgItem(IDC_STATUS), NULL, 0, nHeight - 22, nWidth, 22, SWP_NOZORDER); 


	//MoveWindow(arrHwnd[ID_LIST], 0, 0, x , y/2 , TRUE);
	//ListView_SetColumnWidth(arrHwnd[ID_LIST], 0, x/2);
	//ListView_SetColumnWidth(arrHwnd[ID_LIST], 1, x/8);
	//ListView_SetColumnWidth(arrHwnd[ID_LIST], 2, x/3);

	//MoveWindow(arrHwnd[ID_EDIT_CRC32], 0, y/2 , x , y/2 - 30, TRUE);
	//MoveWindow(arrHwnd[ID_PROGRESS], 1 , y - 25 , x - 150 , 20 , TRUE);
	//MoveWindow(arrHwnd[ID_COMBO], x - 145 , y - 25 , 80 , 20 , TRUE);
	//MoveWindow(arrHwnd[ID_BTN_CALC], x - 63 , y - 28, 61, 25, TRUE);

	return TRUE;
}

BOOL TInstDlg::IsValidWndAndGetPath(TCHAR * szPath, BOOL isGetPath)
{

	HWND	hwndFoundWindow = NULL, hWndParent = NULL, hRootHwnd = NULL;
	TCHAR	sRootClass[MAX_PATH];//, sParentClass[MAX_PATH];//, sExeName[MAX_PATH];
	POINT	screenpoint;

	//DWORD	hProcessID;

	GetCursorPos (&screenpoint);

	hwndFoundWindow = WindowFromPoint (screenpoint);

	//hWndParent = GetAncestor(hwndFoundWindow, GA_PARENT);
	hRootHwnd = GetAncestor(hwndFoundWindow, GA_ROOT);

	//HWND	hWndDesk = GetShellWindow();

	//if(hWndDesk == hRootHwnd)//DeskTop
	//{
	//	if(isGetPath)
	//	{
	//		if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, 0, szPath))) 
	//		{
	//			return TRUE;
	//		}else{
	//			return FALSE;
	//		}
	//	}else{
	//		return TRUE;
	//	}
	//}


	GetClassName(hRootHwnd, sRootClass, MAX_PATH);

	//OutputDebugString(sRootClass);
	//OutputDebugString(TEXT("\r\n"));

	if(	(0 == lstrcmp(sRootClass, SHELL_LISTVIEW_ROOT_CLASS2)) || \
		(0 == lstrcmp(sRootClass, SHELL_LISTVIEW_ROOT_CLASS1))/* || \
		(GetClassName(hWndParent, sParentClass, MAX_PATH), 0 == lstrcmp(sParentClass, SHELL_LISTVIEW_PARENT_CLASS))*/)
	{
		if(isGetPath)
		{
			return GetWndPath(hRootHwnd, szPath);
		}else{
			return TRUE;
		}

	}else{
		//GetClassName(hWndParent, sParentClass, MAX_PATH);

		//if(0 == lstrcmp(sParentClass, SHELL_LISTVIEW_PARENT_CLASS))
		//{

		//	if(isGetPath)
		//	{
		//		return GetWndPath(hRootHwnd, szPath);
		//	}else{
		//		return TRUE;
		//	}

		//}else{
			return FALSE;
		//}
	}

}


#ifdef _USE_CUSTOMDRAW_
////////////////////////////////////////////DrawItem////////////////////////////////////////////////
BOOL TInstDlg::EvMeasureItem(UINT ctlID, MEASUREITEMSTRUCT *lpMis)
{

	switch(ctlID)
	{
		case IDC_LIST:

		//lpMI = (LPMEASUREITEMSTRUCT)lParam; 

		if(lpMis->CtlType == ODT_HEADER)
		{
			//lpMis->itemWidth = 0 ; 
			lpMis->itemHeight = 17;		//icon=16x16
		}

		

		break;

	//case IDC_CUSTOM1:

	//	if(lpMis->CtlType == ODT_MENU)
	//	{

	//		lpMis->itemHeight=50;
	//	}
	//	break;
	}


	return FALSE;
}

BOOL TInstDlg::EvDrawItem(UINT ctlID, DRAWITEMSTRUCT *lpDis)
{
	//HWND						hWndListView;
	HWND						hWndHeader;
	RECT						rc, rcfillrect; 
	HDC							hdc; 
	LVITEM						item;
	//LVCOLUMN					column;

	int							iColumnCount;
	TCHAR						szStrPrintf[MAX_PATH];

	//LPPCK_PATH_NODE				lpNodeToShow = NULL;

	/////////////////一些变量的初始化//////////////////

	//LPDRAWITEMSTRUCT lpDis = (LPDRAWITEMSTRUCT)lParam; 
	
	//lpDis->CtlID;					控件ID
	//lpDis->CtlType;				控件类型，ODT_LISTVIEW;ODT_HEADER;ODT_TAB;
	//lpDis->hDC;					控件HDC
	//lpDis->hwndItem;				控件HWND
	//lpDis->itemAction;			动作类型？？？
	//lpDis->itemData;				DATA，listview第0个subitem的字串
	//lpDis->itemID;				ListView第多少行的数据
	//lpDis->itemState;				状态，ODS_SELECTED（选中）ODS_FOCUS（）ODS_CHECKED（）
	//lpDis->rcItem;


	//hWndListView = lpDis->hwndItem;
	hWndHeader = ListView_GetHeader(lpDis->hwndItem);
	rcfillrect = lpDis->rcItem; 
	hdc = lpDis->hDC; 

	/////////////得到 ListView Header 的数量/////////////////
	iColumnCount = Header_GetItemCount(hWndHeader);////////////////此处有待优化

	/////////////////////////////////////////////////////

	if(lpDis->CtlType == ODT_LISTVIEW)
	{

		//if(ctlID == IDC_LIST_ICON)
		//{

		//	Debug((WCHAR*)lpDis->itemData);

		//}

		item.mask = /* | LVIF_STATE |*/ /*LVIF_PARAM | */LVIF_IMAGE;
		item.iItem = lpDis->itemID;
		item.iSubItem = 0;
		//item.stateMask = 0x000c;		// get all state flags
		ListView_GetItem(lpDis->hwndItem, &item);

		//lpNodeToShow = (LPPCK_PATH_NODE)item.lParam;

		
		if(lpDis->itemState & ODS_SELECTED)
		{
			//if( lpDis->hwndItem == GetFocus() )
				redraw=R_SELECT;
			//else
			//	redraw=R_SEL_NOFOCUS;
		}
		else
			redraw=R_NORMAL;


		rcfillrect.right++;
		rcfillrect.bottom--;


		SetBkMode(hdc,TRANSPARENT);

		//rcfillrect.right-=20;
		if( R_NORMAL == redraw)
			FillRect(hdc,&rcfillrect,hBrushs[item.iItem   &   1]);
		else
			FillRect(hdc,&rcfillrect,hBrushs[redraw]);

		//rcfillrect.right+=20;
		DrawIconEx(hdc, rcfillrect.left + 4, rcfillrect.top, hIconList[item.iImage], 16, 16, 0, 0, DI_NORMAL);
		
		rcfillrect.left+=6;

		item.cchTextMax = 256;
		item.pszText = szStrPrintf;


		for (item.iSubItem=0; item.iSubItem<iColumnCount; item.iSubItem++)
		{

			rcfillrect.right = rcfillrect.left + ListView_GetColumnWidth(lpDis->hwndItem, item.iSubItem);	//2.984

/*			column.mask = LVCF_WIDTH | LVCF_FMT;
			ListView_GetColumn(lpDis->hwndItem, item.iSubItem, &column);

			rcfillrect.right = rcfillrect.left + column.cx;	*/									//3.015

			rc = rcfillrect;
			rc.right-=5;

			//修正位置
			//if(1 == lpDis->CtlID)
			//{
			if(0 == item.iSubItem) 
				rc.left+=16;
			//}
			//int count = SendMessage(lpDis->hwndItem, LVM_GETITEMTEXT, (WPARAM)lpDis->itemID, (LPARAM)(LV_ITEM *)&item);
			//rcfillrect.right-=20;
			DrawText(hdc,szStrPrintf, ::SendMessage(lpDis->hwndItem, LVM_GETITEMTEXT, (WPARAM)lpDis->itemID, (LPARAM)&item) ,&rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
			//rcfillrect.right+=20;
			rcfillrect.left = rcfillrect.right;
		}

	}


	return FALSE;
}

#endif



BOOL TInstDlg::EvDropFiles(HDROP hDrop)
{
	
	if(lpPckParams->cVarParams.bThreadRunning)goto END_DROP;

	TCHAR	(*lpszFilePathPtr)[MAX_PATH];
	TCHAR	szFirstFile[MAX_PATH];

	m_DropFileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

	if(0 == m_DropFileCount)goto END_DROP;

	if(1 == m_DropFileCount)
	{
		if(!m_lpPckCenter->IsValidPck())
		{
			size_t	nFirstFileLength;
			DragQueryFile(hDrop, 0, szFirstFile, MAX_PATH);
			StringCchLength(szFirstFile, MAX_PATH, &nFirstFileLength);

			if(7 <= nFirstFileLength)
			{
				if(0 == lstrcmpi(szFirstFile + nFirstFileLength - 4, TEXT(".pck"))){

					OpenPckFile(szFirstFile);
					goto END_DROP;
				}else if(0 == lstrcmpi(szFirstFile + nFirstFileLength - 4, TEXT(".zup"))){

					OpenPckFile(szFirstFile);
					goto END_DROP;
				}
			}
		}
	}

	if(IDCANCEL == MessageBoxA("确定添加这些文件吗？", "询问", MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2))
		goto END_DROP;

	DragAcceptFiles(hWnd,FALSE);

	if(NULL == (m_lpszFilePath = (TCHAR(*)[MAX_PATH]) malloc (sizeof(TCHAR) * MAX_PATH * m_DropFileCount)))
	{
		m_lpPckCenter->PrintLogE(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
		goto END_DROP;
	}

	lpszFilePathPtr = m_lpszFilePath;

	for(DWORD i=0;i<m_DropFileCount;i++)
	{

		DragQueryFile(hDrop, i, *lpszFilePathPtr, MAX_PATH);

		lpszFilePathPtr++;
	}

	//mt_MaxMemoryCount = mt_MaxMemory;
	_beginthread(UpdatePckFile, 0, this);



END_DROP:
	DragFinish ( hDrop );
	DragAcceptFiles(hWnd,TRUE);
	return	TRUE;
}

BOOL TInstDlg::EventUser(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char szPrintf[256];

	switch(uMsg)
	{
	case WM_FRESH_MAIN_CAPTION:

		if(wParam){
#ifdef UNICODE
			sprintf_s(szPrintf, "%s - %s", THIS_MAIN_CAPTION, WtoA(m_lpPckCenter->GetCurrentVersionName()));
#else
			sprintf_s(szPrintf, "%s - %s", THIS_MAIN_CAPTION, m_lpPckCenter->GetCurrentVersionName());
#endif
			SetWindowTextA(szPrintf);
		}else{
			SetWindowTextA(THIS_MAIN_CAPTION);
		}

		break;

	}

	return FALSE;
}

//DWORD ShowErrorMsg ( CONST DWORD dwError )
//{
//	LPVOID lpMsgBuf; // temporary message buffer
//	TCHAR szMessage[500];
//
//	// retrieve a message from the system message table
//	if (!FormatMessage( 
//		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
//		FORMAT_MESSAGE_FROM_SYSTEM | 
//		FORMAT_MESSAGE_IGNORE_INSERTS,
//		NULL,
//		dwError,
//		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
//		(LPTSTR) &lpMsgBuf,
//		0,
//		NULL ))
//	{
//		return GetLastError();
//	}
//
//	// display the message in a message box
//	StringCchCopy(szMessage, 500, TEXT("Windows 给出的对错误的详细描述:\r\n\r\n"));
//	StringCchCat(szMessage, 500, (TCHAR *) lpMsgBuf);
//	MessageBox( NULL, szMessage, TEXT("错误信息"), MB_ICONEXCLAMATION | MB_OK );
//
//	// release the buffer FormatMessage allocated
//	LocalFree( lpMsgBuf );
//
//	return NOERROR;
//}

