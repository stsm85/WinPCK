#pragma once

#include "WinControl.h"
class CComboBox :
	public CWinControl
{
public:
	CComboBox() = default;
	virtual ~CComboBox() = default;

	LRESULT AddString(LPCSTR s) { return SendMsgA(CB_ADDSTRING, 0, (LPARAM)s); }
	LRESULT InsertString(int nIndex, LPCSTR s) { return SendMsgA(CB_ADDSTRING, nIndex, (LPARAM)s); }
	int DeleteString(UINT nIndex) { return SendMsgA(CB_DELETESTRING, nIndex, NULL); }

	void clear() { SendMsgA(CB_RESETCONTENT, NULL, NULL); };

	LRESULT SetCurSel(int index) { return SendMsgA(CB_SETCURSEL, index, 0); }
	int GetCurSel() { return (int)SendMsgA(CB_GETCURSEL, 0, 0); }
	int GetCount() { return (int)SendMsgA(CB_GETCOUNT, 0, 0); }

	LRESULT GetLBTextLen(int index) { return SendMsgA(CB_GETLBTEXTLEN, index, 0); }
	LRESULT GetLBText(int index, LPSTR s) { return SendMsgA(CB_GETLBTEXT, index, (LPARAM)s); }
	std::string GetLBText(int index);
	std::string GetCurLBText() { return GetLBText((int)SendMsgA(CB_GETCURSEL, 0, 0)); }

	// selection helpers
	int FindString(int nStartAfter, LPCSTR lpszString);
	int SelectString(int nStartAfter, LPCSTR lpszString);

};
