//////////////////////////////////////////////////////////////////////
// MenuButtonFuncs.cpp: WinPCK 界面线程部分
// 界面按钮功能
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2018.9.4
//////////////////////////////////////////////////////////////////////

#include "tlib.h"
#include "resource.h"
#include "globals.h"
#include "winmain.h"
#include <process.h>

void TInstDlg::ListViewEnter()
{
	if (!m_isListviewRenaming)
		DbClickListView(m_cPckCenter.GetListCurrentHotItem());
	else
		keybd_event(VK_TAB, 0, 0, 0);
}

void TInstDlg::MenuClose()
{
	if (lpPckParams->cVarParams.bThreadRunning)
		lpPckParams->cVarParams.bThreadRunning = FALSE;
	ListView_DeleteAllItems(GetDlgItem(IDC_LIST));
	m_cPckCenter.Close();
}

void TInstDlg::MenuInfo()
{
	if (m_cPckCenter.IsValidPck()) {
		TInfoDlg	dlg(m_cPckCenter.GetAdditionalInfo(), this);
		if (dlg.Exec() == TRUE) {
			m_cPckCenter.SetAdditionalInfo();
		}
	}
}

void TInstDlg::MenuSearch()
{
	if (m_cPckCenter.IsValidPck()) {
		TSearchDlg	dlg(m_szStrToSearch, this);
		if (dlg.Exec() == TRUE) {
			SearchPckFiles();
		}
	}
}

void TInstDlg::MenuNew(WORD wID)
{
	if (lpPckParams->cVarParams.bThreadRunning) {
		lpPckParams->cVarParams.bThreadRunning = FALSE;
		EnableButton(wID, FALSE);
	}
	else {
		_beginthread(CreateNewPckFile, 0, this);
	}
}

void TInstDlg::MenuAdd(WORD wID)
{
	if (lpPckParams->cVarParams.bThreadRunning) {
		lpPckParams->cVarParams.bThreadRunning = FALSE;
		EnableButton(wID, FALSE);
	}
	else {
		AddFiles();
	}
}

void TInstDlg::MenuRebuild(WORD wID)
{
	if (lpPckParams->cVarParams.bThreadRunning) {
		lpPckParams->cVarParams.bThreadRunning = FALSE;
		EnableButton(wID, FALSE);
	}
	else {

		lpPckParams->cVarParams.dwMTMemoryUsed = 0;
		_beginthread(RebuildPckFile, 0, this);
	}
}

void TInstDlg::MenuCompressOpt()
{
	TCompressOptDlg	dlg(lpPckParams, this);
	dlg.Exec();
}

void TInstDlg::MenuRename()
{
	if(lpPckParams->cVarParams.bThreadRunning)
		return;

	HWND	hList = GetDlgItem(IDC_LIST);
	::SetWindowLong(hList, GWL_STYLE, ::GetWindowLong(hList, GWL_STYLE) | LVS_EDITLABELS);
	ListView_EditLabel(hList, m_cPckCenter.GetListCurrentHotItem());
}

void TInstDlg::MenuDelete()
{
	if (lpPckParams->cVarParams.bThreadRunning)
		return;
	if (IDNO == MessageBox(GetLoadStr(IDS_STRING_ISDELETE), GetLoadStr(IDS_STRING_ISDELETETITLE), MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON2))
		return;
	lpPckParams->cVarParams.dwMTMemoryUsed = 0;
	_beginthread(DeleteFileFromPckFile, 0, this);
}

void TInstDlg::MenuSelectAll()
{
	LVITEM item = { 0 };

	item.mask = LVIF_STATE;
	item.stateMask = item.state = LVIS_SELECTED;

	HWND hList = GetDlgItem(IDC_LIST);

	int	nItemCount = ListView_GetItemCount(hList);

	for (item.iItem = 1; item.iItem < nItemCount; item.iItem++) {
		ListView_SetItem(hList, &item);
	}
}

void TInstDlg::MenuSelectReverse()
{
	LVITEM item = { 0 };

	item.mask = LVIF_STATE;
	item.stateMask = LVIS_SELECTED;

	HWND hList = GetDlgItem(IDC_LIST);

	int	nItemCount = ListView_GetItemCount(hList);

	for (item.iItem = 1; item.iItem < nItemCount; item.iItem++) {
		ListView_GetItem(hList, &item);
		item.state = LVIS_SELECTED == item.state ? 0 : LVIS_SELECTED;
		ListView_SetItem(hList, &item);
	}
}

void TInstDlg::MenuAttribute()
{
	if (lpPckParams->cVarParams.bThreadRunning)
		return;

	ViewFileAttribute();
}

void TInstDlg::MenuView()
{
	if (lpPckParams->cVarParams.bThreadRunning)
		return;

	ViewFile();
}

void TInstDlg::MenuAbout()
{
	MessageBoxW(THIS_MAIN_CAPTION
		"\r\n"
		THIS_DESC
		"\r\n\r\n"
		THIS_AUTHOR
		"\r\n\r\n"
		THIS_UESDLIB,
		L"关于 "
		THIS_NAME,
		MB_OK | MB_ICONASTERISK);
}