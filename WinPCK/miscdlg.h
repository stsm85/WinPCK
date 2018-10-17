
#ifndef _MISCDLG_H_
#define _MISCDLG_H_

#include "tlib.h"
#include "resource.h"
#include <tchar.h>
#include <stdio.h>
#include "PckHeader.h"

class TLogDlg : public TDlg
{
protected:

	//LPPCK_RUNTIME_PARAMS	lpParams;
	HWND	hWndList;
	wchar_t	szExePath[MAX_PATH];

	wchar_t*	pszLogFileName();
	wchar_t*	pszTargetListLog(int iItem);

	int		m_iCurrentHotItem;


public:
	TLogDlg(TWin *_win) : TDlg(IDD_DIALOG_LOG, _win) { }
	
	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
	//virtual BOOL	EventScroll(UINT uMsg, int nCode, int nPos, HWND scrollBar);
	virtual BOOL	EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight);
	virtual BOOL	EvClose();
	virtual BOOL	EvNotify(UINT ctlID, NMHDR *pNmHdr);

	HWND	GetListWnd();
};

class TInfoDlg : public TDlg
{
protected:
	char	*dirBuf;

public:
	TInfoDlg(char *_dirBuf, TWin *_win) : TDlg(IDD_DIALOG_INFO, _win) { dirBuf = _dirBuf; }
	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
};

class TSearchDlg : public TDlg
{
protected:
	char	*dirBuf;

public:
	TSearchDlg(char *_dirBuf, TWin *_win) : TDlg(IDD_DIALOG_SEARCH, _win) { dirBuf = _dirBuf; }
	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
};

class TCompressOptDlg : public TDlg
{
protected:

	LPPCK_RUNTIME_PARAMS	lpParams;

public:
	TCompressOptDlg(LPPCK_RUNTIME_PARAMS in, TWin *_win) : TDlg(IDD_DIALOG_COMPRESS, _win) { lpParams = in; }
	
	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
	virtual BOOL	EventScroll(UINT uMsg, int nCode, int nPos, HWND scrollBar);
};

class TAttrDlg : public TDlg
{
protected:
	void	*lpPckInfo;
	void	*lpRootInfo;

	wchar_t	*lpszPath;
	BOOL	isSearchMode;

	QWORD	qwPckFileSize;

public:
	TAttrDlg(void *_lpPckInfo, void *_lpRootInfo, QWORD _qwPckFileSize, wchar_t *_lpszPath, BOOL _isSearchMode, TWin *_win);
	//~TAttrDlg();

	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
};


class TRebuildOptDlg : public TDlg
{
protected:

	LPPCK_RUNTIME_PARAMS	lpParams;
	TCHAR					szScriptFile[MAX_PATH];
	BOOL					*lpNeedRecompress;

public:
	TRebuildOptDlg(LPPCK_RUNTIME_PARAMS in, BOOL *_lpNeedRecompress, TWin *_win) : \
		TDlg(IDD_DIALOG_REBUILD_OPT, _win), \
		lpParams(in), \
		lpNeedRecompress(_lpNeedRecompress), \
		isScriptParseSuccess(FALSE)
	{
		*szScriptFile = 0;
	}


	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
	virtual BOOL	EventScroll(UINT uMsg, int nCode, int nPos, HWND scrollBar);
	virtual BOOL	EvDropFiles(HDROP hDrop);

protected:

	void OnOK();
	BOOL OnOpenClick();
	BOOL isScriptParseSuccess;
	BOOL ParseScript();
};

#endif
