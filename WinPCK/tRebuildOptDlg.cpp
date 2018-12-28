//////////////////////////////////////////////////////////////////////
// tRebuildOptDlg.cpp: WinPCK 界面线程部分
// 对话框代码
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2018.05.11
//////////////////////////////////////////////////////////////////////

#include "globals.h"
#include "miscdlg.h"
#include "OpenSaveDlg.h"


/*
压缩选项
*/
BOOL TRebuildOptDlg::EvCreate(LPARAM lParam)
{
	char	szStr[8];

	SendDlgItemMessage(IDC_EDIT_SCRIPT, EM_LIMITTEXT, MAX_PATH, 0);

	SendDlgItemMessage(IDC_SLIDER_LEVEL, TBM_SETRANGE, FALSE, MAKELONG(1, pck_getMaxCompressLevel()));
	SendDlgItemMessage(IDC_SLIDER_LEVEL, TBM_SETPOS, TRUE, (LPARAM)pck_getCompressLevel());

	SetDlgItemTextA(IDC_STATIC_LEVEL, ultoa(pck_getCompressLevel(), szStr, 10));
#ifdef _DEBUG
	SetDlgItemTextA(IDC_EDIT_SCRIPT, "F:\\!)MyProjects\\VC\\WinPCK\\testpck\\script\\test.txt");
#endif

	*lpszScriptFile = 0;

	return	TRUE;
}

BOOL TRebuildOptDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{

	switch(wID) {
	case IDOK:
	{
		OnOK();
		EndDialog(wID);
		return	TRUE;
	}
	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;


	case IDC_BUTTON_OPEN:
		OnOpenClick();
		break;
	}
	return	FALSE;
}

BOOL TRebuildOptDlg::OnOpenClick()
{
	if(OpenSingleFile(hWnd, szScriptFile)) {

		SetDlgItemText(IDC_EDIT_SCRIPT, szScriptFile);
		return ParseScript();
	}
	return FALSE;
}

void TRebuildOptDlg::OnOK()
{
	DWORD dwCompressLevel = pck_getCompressLevel();
	pck_setCompressLevel(SendDlgItemMessage(IDC_SLIDER_LEVEL, TBM_GETPOS, 0, 0));

	*lpNeedRecompress = IsDlgButtonChecked(IDC_CHECK_RECPMPRESS);

}

BOOL TRebuildOptDlg::EventScroll(UINT uMsg, int nCode, int nPos, HWND scrollBar)
{
	int		iPos;
	char	szStr[4];

	switch(nCode) {
	case TB_THUMBTRACK:
	case TB_PAGEDOWN:
	case TB_PAGEUP:
		iPos = ::SendMessage(scrollBar, TBM_GETPOS, 0, 0);
		SetDlgItemTextA(IDC_STATIC_LEVEL, ultoa(iPos, szStr, 10));
		CheckDlgButton(IDC_CHECK_RECPMPRESS, BST_CHECKED);

		break;
	}
	return 0;
}

BOOL TRebuildOptDlg::ParseScript()
{
	GetDlgItemText(IDC_EDIT_SCRIPT, szScriptFile, MAX_PATH);
	if(isScriptParseSuccess = (WINPCK_OK == pck_TestScript((LPCTSTR)szScriptFile))) {

		SetDlgItemTextA(IDC_EDIT_RESULT, "解析脚本成功");
		_tcscpy_s(lpszScriptFile, MAX_PATH, szScriptFile);

		::EnableWindow(GetDlgItem(IDOK), TRUE);
		return TRUE;
	} else {
		SetDlgItemTextA(IDC_EDIT_RESULT, "解析脚本失败");
		::EnableWindow(GetDlgItem(IDOK), FALSE);
		return FALSE;
	}
}

BOOL TRebuildOptDlg::EvDropFiles(HDROP hDrop)
{

	TCHAR	szFirstFile[MAX_PATH];

	DWORD dwDropFileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

	if(0 == dwDropFileCount)goto END_DROP;

	if(1 == dwDropFileCount) {
		
		DragQueryFile(hDrop, 0, szFirstFile, MAX_PATH);
		SetDlgItemText(IDC_EDIT_SCRIPT, szFirstFile);
	}


	DragAcceptFiles(hWnd, FALSE);

	ParseScript();

END_DROP:
	DragFinish(hDrop);
	DragAcceptFiles(hWnd, TRUE);
	return	TRUE;
}