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

#include "guipch.h"
#include "tCompressOptDlg.h"

/*
压缩选项
*/
BOOL TCompressOptDlg::EvCreate(LPARAM lParam)
{
	this->SendDlgItemMessage(IDC_EDIT_MEM, EM_LIMITTEXT, 4, 0);

	this->SendDlgItemMessage(IDC_SLIDER_LEVEL, TBM_SETRANGE, FALSE, MAKELONG(1, pck_getMaxCompressLevel()));
	this->SendDlgItemMessage(IDC_SLIDER_THREAD, TBM_SETRANGE, FALSE, MAKELONG(1, pck_getMaxThreadUpperLimit()));
	this->SendDlgItemMessage(IDC_SLIDER_LEVEL, TBM_SETPOS, TRUE, (LPARAM)pck_getCompressLevel());
	this->SendDlgItemMessage(IDC_SLIDER_THREAD, TBM_SETPOS, TRUE, (LPARAM)pck_getMaxThread());

	this->SendDlgItemMessageA(IDC_CBO_CODEPAGE, CB_ADDSTRING, 0, (LPARAM)"CP936");
	this->SendDlgItemMessageA(IDC_CBO_CODEPAGE, CB_SETCURSEL, 0, 0);

	this->SetDlgItemTextA(IDC_STATIC_LEVEL, std::to_string(pck_getCompressLevel()).c_str());
	this->SetDlgItemTextA(IDC_STATIC_THREAD, std::to_string(pck_getMaxThread()).c_str());
	this->SetDlgItemTextA(IDC_EDIT_MEM, std::to_string((pck_getMTMaxMemory()) >> 20).c_str());

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

		this->GetDlgItemTextA(IDC_EDIT_MEM, szStr, 8);
		pck_setMTMaxMemory((std::stoul(szStr) << 20));

		this->EndDialog(wID);
		return	TRUE;
	}
	case IDCANCEL:
		this->EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}

BOOL TCompressOptDlg::EventScroll(UINT uMsg, int nCode, int nPos, HWND scrollBar)
{
	switch(nCode) {
	case TB_THUMBTRACK:
	case TB_PAGEDOWN:
	case TB_PAGEUP:

		int iPos = ::SendMessage(scrollBar, TBM_GETPOS, 0, 0);

		if(scrollBar == this->GetDlgItem(IDC_SLIDER_LEVEL)) {
			this->SetDlgItemTextA(IDC_STATIC_LEVEL, std::to_string(iPos).c_str());
			break;
		}
		if(scrollBar == this->GetDlgItem(IDC_SLIDER_THREAD)) {
			this->SetDlgItemTextA(IDC_STATIC_THREAD, std::to_string(iPos).c_str());
			break;
		}

		break;
	}
	return 0;
}
