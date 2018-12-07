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
	SendDlgItemMessage(IDC_EDIT_INFO, EM_LIMITTEXT, pck_GetAdditionalInfoMaxSize() - 1, 0);
	SetDlgItemTextA(IDC_EDIT_INFO, pck_GetAdditionalInfo());
	return	TRUE;
}

BOOL TInfoDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	char *szAdditionalInfo;
	switch(wID) {
	case IDOK:
		szAdditionalInfo = new char[pck_GetAdditionalInfoMaxSize()];
		memset(szAdditionalInfo, 0, pck_GetAdditionalInfoMaxSize());
		GetDlgItemTextA(IDC_EDIT_INFO, szAdditionalInfo, pck_GetAdditionalInfoMaxSize());

		//返回1为操作成功，success=WINPCK_OK
		pck_SetAdditionalInfo(szAdditionalInfo);
		EndDialog(wID);
		return	TRUE;

	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}

