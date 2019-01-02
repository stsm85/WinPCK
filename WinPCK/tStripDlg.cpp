//////////////////////////////////////////////////////////////////////
// tStripDlg.cpp: WinPCK 界面线程部分
// 对话框代码
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2018.12.29
//////////////////////////////////////////////////////////////////////

#include "miscdlg.h"
#include "PckModelStripDefines.h"

BOOL TStripDlg::EvCreate(LPARAM lParam)
{
	Show();
	return	FALSE;
}

BOOL TStripDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	switch (wID) {
	case IDOK:
		OnOK();
	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}

void TStripDlg::OnOK()
{
	int flag = PCK_STRIP_NONE;

	flag |= (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_DDS) ? PCK_STRIP_DDS : PCK_STRIP_NONE);
	flag |= (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_ATT) ? PCK_STRIP_ATT : PCK_STRIP_NONE);
	flag |= (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_GFX) ? PCK_STRIP_GFX : PCK_STRIP_NONE);
	flag |= (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_ECM) ? PCK_STRIP_ECM : PCK_STRIP_NONE);

	*pStripFlag = flag;
}