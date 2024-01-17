#pragma once

class TRebuildOptDlg : public TDlg
{
private:

	std::wstring			szScriptFile = std::wstring(MAX_PATH, '\x0');
	BOOL* lpNeedRecompress;
	TCHAR* lpszScriptFile;
public:
	TRebuildOptDlg(TCHAR* _lpszScriptFile, BOOL* _lpNeedRecompress, TWin* _win) :
		TDlg(IDD_DIALOG_REBUILD_OPT, _win),
		lpNeedRecompress(_lpNeedRecompress),
		isScriptParseSuccess(FALSE),
		lpszScriptFile(_lpszScriptFile)
	{
	}


	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl) override;
	virtual BOOL	EvCreate(LPARAM lParam) override;
	virtual BOOL	EventScroll(UINT uMsg, int nCode, int nPos, HWND scrollBar) override;
	virtual BOOL	EvDropFiles(HDROP hDrop) override;

private:

	void OnOK();
	BOOL OnOpenClick();
	BOOL isScriptParseSuccess;
	BOOL ParseScript();
};
