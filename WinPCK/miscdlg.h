
#ifndef _MISCDLG_H_
#define _MISCDLG_H_

#include "tlib.h"
#include "resource.h"
#include <tchar.h>
#include <stdio.h>
#include "pck_handle.h"

class TInfoDlg : public TDlg
{
public:
	TInfoDlg(TWin *_win) : TDlg(IDD_DIALOG_INFO, _win) {}
	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
};

class TSearchDlg : public TDlg
{
protected:
	wchar_t	*dirBuf;

public:
	TSearchDlg(wchar_t *_dirBuf, TWin *_win) : TDlg(IDD_DIALOG_SEARCH, _win) { dirBuf = _dirBuf; }
	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
};

class TCompressOptDlg : public TDlg
{
public:
	TCompressOptDlg(TWin *_win) : TDlg(IDD_DIALOG_COMPRESS, _win) { }
	
	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
	virtual BOOL	EventScroll(UINT uMsg, int nCode, int nPos, HWND scrollBar);
};

class TAttrDlg : public TDlg
{
protected:
	const PCK_UNIFIED_FILE_ENTRY*	lpPckInfo;

	wchar_t	*lpszPath;

public:
	TAttrDlg(const PCK_UNIFIED_FILE_ENTRY *_lpPckInfo, wchar_t *_lpszPath, TWin *_win);
	//~TAttrDlg();

	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
};


class TRebuildOptDlg : public TDlg
{
private:

	TCHAR					szScriptFile[MAX_PATH];
	BOOL					*lpNeedRecompress;
	TCHAR*					lpszScriptFile;
public:
	TRebuildOptDlg(TCHAR* _lpszScriptFile, BOOL *_lpNeedRecompress, TWin *_win) :
		TDlg(IDD_DIALOG_REBUILD_OPT, _win), 
		lpNeedRecompress(_lpNeedRecompress), 
		isScriptParseSuccess(FALSE),
		lpszScriptFile(_lpszScriptFile)
	{
		*szScriptFile = 0;
	}


	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
	virtual BOOL	EventScroll(UINT uMsg, int nCode, int nPos, HWND scrollBar);
	virtual BOOL	EvDropFiles(HDROP hDrop);

private:

	void OnOK();
	BOOL OnOpenClick();
	BOOL isScriptParseSuccess;
	BOOL ParseScript();
};


class TStripDlg : public TDlg
{
private:

	int	*pStripFlag;

public:
	TStripDlg(int *_pStripFlag, TWin *_win) :
		TDlg(IDD_DIALOG_STRIP, _win),
		pStripFlag(_pStripFlag)
	{}

	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);

private:
	void OnOK();
};
#endif
