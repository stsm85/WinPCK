/* @(#)Copyright (C) 1996-2010 H.Shirouzu		tlib.h	Ver0.99 */
/* ========================================================================
	Project  Name			: Win32 Lightweight  Class Library Test
	Module Name				: Main Header
	Create					: 1996-06-01(Sat)
	Update					: 2010-05-09(Mon)
	Copyright				: H.Shirouzu
	Reference				: 
	======================================================================== */
#include "tconf.h"

#ifndef TLIB_H
#define TLIB_H

#ifndef STRICT
#define STRICT
#endif

#ifdef _USE_T_CRYPT_
/* for crypto api */
#if _WIN32_WINNT < 0x0600
#define _WIN32_WINNT 0x0600
#endif
#endif

/* for new version VC */
#if _MSC_VER >= 1400
#pragma warning ( disable : 4996 )
#else
#define LONG_PTR LONG
#endif
#pragma warning ( disable : 4355 )
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4244 )
#pragma warning ( disable : 4312 )

#include <windows.h>
//#include <windowsx.h>
//#include <stdio.h>
//#include <stdlib.h>
#include <richedit.h>
#include "commctrl.h"
//#include <regstr.h>
//#include <shlobj.h>
//#include <tchar.h>
#include "tapi32ex.h"
//#include "tapi32v.h"
//#include "tapi32u8.h"	 /* describe last line */

extern DWORD TWinVersion;	// define in tmisc.cpp

#define WINEXEC_ERR_MAX		31
#define TLIB_SLEEPTIMER		32000

#define TLIB_CRYPT		0x00000001
#define TLIB_PROTECT	0x00000002
#define TLIB_WIN32V		0x00000004

#define IsNewShell()	(LOBYTE(LOWORD(TWinVersion)) >= 4)

#define IsWin31()		(LOBYTE(LOWORD(TWinVersion)) == 3 && HIBYTE(LOWORD(TWinVersion)) < 20)
#define IsWin95()		(LOBYTE(LOWORD(TWinVersion)) >= 4 && TWinVersion >= 0x80000000)

#define IsWinNT350()	(LOBYTE(LOWORD(TWinVersion)) == 3 && TWinVersion < 0x80000000 \
							&& HIBYTE(LOWORD(TWinVersion)) == 50)
#define IsWinNT()		(LOBYTE(LOWORD(TWinVersion)) >= 4 && TWinVersion < 0x80000000)
#define IsWin2K()		(LOBYTE(LOWORD(TWinVersion)) >= 5 && TWinVersion < 0x80000000)
#define IsWinXP()		((LOBYTE(LOWORD(TWinVersion)) >= 6 || LOBYTE(LOWORD(TWinVersion)) == 5 \
							&& HIBYTE(LOWORD(TWinVersion)) >= 10) && TWinVersion < 0x80000000)
#define IsWinVista()	(LOBYTE(LOWORD(TWinVersion)) >= 6 && TWinVersion < 0x80000000)

#define IsLang(lang)	(PRIMARYLANGID(LANGIDFROMLCID(GetThreadLocale())) == lang)

#define ALIGN_SIZE(all_size, block_size) (((all_size) + (block_size) -1) \
										 / (block_size) * (block_size))

#define ALIGN_BLOCK(size, align_size) (((size) + (align_size) -1) / (align_size))

#define BUTTON_CLASS		"BUTTON"
#define COMBOBOX_CLASS		"COMBOBOX"
#define EDIT_CLASS			"EDIT"
#define LISTBOX_CLASS		"LISTBOX"
#define MDICLIENT_CLASS		"MDICLIENT"
#define SCROLLBAR_CLASS		"SCROLLBAR"
#define STATIC_CLASS		"STATIC"

#define MAX_WPATH		32000
#define MAX_PATH_EX		(MAX_PATH * 8)
#define MAX_PATH_U8		(MAX_PATH * 3)
#define MAX_FNAME_LEN	255

#define BITS_OF_BYTE	8
#define BYTE_NUM		256

#ifndef EM_SETTARGETDEVICE
#define COLOR_BTNFACE           15
#define COLOR_3DFACE            COLOR_BTNFACE
#define EM_SETBKGNDCOLOR		(WM_USER + 67)
#define EM_SETTARGETDEVICE		(WM_USER + 72)
#endif

#ifndef WM_COPYGLOBALDATA
#define WM_COPYGLOBALDATA	0x0049
#endif

#ifndef CSIDL_PROGRAM_FILES
#define CSIDL_LOCAL_APPDATA		0x001c
#define CSIDL_WINDOWS			0x0024
#define CSIDL_PROGRAM_FILES		0x0026
#endif
#ifndef CSIDL_PROGRAM_FILESX86
#define CSIDL_PROGRAM_FILESX86	0x002a 
#endif

#ifndef CSIDL_APPDATA
#define CSIDL_APPDATA  0x1a
#endif

#ifndef CSIDL_COMMON_APPDATA
#define CSIDL_COMMON_APPDATA  0x23
#endif

#ifndef SHCNF_PATHW
#define SHCNF_PATHW 0x0005
#endif

#ifndef BCM_FIRST
#define BCM_FIRST     0x1600
#define IDI_SHIELD    1022
#endif

#ifndef BCM_SETSHIELD
#define BCM_SETSHIELD (BCM_FIRST + 0x000C)
#endif

#ifndef PROCESS_MODE_BACKGROUND_BEGIN
#define PROCESS_MODE_BACKGROUND_BEGIN 0x00100000
#define PROCESS_MODE_BACKGROUND_END   0x00200000
#endif


#ifdef _WIN64
#define SetClassLongA	SetClassLongPtrA
#define SetClassLongW	SetClassLongPtrW
#define GetClassLongA	GetClassLongPtrA
#define GetClassLongW	GetClassLongPtrW
#define SetWindowLongA	SetWindowLongPtrA
#define SetWindowLongW	SetWindowLongPtrW
#define GetWindowLongA	GetWindowLongPtrA
#define GetWindowLongW	GetWindowLongPtrW
#define GCL_HICON		GCLP_HICON
#define GWL_WNDPROC		GWLP_WNDPROC
#define DWL_MSGRESULT	DWLP_MSGRESULT
#else
#define DWORD_PTR		DWORD
#endif

struct WINPOS {
	int		x;
	int		y;
	int		cx;
	int		cy;
};

enum DlgItemFlags {
	NONE_FIT	= 0x000,
	LEFT_FIT	= 0x001,
	RIGHT_FIT	= 0x002,
	TOP_FIT		= 0x004,
	BOTTOM_FIT	= 0x008,
	FIT_SKIP	= 0x800,
	X_FIT		= LEFT_FIT|RIGHT_FIT,
	Y_FIT		= TOP_FIT|BOTTOM_FIT,
	XY_FIT		= X_FIT|Y_FIT,
};

struct DlgItem {
	DWORD	flags;	// DlgItemFlags
	HWND	hWnd;
	WINPOS	wpos;
};



class THashTbl;

class THashObj {
public:
	THashObj	*priorHash;
	THashObj	*nextHash;
	u_int		hashId;

public:
	THashObj() { priorHash = nextHash = NULL; hashId = 0; }
	virtual ~THashObj() { if (priorHash && priorHash != this) UnlinkHash(); }

	virtual BOOL LinkHash(THashObj *top);
	virtual BOOL UnlinkHash();
	friend THashTbl;
};

class THashTbl {
protected:
	THashObj	*hashTbl;
	int			hashNum;
	int			registerNum;
	BOOL		isDeleteObj;

	virtual BOOL	IsSameVal(THashObj *, const void *val) = 0;

public:
	THashTbl(int _hashNum=0, BOOL _isDeleteObj=TRUE);
	virtual ~THashTbl();
	virtual BOOL	Init(int _hashNum);
	virtual void	UnInit();
	virtual void	Register(THashObj *obj, u_int hash_id);
	virtual void	UnRegister(THashObj *obj);
	virtual THashObj *Search(const void *data, u_int hash_id);
	virtual int		GetRegisterNum() { return registerNum; }
//	virtual u_int	MakeHashId(const void *data) = 0;
};

/* for internal use start */
struct TResHashObj : THashObj {
	void	*val;
	TResHashObj(UINT _resId, void *_val) { hashId = _resId; val = _val; }
	~TResHashObj() { free(val); }
	
};

class TResHash : public THashTbl {
protected:
	virtual BOOL IsSameVal(THashObj *obj, const void *val) {
		return obj->hashId == *(u_int *)val;
	}

public:
	TResHash(int _hashNum) : THashTbl(_hashNum) {}
	TResHashObj	*Search(UINT resId) { return (TResHashObj *)THashTbl::Search(&resId, resId); }
	void		Register(TResHashObj *obj) { THashTbl::Register(obj, obj->hashId); }
};
/* for internal use end */

class TWin : public THashObj {
protected:
	RECT			rect;
	RECT			orgRect;
	HACCEL			hAccel;
	TWin			*parent;
	BOOL			sleepBusy;	// for TWin::Sleep() only

public:
	TWin(TWin *_parent = NULL);
	virtual	~TWin();

	HWND			hWnd;

	virtual void	Show(int mode = SW_SHOWDEFAULT);
	virtual BOOL	Create(LPCTSTR className=NULL, LPCTSTR title=TEXT(""),
						DWORD style=(WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN),
						DWORD exStyle=0, HMENU hMenu=NULL);
	virtual	void	Destroy(void);

	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvSysCommand(WPARAM uCmdType, POINTS pos);
	virtual BOOL	EvCreate(LPARAM lParam);
	virtual BOOL	EvClose(void);
	virtual BOOL	EvNcDestroy(void);
	virtual BOOL	EvQueryEndSession(BOOL nSession, BOOL nLogOut);
	virtual BOOL	EvEndSession(BOOL nSession, BOOL nLogOut);
	virtual BOOL	EvQueryOpen(void);
	virtual BOOL	EvPaint(void);
	virtual BOOL	EvNcPaint(HRGN hRgn);
	virtual BOOL	EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight);
	virtual BOOL	EvShowWindow(BOOL fShow, int fnStatus);
	virtual BOOL	EvGetMinMaxInfo(MINMAXINFO *info);
	virtual BOOL	EvTimer(WPARAM timerID, TIMERPROC proc);
	virtual BOOL	EvSetCursor(HWND cursorWnd, WORD nHitTest, WORD wMouseMsg);
	virtual BOOL	EvMouseMove(UINT fwKeys, POINTS pos);
	virtual BOOL	EvMouseWheel(UINT nFlags, short zDelta, POINTS pos);
	virtual BOOL	EvNcHitTest(POINTS pos, LRESULT *result);
	virtual BOOL	EvMeasureItem(UINT ctlID, MEASUREITEMSTRUCT *lpMis);
	virtual BOOL	EvDrawItem(UINT ctlID, DRAWITEMSTRUCT *lpDis);
	virtual BOOL	EvMenuSelect(UINT uItem, UINT fuFlag, HMENU hMenu);
	virtual BOOL	EvDropFiles(HDROP hDrop);
	virtual BOOL	EvNotify(UINT ctlID, NMHDR *pNmHdr);
	virtual BOOL	EvContextMenu(HWND childWnd, POINTS pos);
	virtual BOOL	EvHotKey(int hotKey);

	virtual BOOL	EventActivateApp(BOOL fActivate, DWORD dwThreadID);
	virtual BOOL	EventActivate(BOOL fActivate, DWORD fMinimized, HWND hActiveWnd);
	virtual BOOL	EventScroll(UINT uMsg, int nCode, int nPos, HWND scrollBar);

	virtual BOOL	EventButton(UINT uMsg, int nHitTest, POINTS pos);
	virtual BOOL	EventKey(UINT uMsg, int nVirtKey, LONG lKeyData);
	virtual BOOL	EventInitMenu(UINT uMsg, HMENU hMenu, UINT uPos, BOOL fSystemMenu);
	virtual BOOL	EventCtlColor(UINT uMsg, HDC hDcCtl, HWND hWndCtl, HBRUSH *result);
	virtual BOOL	EventFocus(UINT uMsg, HWND focusWnd);
	virtual BOOL	EventSystem(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual BOOL	EventUser(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual UINT	GetDlgItemTextA(int ctlId, LPSTR buf, int len);
	virtual UINT	GetDlgItemTextW(int ctlId, LPWSTR buf, int len);
#ifdef UNICODE
#define GetDlgItemText  GetDlgItemTextW
#else
#define GetDlgItemText  GetDlgItemTextA
#endif // !UNICODE
	virtual BOOL	SetDlgItemTextA(int ctlId, LPCSTR buf);
	virtual BOOL	SetDlgItemTextW(int ctlId, LPCWSTR buf);
#ifdef UNICODE
#define SetDlgItemText  SetDlgItemTextW
#else
#define SetDlgItemText  SetDlgItemTextA
#endif // !UNICODE
	virtual int		GetDlgItemInt(int ctlId, BOOL *err=NULL, BOOL sign=TRUE);
	virtual BOOL	SetDlgItemInt(int ctlId, int val, BOOL sign=TRUE);
	virtual HWND	GetDlgItem(int ctlId);
	virtual BOOL	CheckDlgButton(int ctlId, UINT check);
	virtual UINT	IsDlgButtonChecked(int ctlId);
	virtual BOOL	IsWindowVisible(void);
	virtual BOOL	EnableWindow(BOOL is_enable);

	virtual	int		MessageBoxA(LPCSTR msg, LPCSTR title="msg", UINT style=MB_OK);
	virtual	int		MessageBoxW(LPCWSTR msg, LPCWSTR title=L"msg", UINT style=MB_OK);
#ifdef UNICODE
#define MessageBox  MessageBoxW
#else
#define MessageBox  MessageBoxA
#endif // !UNICODE
	virtual BOOL	BringWindowToTop(void);
	virtual BOOL	SetForegroundWindow(void);
	virtual BOOL	SetForceForegroundWindow(void);
	virtual BOOL	ShowWindow(int mode);
	virtual BOOL	PostMessageA(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual BOOL	PostMessageW(UINT uMsg, WPARAM wParam, LPARAM lParam);
#ifdef UNICODE
#define PostMessage  PostMessageW
#else
#define PostMessage  PostMessageA
#endif // !UNICODE
	virtual LRESULT	SendMessageA(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	SendMessageW(UINT uMsg, WPARAM wParam, LPARAM lParam);
#ifdef UNICODE
#define SendMessage  SendMessageW
#else
#define SendMessage  SendMessageA
#endif // !UNICODE
	virtual LRESULT	SendDlgItemMessageA(int ctlId, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	SendDlgItemMessageW(int ctlId, UINT uMsg, WPARAM wParam, LPARAM lParam);
#ifdef UNICODE
#define SendDlgItemMessage  SendDlgItemMessageW
#else
#define SendDlgItemMessage  SendDlgItemMessageA
#endif // !UNICODE
	virtual BOOL	GetWindowRect(RECT *_rect=NULL);
	virtual BOOL	SetWindowPos(HWND hInsAfter, int x, int y, int cx, int cy, UINT fuFlags);
	virtual HWND	SetActiveWindow(void);
	virtual int		GetWindowTextA(LPSTR text, int size);
	virtual int		GetWindowTextW(LPWSTR text, int size);
#ifdef UNICODE
#define GetWindowText  GetWindowTextW
#else
#define GetWindowText  GetWindowTextA
#endif // !UNICODE
	virtual BOOL	SetWindowTextA(LPCSTR text);
	virtual BOOL	SetWindowTextW(LPCWSTR text);
#ifdef UNICODE
#define SetWindowText  SetWindowTextW
#else
#define SetWindowText  SetWindowTextA
#endif // !UNICODE

	virtual int		GetWindowTextLengthA(void);
	virtual int		GetWindowTextLengthW(void);
#ifdef UNICODE
#define GetWindowTextLength  GetWindowTextLengthW
#else
#define GetWindowTextLength  GetWindowTextLengthA
#endif // !UNICODE

	virtual LONG_PTR SetWindowLong(int index, LONG_PTR val);
	virtual WORD	SetWindowWord(int index, WORD val);
	virtual LONG_PTR GetWindowLong(int index);
	virtual WORD	GetWindowWord(int index);
	virtual TWin	*GetParent(void) { return parent; };
	virtual void	SetParent(TWin *_parent) { parent = _parent; };
	virtual BOOL	MoveWindow(int x, int y, int cx, int cy, int bRepaint);
	virtual BOOL	Sleep(UINT mSec);
	virtual	BOOL	SetTimer(UINT_PTR nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc);
	virtual	BOOL	KillTimer(UINT_PTR nIDEvent);
	virtual	HWND	SetCapture();
	virtual	HWND	SetFocus();
	virtual BOOL	Idle(void);
	virtual RECT	*Rect() { return &rect; }

	virtual	BOOL	PreProcMsg(MSG *msg);
	virtual	LRESULT	WinProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual	LRESULT	DefWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

class TDlg : public TWin {
protected:
	UINT	resId;
	BOOL	modalFlg;
	int		maxItems;
	DlgItem	*dlgItems;

	//BOOL	IsInitRichEdit2;
	HMODULE	hRichEditLib;

public:
	TDlg(UINT	resid=0, TWin *_parent = NULL);
	virtual ~TDlg();

	virtual BOOL	Create(HINSTANCE hI = NULL);
	virtual	void	Destroy(void);
	virtual int		Exec(void);
	virtual void	EndDialog(int);
	UINT			ResId(void) { return resId; }
	virtual int		SetDlgItem(UINT ctl_id, DWORD flags=0);
	virtual BOOL	FitDlgItems();

	virtual BOOL	EvCreate(LPARAM lParam);
	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvSysCommand(WPARAM uCmdType, POINTS pos);
	virtual BOOL	EvQueryOpen(void);

	virtual	BOOL	PreProcMsg(MSG *msg);
	virtual LRESULT	WinProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual BOOL	InitRichEdit2();
};

class TSubClass : public TWin {
protected:
	WNDPROC		oldProc;

public:
	TSubClass(TWin *_parent = NULL);
	virtual ~TSubClass();

	virtual BOOL	AttachWnd(HWND _hWnd);
	virtual BOOL	DetachWnd();
	virtual	LRESULT	DefWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

class TSubClassCtl : public TSubClass {
protected:
public:
	TSubClassCtl(TWin *_parent);

	virtual	BOOL	PreProcMsg(MSG *msg);
};

BOOL TRegisterClassA(LPCSTR class_name, UINT style=CS_DBLCLKS, HICON hIcon=0, HCURSOR hCursor=0,
		HBRUSH hbrBackground=0, int classExtra=0, int wndExtra=0, LPCSTR menu_str=0);
BOOL TRegisterClassW(LPCWSTR class_name, UINT style=CS_DBLCLKS, HICON hIcon=0, HCURSOR hCursor=0,
		HBRUSH hbrBackground=0, int classExtra=0, int wndExtra=0, LPCWSTR menu_str=0);
#ifdef UNICODE
#define TRegisterClass  TRegisterClassW
#else
#define TRegisterClass  TRegisterClassA
#endif // !UNICODE


class TWinHashTbl : public THashTbl {
protected:
	virtual BOOL	IsSameVal(THashObj *obj, const void *val) {
		return ((TWin *)obj)->hWnd == *(HWND *)val;
	}

public:
	TWinHashTbl(int _hashNum) : THashTbl(_hashNum) {}
	~TWinHashTbl() {}
#pragma warning(push)
#pragma warning(disable: 4302)
	u_int	MakeHashId(HWND hWnd) { return (u_int)hWnd * 0xf3f77d13; }
#pragma warning(pop)
};

class TApp {
protected:
	static TApp	*tapp;
	TWinHashTbl	*hash;
	LPCTSTR		defaultClass;

	LPTSTR		cmdLine;
	int			nCmdShow;
	TWin		*mainWnd;
	TWin		*preWnd;
	HINSTANCE	hI;

	TWin	*SearchWnd(HWND hWnd) { return (TWin *)hash->Search(&hWnd, hash->MakeHashId(hWnd)); }
	virtual BOOL	InitApp(void);

public:
	TApp(HINSTANCE _hI, LPTSTR _cmdLine, int _nCmdShow);
	virtual ~TApp();
	virtual void	InitWindow() = 0;
	virtual int		Run();
	virtual BOOL	PreProcMsg(MSG *msg);

	LPCTSTR	GetDefaultClass() { return defaultClass; }
	void	AddWin(TWin *win) { preWnd = win; }
	void	AddWinByWnd(TWin *win, HWND hWnd) {
			win->hWnd = hWnd; hash->Register(win, hash->MakeHashId(hWnd));
	}
	void	DelWin(TWin *win) { hash->UnRegister(win); }

	static TApp *GetApp() { return tapp; }
	static HINSTANCE GetInstance() { return tapp->hI; }
	static LRESULT CALLBACK WinProc(HWND hW, UINT uMsg, WPARAM wParam, LPARAM lParam);
};


void InitInstanceForLoadStr(HINSTANCE hI);
LPSTR GetLoadStrA(UINT resId, HINSTANCE hI=NULL);
LPWSTR GetLoadStrW(UINT resId, HINSTANCE hI=NULL);
#ifdef UNICODE
#define GetLoadStr GetLoadStrW
#else
#define GetLoadStr GetLoadStrA
#endif


void TSetDefaultLCID(LCID id=0);

HMODULE TLoadLibraryA(LPSTR dllname);
HMODULE TLoadLibraryW(LPWSTR dllname);
#ifdef UNICODE
#define TLoadLibrary  TLoadLibraryW
#else
#define TLoadLibrary  TLoadLibraryA
#endif // !UNICODE

//BOOL TSetPrivilege(LPTSTR pszPrivilege, BOOL bEnable);

_int64 hex2ll(char *buf);
int bin2hexstr(const BYTE *bindata, int len, char *buf);
int bin2hexstrW(const BYTE *bindata, int len, WCHAR *buf);
int bin2hexstr_bigendian(const BYTE *bin, int len, char *buf);
BOOL hexstr2bin(const char *buf, BYTE *bindata, int maxlen, int *len);
BOOL hexstr2bin_bigendian(const char *buf, BYTE *bindata, int maxlen, int *len);

char *strdupNew(const char *_s);
WCHAR *wcsdupNew(const WCHAR *_s);

int strncmpi(const char *str1, const char *str2, int num);
char *strncpyz(char *dest, const char *src, int num);

BOOL TIsWow64();
BOOL TIsUserAnAdmin();
BOOL TSetThreadLocale(int lcid);
BOOL TChangeWindowMessageFilter(UINT msg, DWORD flg);

BOOL InstallExceptionFilter(char *title, char *info);
#ifdef _DEBUG
void DebugA(char *fmt,...);
void DebugW(WCHAR *fmt,...);
void DebugU8(char *fmt,...);
#ifdef UNICODE
#define Debug  DebugW
#else
#define Debug  DebugA
#endif // !UNICODE
#else
#include "tDebug.h"
#endif
#include "tapi32u8.h"

#endif
