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

#include "guipch.h"
#include "winmain.h"

void TInstDlg::initCommctrls()
{
	//ListView 初始化
	this->ListView_Init();

	//设置进度条的范围
	SendDlgItemMessage(IDC_PROGRESS, PBM_SETRANGE, (WPARAM)0, MAKELPARAM(0, 1024));

	//StatusBar,文件名，文件数，文件大小，进度，状态
	//int		iStatusWidth[] = {150, 250, 320, 530, 1024};
	constexpr int		iStatusWidth[] = { 150, 260, 350, 750, 1224 };
	SendDlgItemMessage(IDC_STATUS, SB_SETPARTS, 5, reinterpret_cast<LPARAM>(iStatusWidth));

#if 0
	this->SetStatusBarTitle(L"textures123456.pck");
	this->SetStatusBarFileSize(9999999999);
	this->SetStatusBarFileCount(9999999);
	this->SetStatusBarProgress(L"进度: 9999999/9999999 100.0% 缓存:9999 MB / 9999 MB 100.0%");
#endif

	//快捷键
	this->hAccel = LoadAcceleratorsW(TApp::GetInstance(), MAKEINTRESOURCEW(IDR_ACCELERATOR1));

	//任务栏进度
	::CoCreateInstance(
		CLSID_TaskbarList, NULL, CLSCTX_ALL,
		IID_ITaskbarList3, (void**)&this->m_pTaskBarlist);

	if(nullptr != this->m_pTaskBarlist)
		this->m_pTaskBarlist->SetProgressState(this->hWnd, TBPF_INDETERMINATE);

}

void TInstDlg::initParams()
{

	//打开、关闭、复原等事件注册
	pck_regMsgFeedback(this, MyFeedbackCallback);

	*this->m_szStrToSearch = 0;			//查找的文字
	this->bGoingToExit = FALSE;			//是否准备退出8

#ifdef _DEBUG
	pck_setMTMaxMemory((512 * 1024 * 1024));
#endif

	this->m_isListviewRenaming = FALSE;

	//光标加载
	this->m_isSearchWindow = FALSE;

#ifdef _WIN64
	this->m_hCursorOld = (HCURSOR)GetClassLong(this->hWnd, GCLP_HCURSOR);
#else
	this->m_hCursorOld = (HCURSOR)GetClassLong(this->hWnd, GCL_HCURSOR);
#endif


	this->m_hCursorAllow = LoadCursor(TApp::GetInstance(), MAKEINTRESOURCE(IDC_CURSOR_DROP));
	this->m_hCursorNo = LoadCursor(NULL, MAKEINTRESOURCE(IDC_NO));

}

//enum class ENABLE_TOOLBAR_CTL : int
//{
//
//};

struct TOOL_BAR_ST {
	WORD img_id;
	WORD id;
	WORD text;
	WORD command;
	int	 sub_menu = -1;
	BOOL enable_ctl = TRUE;
	BOOL enable_init = TRUE;
};

const auto get_wToolBarList() noexcept
{
	constexpr static TOOL_BAR_ST wToolBarList[] = {
		{0,		IDI_ICON_OPEN,		IDS_STRING_OPEN,			ID_MENU_OPEN, 0},
		{1,		IDI_ICON_NEW,		IDS_STRING_NEW,				ID_MENU_NEW, 0},
		{2,		IDI_ICON_CLOSE,		IDS_STRING_CLOSE,			ID_MENU_CLOSE, 0},
		{3,		IDI_ICON_APP, 0, 0, -1, FALSE},
		{4,		IDI_ICON_SAVE,		IDS_STRING_UNPACK_SELECTED, ID_MENU_UNPACK_SELECTED, 1},
		{5,		IDI_ICON_SAVEALL,	IDS_STRING_UNPACK_ALL,		ID_MENU_UNPACK_ALL, 1},
		{6,		IDI_ICON_APP, 0, 0, -1, FALSE},
		{7,		IDI_ICON_ADD,		IDS_STRING_ADD,				ID_MENU_ADD, 2},
		{8,		IDI_ICON_REBUILD,	IDS_STRING_REBUILD,			ID_MENU_REBUILD, 2},
		{9,		IDI_ICON_STRIPSAVE, IDS_STRING_STRIPPCK,		ID_MENU_SIMPLIFY, 2},
		{10,	IDI_ICON_APP, 0, 0, -1, FALSE},
		{11,	IDI_ICON_EDIT,		IDS_STRING_INFO,			ID_MENU_INFO, 2},
		{12,	IDI_ICON_SEARCH,	IDS_STRING_SEARCH,			ID_MENU_SEARCH, 2},
		{13,	IDI_ICON_OPT,		IDS_STRING_COMPRESS_OPT,	ID_MENU_COMPRESS_OPT, 2},
		{14,	IDI_ICON_APP, 0, 0, -1, FALSE},
		{15,	IDI_ICON_STOP,		IDS_STRING_CANCEL,			ID_MENU_CANCEL, 2, TRUE, FALSE},
		{16,	IDI_ICON_APP, 0, 0, -1, FALSE},
		{17,	IDI_ICON_INFO,		IDS_STRING_ABOUT,			ID_MENU_ABOUT, -1, FALSE},
		{18,	IDI_ICON_EXIT,		IDS_STRING_EXIT,			ID_MENU_EXIT, -1, FALSE},
	};
	return std::span{ wToolBarList };
}

void TInstDlg::initToolbar()
{
	auto wToolBarList = get_wToolBarList();

	//初始化工具栏
	this->m_imageList = ::ImageList_Create(32, 32, ILC_COLOR32, 1, wToolBarList.size());

	for (auto& toolbar : wToolBarList)
	{
		HICON hiconItem = LoadIcon(TApp::GetInstance(), MAKEINTRESOURCE(toolbar.id));
		ImageList_AddIcon(this->m_imageList, hiconItem);
		::DestroyIcon(hiconItem);
	}

	SendDlgItemMessage(IDC_TOOLBAR, TB_SETIMAGELIST, 0, reinterpret_cast<LPARAM>(this->m_imageList));
	SendDlgItemMessage(IDC_TOOLBAR, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

	TBBUTTON tbbtn{ 0 };

	for (auto& toolbar : wToolBarList)
	{

		if (0 == toolbar.command) [[unlikely]]
		{
			tbbtn.fsStyle = TBSTYLE_SEP;
			tbbtn.iString = -1;
			tbbtn.idCommand = 0;
			tbbtn.iBitmap = -1;
		}
		else [[likely]] {
			tbbtn.fsStyle = TBSTYLE_BUTTON;
			tbbtn.iString = reinterpret_cast<INT_PTR>(::GetLoadStrW(toolbar.text));
			tbbtn.idCommand = toolbar.command;
			tbbtn.iBitmap = toolbar.img_id;
			tbbtn.fsState = toolbar.enable_init ? TBSTATE_ENABLED : 0;
		}

		SendDlgItemMessage(IDC_TOOLBAR, TB_ADDBUTTONS, 1, reinterpret_cast<LPARAM>(&tbbtn));
	}

	::SetWindowPos(GetDlgItem(IDC_TOOLBAR), NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW);
}

void TInstDlg::EnbaleButtons(BOOL bEnbale)
{

	auto wToolBarList = get_wToolBarList();
	auto EnableButton = [this](UINT buttonID, BOOL enable) {
		return (BOOL)(this->SendDlgItemMessage(IDC_TOOLBAR, TB_ENABLEBUTTON, buttonID, MAKELONG(enable, 0)));
	};

	HMENU	hMenu = ::GetMenu(this->hWnd);

	for (auto& toolbar : wToolBarList)
	{
		if (toolbar.enable_ctl)
		{
			const BOOL set_enable = bEnbale ? toolbar.enable_init : !toolbar.enable_init;
			EnableButton(toolbar.command, set_enable);
			if(-1 != toolbar.sub_menu)
				::EnableMenuItem(::GetSubMenu(hMenu, toolbar.sub_menu), toolbar.command, set_enable ? MF_ENABLED : MF_GRAYED);
		}
	}

	hMenu = ::LoadMenuW(TApp::GetInstance(), (LPCTSTR)IDR_MENU_RCLICK);
	HMENU hSubMenu = ::GetSubMenu(hMenu, 0);

	constexpr WORD ctlRMenuIDs[] = {
		ID_MENU_VIEW,
		ID_MENU_UNPACK_SELECTED,
		ID_MENU_RENAME,
		ID_MENU_DELETE,
		ID_MENU_ATTR
	};

	for (const auto id : ctlRMenuIDs)
	{
		::EnableMenuItem(hSubMenu, id, bEnbale ? MF_ENABLED : MF_GRAYED);
	}

	return;
}

void TInstDlg::initArgument()
{
	try {

		//初始化浏览路径
		this->m_FolderBrowsed.clear();

		this->m_CurrentPath = fs::current_path().wstring();
		this->m_MyFileName.assign(__wargv[0]);

		if(__argc > 1) {
			this->m_Filename = std::move(fs::absolute(fs::path(__wargv[1])));
			this->OpenPckFile();
		}

	}
	catch(std::exception)
	{
		;
	}
}
