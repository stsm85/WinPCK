//////////////////////////////////////////////////////////////////////
// tInfoDlg.cpp: WinPCK 界面线程部分
// 对话框代码
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2017.12.26
//////////////////////////////////////////////////////////////////////

#include "globals.h"
#include "miscdlg.h"
#include "PckHeader.h"

/*
附加信息对话框
*/
BOOL TInfoDlg::EvCreate(LPARAM lParam)
{
	SendDlgItemMessage(IDC_EDIT_INFO, EM_LIMITTEXT, PCK_ADDITIONAL_INFO_SIZE - 1, 0);
	SetDlgItemTextA(IDC_EDIT_INFO, dirBuf);
	return	TRUE;
}

BOOL TInfoDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	switch(wID) {
	case IDOK:
		memset(dirBuf, 0, PCK_ADDITIONAL_INFO_SIZE);
		GetDlgItemTextA(IDC_EDIT_INFO, dirBuf, PCK_ADDITIONAL_INFO_SIZE);
		EndDialog(wID);
		return	TRUE;

	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}

