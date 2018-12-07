
#include "globals.h"
#include "pck_handle.h"
#include "miscdlg.h"

class TInstDlg : public TDlg
{
protected:
	//TSubClassCtl	staticText;
	//TInstSheet		*propertySheet;
	//InstallCfg		cfg;
	TLogDlg			*logdlg;

public:
	TInstDlg(LPTSTR cmdLine);
	virtual ~TInstDlg();

	virtual BOOL	EvCreate(LPARAM lParam);
	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvClose();
	virtual BOOL	EvTimer(WPARAM timerID, TIMERPROC proc);
	virtual BOOL	EvNotify(UINT ctlID, NMHDR *pNmHdr);
#ifdef _USE_CUSTOMDRAW_
	virtual BOOL	EvMeasureItem(UINT ctlID, MEASUREITEMSTRUCT *lpMis);
	virtual BOOL	EvDrawItem(UINT ctlID, DRAWITEMSTRUCT *lpDis);
#endif

	virtual BOOL	EvDropFiles(HDROP hDrop);

	virtual BOOL	EventButton(UINT uMsg, int nHitTest, POINTS pos);
	virtual BOOL	EvMouseMove(UINT fwKeys, POINTS pos);

	virtual BOOL	EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight);
	//virtual BOOL	EventUser(UINT uMsg, WPARAM wParam, LPARAM lParam);

	//virtual BOOL	EventKey(UINT uMsg, int nVirtKey, LONG lKeyData);

	//virtual BOOL	EvSysCommand(WPARAM uCmdType, POINTS pos);
	//virtual BOOL	
#if 0
	virtual BOOL	EvNcDestroy(void);
#endif

	//BOOL	RunAsAdmin(BOOL is_imme);
	//void	ChangeMode(void);



//用户变量
private:

	TCHAR	m_MyFileName[MAX_PATH];
	TCHAR	m_Filename[MAX_PATH], m_CurrentPath[MAX_PATH];

	BOOL	m_isListviewRenaming;

	int		m_iListHotItem;	//当前鼠标在listview上的选中数据行

	const PCK_UNIFIED_FILE_ENTRY*	m_currentNodeOnShow;

	BOOL	bGoingToExit;
	wchar_t	m_szStrToSearch[256];

	//addmode
	vector<tstring>	m_lpszFilePath;

	//用于找窗口的变量
	BOOL	m_isSearchWindow;

	HCURSOR	m_hCursorOld, m_hCursorAllow, m_hCursorNo;

	wchar_t		m_FolderBrowsed[MAX_PATH];

	HIMAGELIST	m_imageList;
	_inline BOOL EnableButton(UINT buttonID, BOOL enable) { return (BOOL)(SendDlgItemMessage(IDC_TOOLBAR, TB_ENABLEBUTTON, buttonID, MAKELONG(enable, 0))); }

	//Timer String
	wchar_t		szTimerProcessingFormatString[64];
	wchar_t		szTimerProcessedFormatString[64];

	//用户函数
private:

	//winmain.cpp
	VOID SetStatusBarText(int iPart, LPCSTR lpszText);
	VOID SetStatusBarText(int iPart, LPCWSTR lpszText);
	BOOL IsValidWndAndGetPath(wchar_t * szPath, BOOL isGetPath = FALSE);
	void RefreshProgress();
	TCHAR* BuildSaveDlgFilterString();


	//threadproc.cpp
	void EnbaleButtons(int ctlExceptID, BOOL bEnbale);
	static VOID UpdatePckFile(VOID *pParam);
	static VOID RenamePckFile(VOID *pParam);
	static VOID RebuildPckFile(VOID	*pParam);
	static VOID CreateNewPckFile(VOID *pParam);
	static VOID ToExtractAllFiles(VOID *pParam);
	static VOID ToExtractSelectedFiles(VOID	*pParam);
	static VOID DeleteFileFromPckFile(VOID *pParam);


	//mainfunc.cpp
	BOOL OpenPckFile(TCHAR *lpszFileToOpen = TEXT(""), BOOL isReOpen = FALSE);
	VOID SearchPckFiles();
	VOID ShowPckFiles(const PCK_UNIFIED_FILE_ENTRY* lpNodeToShow);


	//guilated.cpp
	void		initCommctrls();
	void		initParams();
	void		initToolbar();
	void		initArgument();


	//helpfunc.cpp
	VOID ViewFileAttribute();
	VOID ViewFile(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry);
	BOOL AddFiles();
	void AddSetupReg();
	void DeleteSetupReg();

	void InitLogWindow();

	const PCK_UNIFIED_FILE_ENTRY* GetFileEntryByItem(int itemIndex);

	void DbClickListView(const int itemIndex);	//进入列表中的itemIndex项（进入目录或预览文件）
	void PopupRightMenu(const int itemIndex);		//listview上右击出菜单

	void UnpackAllFiles();					//解压所有文件
	void UnpackSelectedFiles();				//解压选中的文件

	//打开、关闭、复原等事件注册
	static int MyFeedbackCallback(void* pTag, int eventId, WPARAM wParam, LPARAM lParam);


	//listViewFunc.cpp

	BOOL EvNotifyListView(const NMHDR *pNmHdr);
	BOOL EvDrawItemListView(const DRAWITEMSTRUCT *lpDis);

public:
	void InsertList(CONST HWND hWndList, CONST INT iIndex, CONST UINT uiMask, CONST INT iImage, CONST LPVOID lParam, CONST INT nColCount, ...);
private:
	BOOL InitListView(CONST HWND hWndListView, const LPTSTR *lpszText, const int *icx, const int *ifmt);

	BOOL ListView_BeginLabelEdit(const HWND hWndList, LPARAM lParam);
	BOOL ListView_EndLabelEdit(const NMLVDISPINFO* pNmHdr);

	void ListView_Init();
	void ListView_Uninit();

	void ProcessColumnClick(CONST HWND hWndList, CONST NMLISTVIEW * pnmlistview, DWORD& dwSortStatus);

	//menu
	void MenuClose();
	void MenuInfo();
	void MenuSearch();
	void MenuNew(WORD wID);
	void MenuAdd(WORD wID);
	void MenuRebuild(WORD wID);
	void MenuCompressOpt();
	void MenuRename();
	void MenuDelete();
	void MenuSelectAll();
	void MenuSelectReverse();
	void MenuAttribute();
	void MenuView();
	void MenuAbout();
	void ListViewEnter();

};

class TInstApp : public TApp
{
public:
	TInstApp(HINSTANCE _hI, LPTSTR _cmdLine, int _nCmdShow);
	virtual ~TInstApp();

	void InitWindow(void);
};