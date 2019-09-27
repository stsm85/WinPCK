//////////////////////////////////////////////////////////////////////
// mainControlStatus.cpp: WinPCK 界面线程部分
// 界面控件功能
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2019.9.10
//////////////////////////////////////////////////////////////////////

#include "tlib.h"
#include "resource.h"
//#include "globals.h"
#include "winmain.h"


void TInstDlg::SetStatusBarText(int	iPart, LPCSTR	lpszText)
{
	SendDlgItemMessageA(IDC_STATUS, SB_SETTEXTA, iPart, (LPARAM)lpszText);
	SendDlgItemMessageA(IDC_STATUS, SB_SETTIPTEXTA, iPart, (LPARAM)lpszText);
}

void TInstDlg::SetStatusBarText(int	iPart, LPCWSTR	lpszText)
{
	SendDlgItemMessageW(IDC_STATUS, SB_SETTEXTW, iPart, (LPARAM)lpszText);
	SendDlgItemMessageW(IDC_STATUS, SB_SETTIPTEXTW, iPart, (LPARAM)lpszText);
}

void TInstDlg::SetStatusBarTitle(LPCWSTR lpszText)
{
	SetStatusBarText(0, lpszText);
}

void TInstDlg::SetStatusBarFileSize(uint64_t size)
{
	wchar_t	szString[64];
	swprintf_s(szString, 64, GetLoadStrW(IDS_STRING_OPENFILESIZE), size);
	SetStatusBarText(1, szString);
}

void TInstDlg::SetStatusBarFileCount(uint32_t size)
{
	wchar_t	szString[64];
	swprintf_s(szString, 64, GetLoadStrW(IDS_STRING_OPENFILECOUNT), size);
	SetStatusBarText(2, szString);
}

void TInstDlg::ClearStatusBarProgress()
{
	SetStatusBarText(3, "");
}

void TInstDlg::SetStatusBarProgress(LPCWSTR lpszText)
{
	SetStatusBarText(3, lpszText);
}

void TInstDlg::SetStatusBarInfo(LPCWSTR lpszText)
{
	SetStatusBarText(4, lpszText);
}