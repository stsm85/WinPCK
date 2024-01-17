#pragma once

class TAttrDlg : public TDlg
{
protected:
	const PCK_UNIFIED_FILE_ENTRY* lpPckInfo;

	wchar_t* lpszPath;

public:
	TAttrDlg(const PCK_UNIFIED_FILE_ENTRY* _lpPckInfo, wchar_t* _lpszPath, TWin* _win);
	//~TAttrDlg();

	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl) override;
	virtual BOOL	EvCreate(LPARAM lParam) override;
};

