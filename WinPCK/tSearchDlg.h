#pragma once

class TSearchDlg : public TDlg
{
protected:
	wchar_t* dirBuf;

public:
	TSearchDlg(wchar_t* _dirBuf, TWin* _win) : TDlg(IDD_DIALOG_SEARCH, _win) { dirBuf = _dirBuf; }
	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl) override;
	virtual BOOL	EvCreate(LPARAM lParam) override;
};
