#pragma once

class TInfoDlg : public TDlg
{
public:
	TInfoDlg(TWin* _win) : TDlg(IDD_DIALOG_INFO, _win) {}
	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl) override;
	virtual BOOL	EvCreate(LPARAM lParam) override;

private:
	void set_additional_info();
};