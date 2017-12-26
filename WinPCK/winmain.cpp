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

#include "tlib.h"
#include "resource.h"
#include "globals.h"
#include "winmain.h"
#include <process.h>
#include <shlobj.h>
#include <tchar.h>


BOOL GetWndPath(HWND hWnd, TCHAR * szPath);

/*
	WinMain
*/
int WINAPI _tWinMain(HINSTANCE hI, HINSTANCE, LPTSTR cmdLine, int nCmdShow)
{
	return	TInstApp(hI, cmdLine, nCmdShow).Run();
}


TInstApp::TInstApp(HINSTANCE _hI, LPTSTR _cmdLine, int _nCmdShow) : TApp(_hI, _cmdLine, _nCmdShow) {}

TInstApp::~TInstApp() {}

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

TInstDlg::TInstDlg(LPTSTR cmdLine) : TDlg(IDD_MAIN)//, staticText(this)
{}

TInstDlg::~TInstDlg() {}

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
	MoveWindow((cx - xsize) / 2, (cy - ysize) / 2, xsize, ysize, TRUE);

	//界面和数据初始化
	OleInitialize(0);
	SetWindowTextA(THIS_MAIN_CAPTION);
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
	//显示窗口
	Show();

	return	TRUE;
}


BOOL TInstDlg::EvClose()
{

	if(bGoingToExit)return FALSE;

	SetStatusBarText(0, GetLoadStr(IDS_STRING_EXITING));

	if(lpPckParams->cVarParams.bThreadRunning) {
		if(IDNO == MessageBox(GetLoadStr(IDS_STRING_ISEXIT), GetLoadStr(IDS_STRING_ISEXITTITLE), MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON2))return FALSE;

		bGoingToExit = TRUE;
		lpPckParams->cVarParams.bThreadRunning = FALSE;
		return FALSE;
	}

	ShowWindow(SW_HIDE);

	OleUninitialize();

	ListView_Uninit();

	delete m_lpPckCenter;
	delete logdlg;

	::PostQuitMessage(0);
	return FALSE;
}


BOOL TInstDlg::EvTimer(WPARAM timerID, TIMERPROC proc)
{
	switch(timerID) {
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

	switch(wID) {
	case IDOK:
		return	TRUE;
	case IDCANCEL:
		return	TRUE;
	case ID_OPEN_PCK:
	case ID_MENU_OPEN:
		OpenPckFile();
		break;
	case ID_MENU_UNPACK_ALL:
		UnpackAllFiles();
		break;
	case ID_MENU_UNPACK_SELECTED:
		UnpackSelectedFiles();
		break;
	case ID_CLOSE_PCK:
	case ID_MENU_CLOSE:
		if(lpPckParams->cVarParams.bThreadRunning)
			lpPckParams->cVarParams.bThreadRunning = FALSE;
		ListView_DeleteAllItems(GetDlgItem(IDC_LIST));
		m_lpPckCenter->Close();
		break;
	case ID_MENU_INFO:
		if(m_lpPckCenter->IsValidPck()) {
			TInfoDlg	dlg(m_lpPckCenter->GetAdditionalInfo(), this);
			if(dlg.Exec() == TRUE) {
				m_lpPckCenter->SetAdditionalInfo();
			}
		}
		break;
	case ID_MENU_SEARCH:
		if(m_lpPckCenter->IsValidPck()) {
			TSearchDlg	dlg(m_szStrToSearch, this);
			if(dlg.Exec() == TRUE) {
				SearchPckFiles();
			}
		}
		break;
	case ID_CREATE_NEWPCK:
	case ID_MENU_NEW:
		if(lpPckParams->cVarParams.bThreadRunning) {
			lpPckParams->cVarParams.bThreadRunning = FALSE;
			EnableButton(wID, FALSE);
		} else {
			_beginthread(CreateNewPckFile, 0, this);
		}
		break;
	case ID_ADD_FILE_TO_PCK:
	case ID_MENU_ADD:
		if(lpPckParams->cVarParams.bThreadRunning) {
			lpPckParams->cVarParams.bThreadRunning = FALSE;
			EnableButton(wID, FALSE);
		} else {
			AddFiles();
		}
		break;
	case ID_MENU_REBUILD:
		if(lpPckParams->cVarParams.bThreadRunning) {
			lpPckParams->cVarParams.bThreadRunning = FALSE;
			EnableButton(wID, FALSE);
		} else {

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
		lpPckParams->cVarParams.dwMTMemoryUsed = 0;
		_beginthread(DeleteFileFromPckFile, 0, this);
		break;
	case ID_LISTVIEW_SELECT_ALL:
	case ID_MENU_SELECTALL:
	{
		LVITEM item = { 0 };

		item.mask = LVIF_STATE;
		item.stateMask = item.state = LVIS_SELECTED;

		HWND hList = GetDlgItem(IDC_LIST);

		int	nItemCount = ListView_GetItemCount(hList);

		for(item.iItem = 1; item.iItem < nItemCount; item.iItem++) {
			ListView_SetItem(hList, &item);
		}
	}
	break;

	case ID_LISTVIEW_SELECT_REV:
	case ID_MENU_SELECTORP:
	{
		LVITEM item = { 0 };

		item.mask = LVIF_STATE;
		item.stateMask = LVIS_SELECTED;

		HWND hList = GetDlgItem(IDC_LIST);

		int	nItemCount = ListView_GetItemCount(hList);

		for(item.iItem = 1; item.iItem < nItemCount; item.iItem++) {
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
		MessageBoxA(THIS_MAIN_CAPTION
			"\r\n"
			THIS_DESC
			"\r\n\r\n"
			THIS_AUTHOR
			"\r\n\r\n"
			THIS_UESDLIB,
			"关于 "
			THIS_NAME,
			MB_OK | MB_ICONASTERISK);
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

VOID TInstDlg::SetStatusBarText(int	iPart, LPCSTR	lpszText)
{
	SendDlgItemMessageA(IDC_STATUS, SB_SETTEXTA, iPart, (LPARAM)lpszText);
	SendDlgItemMessageA(IDC_STATUS, SB_SETTIPTEXTA, iPart, (LPARAM)lpszText);
}

VOID TInstDlg::SetStatusBarText(int	iPart, LPCWSTR	lpszText)
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



BOOL TInstDlg::EvNotify(UINT ctlID, NMHDR *pNmHdr)
{
	switch(ctlID) {
	case IDC_LIST:
		EvNotifyListView(pNmHdr);
		break;
	}
	return FALSE;
}

BOOL TInstDlg::EventButton(UINT uMsg, int nHitTest, POINTS pos)
{
	switch(uMsg) {
	case WM_LBUTTONUP:

		if(m_isSearchWindow) {
			TCHAR szPath[MAX_PATH];

			//DoMouseUp();
			SetCursor(m_hCursorOld);
			ReleaseCapture();

			m_isSearchWindow = FALSE;

			if(IsValidWndAndGetPath(szPath, TRUE)) {
				if(m_lpPckCenter->IsValidPck()) {
					if(!lpPckParams->cVarParams.bThreadRunning) {
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
	if(m_isSearchWindow) {

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

	::SetWindowPos(GetDlgItem(IDC_TOOLBAR), NULL, 0, 0, nWidth, 58, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW);
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

	GetCursorPos(&screenpoint);

	hwndFoundWindow = WindowFromPoint(screenpoint);

	//hWndParent = GetAncestor(hwndFoundWindow, GA_PARENT);
	hRootHwnd = GetAncestor(hwndFoundWindow, GA_ROOT);

	//HWND	hWndDesk = GetShellWindow();

	//if(hWndDesk == hRootHwnd) {
	//	if(isGetPath) {
	//		if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, 0, szPath))) {
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

	if((0 == lstrcmp(sRootClass, SHELL_LISTVIEW_ROOT_CLASS2)) || \
		(0 == lstrcmp(sRootClass, SHELL_LISTVIEW_ROOT_CLASS1))/* || \
		(GetClassName(hWndParent, sParentClass, MAX_PATH), 0 == lstrcmp(sParentClass, SHELL_LISTVIEW_PARENT_CLASS))*/) {
		if(isGetPath) {
			return GetWndPath(hRootHwnd, szPath);
		} else {
			return TRUE;
		}

	} else {
		//GetClassName(hWndParent, sParentClass, MAX_PATH);

		//if(0 == lstrcmp(sParentClass, SHELL_LISTVIEW_PARENT_CLASS)) {

		//	if(isGetPath) {
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
	switch(ctlID) {
	case IDC_LIST:

		//lpMI = (LPMEASUREITEMSTRUCT)lParam; 
		if(lpMis->CtlType == ODT_HEADER) {
			//lpMis->itemWidth = 0 ; 
			lpMis->itemHeight = 17;		//icon=16x16
		}
		break;
	}

	return FALSE;
}

BOOL TInstDlg::EvDrawItem(UINT ctlID, DRAWITEMSTRUCT *lpDis)
{
	switch(ctlID) {
	case IDC_LIST:

		return EvDrawItemListView(lpDis);
		break;
	}
	return FALSE;
}
#endif



BOOL TInstDlg::EvDropFiles(HDROP hDrop)
{

	if(lpPckParams->cVarParams.bThreadRunning)goto END_DROP;

	TCHAR(*lpszFilePathPtr)[MAX_PATH];
	TCHAR	szFirstFile[MAX_PATH];

	m_DropFileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

	if(0 == m_DropFileCount)goto END_DROP;

	if(1 == m_DropFileCount) {
		if(!m_lpPckCenter->IsValidPck()) {
			size_t	nFirstFileLength;
			DragQueryFile(hDrop, 0, szFirstFile, MAX_PATH);
			nFirstFileLength = _tcsnlen(szFirstFile, MAX_PATH);

			if(7 <= nFirstFileLength) {
				if(0 == lstrcmpi(szFirstFile + nFirstFileLength - 4, TEXT(".pck"))) {

					OpenPckFile(szFirstFile);
					goto END_DROP;
				} else if(0 == lstrcmpi(szFirstFile + nFirstFileLength - 4, TEXT(".zup"))) {

					OpenPckFile(szFirstFile);
					goto END_DROP;
				}
			}
		}
	}

	if(IDCANCEL == MessageBoxA("确定添加这些文件吗？", "询问", MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2))
		goto END_DROP;

	DragAcceptFiles(hWnd, FALSE);

	if(NULL == (m_lpszFilePath = (TCHAR(*)[MAX_PATH]) malloc(sizeof(TCHAR) * MAX_PATH * m_DropFileCount))) {
		m_lpPckCenter->PrintLogE(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
		goto END_DROP;
	}

	lpszFilePathPtr = m_lpszFilePath;

	for(DWORD i = 0; i < m_DropFileCount; i++) {

		DragQueryFile(hDrop, i, *lpszFilePathPtr, MAX_PATH);

		lpszFilePathPtr++;
	}

	_beginthread(UpdatePckFile, 0, this);


END_DROP:
	DragFinish(hDrop);
	DragAcceptFiles(hWnd, TRUE);
	return	TRUE;
}

BOOL TInstDlg::EventUser(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char szPrintf[256];

	switch(uMsg) {
	case WM_FRESH_MAIN_CAPTION:

		if(wParam) {
#ifdef UNICODE
			sprintf_s(szPrintf, "%s - %s", THIS_MAIN_CAPTION, WtoA(m_lpPckCenter->GetCurrentVersionName()));
#else
			sprintf_s(szPrintf, "%s - %s", THIS_MAIN_CAPTION, m_lpPckCenter->GetCurrentVersionName());
#endif
			SetWindowTextA(szPrintf);
		} else {
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

