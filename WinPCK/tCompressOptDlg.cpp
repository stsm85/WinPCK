//////////////////////////////////////////////////////////////////////
// tCompressOptDlg.cpp: WinPCK 界面线程部分
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
#include "pck_handle.h"

/*
压缩选项
*/
BOOL TCompressOptDlg::EvCreate(LPARAM lParam)
{
	char	szStr[8];

	SendDlgItemMessage(IDC_EDIT_MEM, EM_LIMITTEXT, 4, 0);

	SendDlgItemMessage(IDC_SLIDER_LEVEL, TBM_SETRANGE, FALSE, MAKELONG(1, pck_getMaxCompressLevel()));
	SendDlgItemMessage(IDC_SLIDER_THREAD, TBM_SETRANGE, FALSE, MAKELONG(1, pck_getMaxThreadUpperLimit()));
	SendDlgItemMessage(IDC_SLIDER_LEVEL, TBM_SETPOS, TRUE, (LPARAM)pck_getCompressLevel());
	SendDlgItemMessage(IDC_SLIDER_THREAD, TBM_SETPOS, TRUE, (LPARAM)pck_getMaxThread());

	SendDlgItemMessageA(IDC_CBO_CODEPAGE, CB_ADDSTRING, 0, (LPARAM)"CP936");
	SendDlgItemMessageA(IDC_CBO_CODEPAGE, CB_SETCURSEL, 0, 0);

	SetDlgItemTextA(IDC_STATIC_LEVEL, ultoa(pck_getCompressLevel(), szStr, 10));
	SetDlgItemTextA(IDC_STATIC_THREAD, ultoa(pck_getMaxThread(), szStr, 10));
	SetDlgItemTextA(IDC_EDIT_MEM, ultoa((pck_getMTMaxMemory()) >> 20, szStr, 10));

	return	TRUE;
}

BOOL TCompressOptDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	char	szStr[8];

	switch(wID) {
	case IDOK:
	{
		DWORD dwCompressLevel = pck_getCompressLevel();
		pck_setCompressLevel(SendDlgItemMessage(IDC_SLIDER_LEVEL, TBM_GETPOS, 0, 0));

		pck_setMaxThread(SendDlgItemMessage(IDC_SLIDER_THREAD, TBM_GETPOS, 0, 0));

		GetDlgItemTextA(IDC_EDIT_MEM, szStr, 8);
		pck_setMTMaxMemory((atoi(szStr) << 20));

		EndDialog(wID);
		return	TRUE;
	}
	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}

BOOL TCompressOptDlg::EventScroll(UINT uMsg, int nCode, int nPos, HWND scrollBar)
{
	int		iPos;
	char	szStr[4];

	switch(nCode) {
	case TB_THUMBTRACK:
	case TB_PAGEDOWN:
	case TB_PAGEUP:

		iPos = ::SendMessage(scrollBar, TBM_GETPOS, 0, 0);

		if(scrollBar == GetDlgItem(IDC_SLIDER_LEVEL)) {
			SetDlgItemTextA(IDC_STATIC_LEVEL, ultoa(iPos, szStr, 10));
			break;
		}
		if(scrollBar == GetDlgItem(IDC_SLIDER_THREAD)) {
			SetDlgItemTextA(IDC_STATIC_THREAD, ultoa(iPos, szStr, 10));
			break;
		}

		break;
	}
	return 0;
}
