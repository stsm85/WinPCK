#pragma once

//#include "resource.h"

class TStripDlg : public TDlg
{
private:

	int	*pStripFlag;

public:
	TStripDlg(int *_pStripFlag, TWin *_win) :
		TDlg(IDD_DIALOG_STRIP, _win),
		pStripFlag(_pStripFlag)
	{}

	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl) override;
	virtual BOOL	EvCreate(LPARAM lParam) override;

private:
	void OnOK();
};

