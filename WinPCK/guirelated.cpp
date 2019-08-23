//////////////////////////////////////////////////////////////////////
// guirelated.cpp: WinPCK 界面线程部分
// 界面初始化
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4312 )
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4005 )
#pragma warning ( disable : 4302 )


#include "winmain.h"
#include <tchar.h>
#pragma comment(lib,"comctl32.lib")

int initArgumentException(int code);

void TInstDlg::initCommctrls()
{
	//ListView 初始化
	ListView_Init();

	//设置进度条的范围
	SendDlgItemMessage(IDC_PROGRESS, PBM_SETRANGE, (WPARAM)0, MAKELPARAM(0, 1024));

	//StatusBar,文件名，文件数，文件大小，进度，状态
	//int		iStatusWidth[] = {150, 250, 320, 530, 1024};
	int		iStatusWidth[] = { 150, 260, 350, 750, 1224 };
	SendDlgItemMessage(IDC_STATUS, SB_SETPARTS, 5, (LPARAM)iStatusWidth);

	//SetStatusBarText(0, TEXT("textures123456.pck"));
	//SetStatusBarText(1, TEXT("大小: 9999999999"));
	//SetStatusBarText(2, TEXT("数量: 9999999"));
	//SetStatusBarText(3, TEXT("进度: 9999999/9999999 100.0% 缓存:9999 MB / 9999 MB 100.0%"));

	//快捷键
	hAccel = LoadAccelerators(TApp::GetInstance(), MAKEINTRESOURCE(IDR_ACCELERATOR1));

	//任务栏进度
	CoCreateInstance(
		CLSID_TaskbarList, NULL, CLSCTX_ALL,
		IID_ITaskbarList3, (void**)&m_pTaskBarlist);

	if(nullptr != m_pTaskBarlist)
		m_pTaskBarlist->SetProgressState(hWnd, TBPF_INDETERMINATE);

}

void TInstDlg::initParams()
{

	//打开、关闭、复原等事件注册
	pck_regMsgFeedback(this, MyFeedbackCallback);

	*m_szStrToSearch = 0;			//查找的文字
	bGoingToExit = FALSE;			//是否准备退出8

#ifdef _DEBUG
	pck_setMTMaxMemory((512 * 1024 * 1024));
#endif

	m_isListviewRenaming = FALSE;

	//光标加载
	m_isSearchWindow = FALSE;

	//Timer
	wcscpy_s(szTimerProcessingFormatString, GetLoadStrW(IDS_STRING_TIMERING));
	wcscpy_s(szTimerProcessedFormatString, GetLoadStrW(IDS_STRING_TIMEROK));


#ifdef _WIN64
	m_hCursorOld = (HCURSOR)GetClassLong(hWnd, GCLP_HCURSOR);
#else
	m_hCursorOld = (HCURSOR)GetClassLong(hWnd, GCL_HCURSOR);
#endif


	m_hCursorAllow = LoadCursor(TApp::GetInstance(), MAKEINTRESOURCE(IDC_CURSOR_DROP));
	m_hCursorNo = LoadCursor(NULL, MAKEINTRESOURCE(IDC_NO));

}

void TInstDlg::initToolbar()
{

	//初始化工具栏
	m_imageList = ImageList_Create(32, 32, ILC_COLOR32, 1, 20);

	WORD	wToolBarIDsList[] = { IDI_ICON_OPEN,
									IDI_ICON_NEW,
									IDI_ICON_CLOSE,
									IDI_ICON_APP,
									IDI_ICON_SAVE,
									IDI_ICON_SAVEALL,
									IDI_ICON_APP,
									IDI_ICON_ADD,
									IDI_ICON_REBUILD,
									IDI_ICON_APP,
									IDI_ICON_EDIT,
									IDI_ICON_SEARCH,
									IDI_ICON_OPT,
									IDI_ICON_APP,
									IDI_ICON_INFO,
									IDI_ICON_EXIT,
									IDI_ICON_STOP,
									0 };

	WORD	wToolBarTextList[] = { IDS_STRING_OPEN,
									IDS_STRING_NEW,
									IDS_STRING_CLOSE,
									0,
									IDS_STRING_UNPACK_SELECTED,
									IDS_STRING_UNPACK_ALL,
									0,
									IDS_STRING_ADD,
									IDS_STRING_REBUILD,
									0,
									IDS_STRING_INFO,
									IDS_STRING_SEARCH,
									IDS_STRING_COMPRESS_OPT,
									0,
									IDS_STRING_ABOUT,
									IDS_STRING_EXIT };

	WORD	wToolBarCommandID[] = { ID_MENU_OPEN,
									ID_MENU_NEW,
									ID_MENU_CLOSE,
									0,
									ID_MENU_UNPACK_SELECTED,
									ID_MENU_UNPACK_ALL,
									0,
									ID_MENU_ADD,
									ID_MENU_REBUILD,
									0,
									ID_MENU_INFO,
									ID_MENU_SEARCH,
									ID_MENU_COMPRESS_OPT,
									0,
									ID_MENU_ABOUT,
									ID_MENU_EXIT };

	WORD	*lpIDs = wToolBarIDsList;

	while(0 != *lpIDs) {

		HICON hiconItem = LoadIcon(TApp::GetInstance(), MAKEINTRESOURCE(*lpIDs));
		ImageList_AddIcon(m_imageList, hiconItem);
		DestroyIcon(hiconItem);

		lpIDs++;
	}

	SendDlgItemMessage(IDC_TOOLBAR, TB_SETIMAGELIST, 0, (LPARAM)m_imageList);
	SendDlgItemMessage(IDC_TOOLBAR, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

	TBBUTTON tbbtn;
	memset(&tbbtn, 0, sizeof(TBBUTTON));
	tbbtn.fsState = TBSTATE_ENABLED;

	lpIDs = wToolBarCommandID;
	WORD	*lpIDsText = wToolBarTextList;

	for(int i = 0;i < 16;i++) {
		if(0 != *lpIDs) {

			//SendDlgItemMessage(IDC_TOOLBAR, TB_ADDSTRING, (WPARAM) 0/*TApp::GetInstance()*/, (LPARAM) GetLoadStr(*lpIDsText)/*MAKELONG (*lpIDsText, 0)*/);
			tbbtn.fsStyle = TBSTYLE_BUTTON;
			tbbtn.iString = (INT_PTR)GetLoadStr(*lpIDsText);
			tbbtn.idCommand = *lpIDs;
			tbbtn.iBitmap = i;
		} else {
			SendDlgItemMessage(IDC_TOOLBAR, TB_ADDSTRING, (WPARAM)0, (LPARAM)TEXT("\0"));
			tbbtn.fsStyle = TBSTYLE_SEP;
			tbbtn.iString = -1;
			tbbtn.idCommand = 0;
			tbbtn.iBitmap = -1;
		}

		SendDlgItemMessage(IDC_TOOLBAR, TB_ADDBUTTONS, 1, (LPARAM)&tbbtn);

		lpIDsText++;
		lpIDs++;

	}

	::SetWindowPos(GetDlgItem(IDC_TOOLBAR), NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW);

	//TBBUTTONINFO tbbtninfo = {0};
	//tbbtninfo.cbSize = sizeof(TBBUTTONINFO);
	//tbbtninfo.dwMask = TBIF_IMAGE | /*TBIF_TEXT | */TBIF_LPARAM | TBIF_SIZE;

	//SendDlgItemMessage(IDC_TOOLBAR, TB_GETBUTTONINFO, /*ID_MENU_OPEN*/ID_MENU_EXIT, (LPARAM)&tbbtninfo);


	//tbbtninfo.dwMask = TBIF_IMAGE | TBIF_TEXT;
	//tbbtninfo.iImage = 16;
	//tbbtninfo.pszText = TEXT("asdasdas");
	//SendDlgItemMessage(IDC_TOOLBAR, TB_SETBUTTONINFO, /*ID_MENU_OPEN*/ID_MENU_EXIT, (LPARAM)&tbbtninfo);

}

void TInstDlg::EnbaleButtons(int ctlExceptID, BOOL bEnbale)
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


#if 0
#ifdef UNICODE
#define CommandLineToArgv CommandLineToArgvW
#else
#define CommandLineToArgv CommandLineToArgvA
#endif

#ifndef UNICODE
PCHAR*  CommandLineToArgvA(
	PCHAR CmdLine,
	int* _argc
)
{
	PCHAR* argv;
	PCHAR  _argv;
	ULONG   len;
	ULONG   argc;
	CHAR   a;
	ULONG   i, j;

	BOOLEAN  in_QM;
	BOOLEAN  in_TEXT;
	BOOLEAN  in_SPACE;

	len = strlen(CmdLine);
	i = ((len + 2) / 2) * sizeof(PVOID) + sizeof(PVOID);

	argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,
		i + (len + 2) * sizeof(CHAR));

	_argv = (PCHAR)(((PUCHAR)argv) + i);

	argc = 0;
	argv[argc] = _argv;
	in_QM = FALSE;
	in_TEXT = FALSE;
	in_SPACE = TRUE;
	i = 0;
	j = 0;

	while(a = CmdLine[i]) {
		if(in_QM) {
			if(a == '\"') {
				in_QM = FALSE;
			} else {
				_argv[j] = a;
				j++;
			}
		} else {
			switch(a) {
			case '\"':
				in_QM = TRUE;
				in_TEXT = TRUE;
				if(in_SPACE) {
					argv[argc] = _argv + j;
					argc++;
				}
				in_SPACE = FALSE;
				break;
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				if(in_TEXT) {
					_argv[j] = '\0';
					j++;
				}
				in_TEXT = FALSE;
				in_SPACE = TRUE;
				break;
			default:
				in_TEXT = TRUE;
				if(in_SPACE) {
					argv[argc] = _argv + j;
					argc++;
				}
				_argv[j] = a;
				j++;
				in_SPACE = FALSE;
				break;
			}
		}
		i++;
	}
	_argv[j] = '\0';
	argv[argc] = NULL;

	(*_argc) = argc;
	return argv;
}
#endif
#endif
void TInstDlg::initArgument()
{
	__try {

		//初始化浏览路径

		//memset(&m_PathDirs, 0, sizeof(m_PathDirs));
		//*m_PathDirs.lpszDirNames = m_PathDirs.szPaths;

		*m_FolderBrowsed = 0;

		*m_CurrentPath = 0;
		GetCurrentDirectory(MAX_PATH, m_CurrentPath);

		//wchar_t	sargc[256];
		//StringCchPrintf(sargc, 256, L"argc = %d", argc);

		//MessageBox(sargc);
		//MessageBox(__targv[0]);
		//MessageBox(__targv[1]);

		_tcscpy_s(m_MyFileName, __targv[0]);

		if(__argc > 1) {
			_tcscpy_s(m_Filename, __targv[1]);
		}

		//初始化路径
		if(2 > __argc) {
			_tcscpy_s(m_Filename, m_CurrentPath);
			_tcscat_s(m_Filename, TEXT("\\"));
		} else {
			OpenPckFile(m_Filename);
		}

	}
	__except(initArgumentException(GetExceptionCode()))
	{
		;
	}
}

int initArgumentException(int code)
{

	switch(code) {
	case EXCEPTION_ACCESS_VIOLATION:
		MessageBoxA(NULL, "存储保护异常 在 initArgument", NULL, MB_OK);
		return EXCEPTION_EXECUTE_HANDLER;
		//return EXCEPTION_CONTINUE_EXECUTION;

	default:
		return 0;
	}

}