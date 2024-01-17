#pragma once

class TCompressOptDlg : public TDlg
{
public:
	TCompressOptDlg(TWin* _win) : TDlg(IDD_DIALOG_COMPRESS, _win) { }

	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl) override;
	virtual BOOL	EvCreate(LPARAM lParam) override;
	virtual BOOL	EventScroll(UINT uMsg, int nCode, int nPos, HWND scrollBar) override;
};
