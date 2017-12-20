
#include "PckControlCenter.h"
#include "miscdlg.h"



class TInstDlg : public TDlg
{
protected:
	//TSubClassCtl	staticText;
	//TInstSheet		*propertySheet;
	//InstallCfg		cfg;
	TLogDlg			*logdlg;

public:
	TInstDlg(LPSTR cmdLine);
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
	virtual BOOL	EventUser(UINT uMsg, WPARAM wParam, LPARAM lParam);

	//virtual BOOL	EventKey(UINT uMsg, int nVirtKey, LONG lKeyData);

	//virtual BOOL	EvSysCommand(WPARAM uCmdType, POINTS pos);
	//virtual BOOL	
#if 0
	virtual BOOL	EvNcDestroy(void);
	virtual BOOL	EventUser(UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

	//BOOL	RunAsAdmin(BOOL is_imme);
	//void	ChangeMode(void);



//用户变量
protected:

	TCHAR	m_MyFileName[MAX_PATH];
	TCHAR	m_Filename[MAX_PATH], m_CurrentPath[MAX_PATH];
	

	CPckControlCenter	*m_lpPckCenter;
	LPPCK_PATH_NODE		m_currentNodeOnShow;	/*m_lpPckNode, */

	LPPCK_RUNTIME_PARAMS	lpPckParams;

	BOOL	bGoingToExit;
	char	m_szStrToSearch[256];

	///addmode
	TCHAR	(*m_lpszFilePath)[MAX_PATH];
	DWORD	m_DropFileCount;


	//用于找窗口的变量
	BOOL	m_isSearchWindow;

	HCURSOR	m_hCursorOld, m_hCursorAllow, m_hCursorNo;

	struct {
		char	szPaths[MAX_PATH];
		char*	lpszDirNames[127];
		int		nDirCount;
	}m_PathDirs;

	HIMAGELIST	m_imageList;
	_inline BOOL EnableButton(UINT buttonID, BOOL enable) { return (BOOL)(SendDlgItemMessage(IDC_TOOLBAR, TB_ENABLEBUTTON, buttonID, MAKELONG(enable, 0))); }

	//Timer String
	TCHAR		szTimerProcessingFormatString[64];
	TCHAR		szTimerProcessedFormatString[64];


//用户函数
protected:

	//winmain.cpp
	VOID SetStatusBarText(int iPart, LPCSTR lpszText );
	VOID SetStatusBarText(int iPart, LPCWSTR lpszText );
	void InsertList(CONST HWND hWndList, CONST INT iIndex, CONST UINT uiMask, CONST INT iImage, CONST LPVOID lParam, CONST INT nColCount, ...);
	BOOL InitListView(CONST HWND hWndListView, LPTSTR *lpszText, int *icx, int *ifmt);
	BOOL IsValidWndAndGetPath(TCHAR * szPath, BOOL isGetPath = FALSE);
	void RefreshProgress();


	//threadproc.cpp
	_inline void EnbaleButtons(int ctlExceptID, BOOL bEnbale);
	static VOID UpdatePckFile(VOID *pParam);
	static VOID RenamePckFile(VOID *pParam);
	static VOID RebuildPckFile(VOID	*pParam);
	static VOID CreateNewPckFile(VOID *pParam);
	static VOID ToExtractAllFiles(VOID *pParam);
	static VOID ToExtractSelectedFiles(VOID	*pParam);
	static VOID DeleteFileFromPckFile(VOID *pParam);


	//mainfunc.cpp
	LPPCK_PATH_NODE	GetLastShowNode();
	BOOL OpenPckFile(TCHAR *lpszFileToOpen = TEXT(""), BOOL isReOpen = FALSE);
	VOID SearchPckFiles();
	VOID ShowPckFiles(LPPCK_PATH_NODE lpNodeToShow);


	//guilated.cpp
	void		initCommctrls();
	void		initParams();
	void		initToolbar();
	void		initArgument();


	//helpfunc.cpp
	VOID ViewFileAttribute();
	VOID ViewFile();
	BOOL AddFiles();
	BOOL OpenSingleFile(TCHAR * lpszFileName);
	BOOL OpenFiles(/*TCHAR &((*lpszFilePathArray)[MAX_PATH])*/LPVOID &lpszFilePathArray, DWORD &dwFileCount);
	DWORD SaveFile(TCHAR * lpszFileName, LPCTSTR lpstrFilter, DWORD nFilterIndex = 1);
	BOOL BrowseForFolderByPath(TCHAR * );
	static int CALLBACK TInstDlg::BFFCallBack(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
	void AddSetupReg();
	void DeleteSetupReg();

	void InitLogWindow();


};

class TInstApp : public TApp
{
public:
	TInstApp(HINSTANCE _hI, LPSTR _cmdLine, int _nCmdShow);
	virtual ~TInstApp();

	void InitWindow(void);
};