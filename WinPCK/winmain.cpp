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

#include <CommDlg/CRichEdit.h>

#include "guipch.h"
#include "winmain.h"
#include <UnknownExceptionFilter.h>

void clean_exit_works()
{
	//Logger->set_level(spdlog::level::debug);
	Logger->critical("系统发生严重错误，正在退出...");
	Logger->flush();
}

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
	maindlg->InitRichEdit();
	maindlg->Create();

}

TInstDlg::TInstDlg(LPTSTR cmdLine) :
	TDlg(IDD_MAIN),
	m_logdlg(this)
{}

TInstDlg::~TInstDlg() {}

/*
	儊僀儞僟僀傾儘僌梡 WM_INITDIALOG 張棟儖乕僠儞
*/
BOOL TInstDlg::EvCreate(LPARAM lParam)
{

	InstallExceptionFilter(clean_exit_works);

	int		cx = ::GetSystemMetrics(SM_CXFULLSCREEN), cy = ::GetSystemMetrics(SM_CYFULLSCREEN);

	//GetWindowRect(&rect);
	//int		xsize = rect.right - rect.left, ysize = rect.bottom - rect.top;

	int		xsize = 1100, ysize = 800;

	::SetClassLong(hWnd, GCL_HICON,
		(LONG_PTR)::LoadIcon(TApp::GetInstance(), (LPCTSTR)IDI_ICON_APP));
	//MoveWindow((cx - xsize) / 2, (cy - ysize) / 2, xsize, ysize, TRUE);

	//界面和数据初始化
	SetWindowTextW(_CRT_WIDE(THIS_MAIN_CAPTION));

	//添加回调
	Logger.append_callback_sink(std::bind(&TInstDlg::log_callback, this, std::placeholders::_1));

	//初始化数据
	initParams();

	OleInitialize(0);
	//初始化公共控件
	initCommctrls();
	//初始化工具栏
	initToolbar();
	//初始化日志窗口
	InitLogWindow();
	//初始化浏览路径
	initArgument();
	//显示窗口
	Show();

	MoveWindow((cx - xsize) / 2, (cy - ysize) / 2, xsize, ysize, TRUE);

	return	TRUE;
}


BOOL TInstDlg::EvClose()
{

	if(this->bGoingToExit)return FALSE;

	//SetStatusBarText(0, GetLoadStr(IDS_STRING_EXITING));
	SetStatusBarInfo(GetLoadStr(IDS_STRING_EXITING));

	if(pck_isThreadWorking()) {
		if(IDNO == MessageBox(GetLoadStr(IDS_STRING_ISEXIT), GetLoadStr(IDS_STRING_ISEXITTITLE), MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON2))return FALSE;

		this->bGoingToExit = TRUE;
		pck_forceBreakThreadWorking();
		return FALSE;
	}

	ShowWindow(SW_HIDE);

	if (nullptr != m_pTaskBarlist)
		m_pTaskBarlist->Release();

	OleUninitialize();

	ListView_Uninit();

	//delete logdlg;

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
	主对话框WM_COMMAND处理程序
*/
BOOL TInstDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{

	switch(wID) {
	case IDOK:
		return	TRUE;
	case IDCANCEL:
		return	TRUE;
	case ID_OPEN_PCK: //[[fallthrough]]
	case ID_MENU_OPEN:
		this->NewPckFile();
		break;
	case ID_MENU_UNPACK_ALL:
		this->UnpackAllFiles();
		break;
	case ID_MENU_UNPACK_SELECTED:
		this->UnpackSelectedFiles();
		break;
	case ID_CLOSE_PCK://[[fallthrough]]
	case ID_MENU_CLOSE:
		this->MenuClose();
		break;
	case ID_MENU_INFO:
		this->MenuInfo();
		break;
	case ID_MENU_SEARCH:
		this->MenuSearch();
		break;
	case ID_CREATE_NEWPCK: //[[fallthrough]]
	case ID_MENU_NEW:
		this->MenuNew(wID);
		break;
	case ID_ADD_FILE_TO_PCK: //[[fallthrough]]
	case ID_MENU_ADD:
		this->MenuAdd(wID);
		break;
	case ID_MENU_REBUILD:
		this->MenuRebuild(wID);
		break;
	case ID_MENU_SIMPLIFY:
		this->MenuStrip();
		break;
	case ID_MENU_COMPRESS_OPT:
		this->MenuCompressOpt();
		break;
	case ID_MENU_RENAME:
		this->MenuRename();
		break;
	case ID_MENU_DELETE:
		this->MenuDelete();
		break;
	case ID_MENU_SELECTALL:
		this->MenuSelectAll();
		break;
	case ID_MENU_SELECTORP:
		this->MenuSelectReverse();
		break;
	case ID_MENU_ATTR:
		this->MenuAttribute();
		break;
	case ID_MENU_EXIT:
		this->SendMessage(WM_CLOSE, 0, 0);
		break;
	case ID_MENU_VIEW:
		this->MenuView();
		break;
	case ID_MENU_SETUP:
		this->AddSetupReg();
		break;
	case ID_MENU_UNSETUP:
		this->DeleteSetupReg();
		break;
	case ID_MENU_ABOUT:
		this->MenuAbout();
		break;
	case ID_MENU_LOG:
		this->m_logdlg.Show();
		break;
	case ID_MENU_CANCEL:
		this->MenuCancelPckOper();
		break;
	case ID_LISTVIEW_ENTER:
		this->ListViewEnter();
		break;
	//case ID_LISTVIEW_BACK:
	//	DbClickListView(0);
	//	break;
	//case ID_LISTVIEW_POPMENU:
	//	PopupRightMenu(m_lpPckCenter->GetListCurrentHotItem());
	//	break;
	}
	return	FALSE;
}


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
			wchar_t szPath[MAX_PATH];

			//DoMouseUp();
			SetCursor(m_hCursorOld);
			ReleaseCapture();

			m_isSearchWindow = FALSE;

			if(IsValidWndAndGetPath(szPath, TRUE)) {
				if(pck_IsValidPck()) {
					if(!pck_isThreadWorking()) {

						this->m_CurrentPath.assign(szPath);
						pck_setMTMaxMemory(0);

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
	HWND	hToolbar = GetDlgItem(IDC_TOOLBAR);

	RECT rectToolbar;

	GetClientRect(hToolbar, &rectToolbar);

	::SetWindowPos(hToolbar, NULL, 0, 0, nWidth, rectToolbar.bottom, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW);
	::SetWindowPos(GetDlgItem(IDC_STATUS), NULL, 0, nHeight - 22, nWidth, 22, SWP_NOZORDER);

	::SetWindowPos(hPrgs, NULL, 0, nHeight - 36, nWidth, 13, SWP_NOZORDER);

	::SetWindowPos(hList, NULL, 0, rectToolbar.bottom + 6, nWidth, nHeight - (36 + rectToolbar.bottom + 6), SWP_NOZORDER);
	ListView_SetColumnWidth(hList, 0, nWidth - 257);

	return TRUE;
}

/*
	szPath 存放获取的目标窗口路径
	isGetPath 是否需要获取目标窗口路径
*/
BOOL TInstDlg::IsValidWndAndGetPath(wchar_t * szPath, BOOL isGetPath)
{

	HWND	hwndFoundWindow = NULL, hWndParent = NULL, hRootHwnd = NULL;
	TCHAR	sRootClass[MAX_PATH];//, sParentClass[MAX_PATH];//, sExeName[MAX_PATH];
	POINT	screenpoint;
	//DWORD	hProcessID;

	GetCursorPos(&screenpoint);

	hwndFoundWindow = WindowFromPoint(screenpoint);

	//hWndParent = GetAncestor(hwndFoundWindow, GA_PARENT);
	hRootHwnd = GetAncestor(hwndFoundWindow, GA_ROOT);

	if(GetShellWindow() == hRootHwnd) {
		if(isGetPath) {
			if(SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_DESKTOP, NULL, 0, szPath))) {
				return TRUE;
			}else{
				return FALSE;
			}
		}else{
			return TRUE;
		}
	}

	GetClassName(hRootHwnd, sRootClass, MAX_PATH);
#ifdef _DEBUG
	Debug(TEXT("sRootClass:%s\r\n"), sRootClass);
#endif
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

	if (pck_isThreadWorking()) {
		DragFinish(hDrop);
		return	TRUE;
	}

	wchar_t	szFirstFile[MAX_PATH];

	auto dwDropFileCount = ::DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);

	if (0 == dwDropFileCount) {

		::DragFinish(hDrop);
		return	TRUE;
	}

	//没打开pck文件，且只拖入了一个文件，如果后缀为zup 或 pck，视为打开pck文件
	if(1 == dwDropFileCount) {
		if(!pck_IsValidPck()) {
			::DragQueryFileW(hDrop, 0, szFirstFile, MAX_PATH);

			fs::path one_file_name(szFirstFile);

			std::wstring valid_pck_ext(L".pck;.zup;");

			if (one_file_name.has_extension()) {
				if (std::wstring::npos != valid_pck_ext.find(one_file_name.extension().wstring())) {

					this->OpenPckFile(szFirstFile);
					::DragFinish(hDrop);
					return	TRUE;
				}
			}
		}
	}

	if(IDCANCEL == MessageBoxW(L"确定添加这些文件吗？", L"询问", MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2)) {

		DragFinish(hDrop);
		return	TRUE;
	}

	DragAcceptFiles(hWnd, FALSE);

	m_lpszFilePath.clear();
	m_lpszFilePath.reserve(dwDropFileCount);

	for(auto i = 0; i < dwDropFileCount; i++) {

		wchar_t szFile[MAX_PATH];
		::DragQueryFileW(hDrop, i, szFile, MAX_PATH);
		this->m_lpszFilePath.emplace_back(szFile);
	}

	_beginthread(UpdatePckFile, 0, this);
	DragFinish(hDrop);
	return	TRUE;
}
