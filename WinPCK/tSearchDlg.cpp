//////////////////////////////////////////////////////////////////////
// tSearchDlg.cpp: WinPCK 界面线程部分
// 对话框代码
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2017.12.26
//////////////////////////////////////////////////////////////////////

#include "miscdlg.h"

/*
查找对话框
*/
BOOL TSearchDlg::EvCreate(LPARAM lParam)
{
	SendDlgItemMessage(IDC_EDIT_SEARCH, EM_LIMITTEXT, 255, 0);
	SetDlgItemTextW(IDC_EDIT_SEARCH, dirBuf);
	//SendDlgItemMessage(IDC_EDIT_SEARCH, EM_SETSEL, 0, -1);
	//::SetFocus(GetDlgItem(IDC_EDIT_SEARCH));
	return	TRUE;
}

BOOL TSearchDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	switch(wID) {
	case IDOK:
		GetDlgItemTextW(IDC_EDIT_SEARCH, dirBuf, 256);
		EndDialog(wID);
		return	TRUE;

	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}
