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

#include "guipch.h"
#include "winmain.h"

void TInstDlg::SetStatusBarText(int	iPart, LPCWSTR	lpszText)
{
	this->SendDlgItemMessageW(IDC_STATUS, SB_SETTEXTW, iPart, (LPARAM)lpszText);
	this->SendDlgItemMessageW(IDC_STATUS, SB_SETTIPTEXTW, iPart, (LPARAM)lpszText);
}

void TInstDlg::SetStatusBarTitle(LPCWSTR lpszText)
{
	this->SetStatusBarText(0, lpszText);
}

void TInstDlg::SetStatusBarFileSize(uint64_t size)
{
	this->SetStatusBarText(1, std::vformat(::GetLoadStrW(IDS_STRING_OPENFILESIZE), std::make_wformat_args(size)).c_str());
}

void TInstDlg::SetStatusBarFileCount(uint32_t size)
{
	this->SetStatusBarText(2, std::vformat(::GetLoadStrW(IDS_STRING_OPENFILECOUNT), std::make_wformat_args(size)).c_str());
}

void TInstDlg::ClearStatusBarProgress()
{
	this->SetStatusBarText(3, L"");
}

void TInstDlg::SetStatusBarProgress(LPCWSTR lpszText)
{
	this->SetStatusBarText(3, lpszText);
}

void TInstDlg::SetStatusBarInfo(LPCWSTR lpszText)
{
	this->SetStatusBarText(4, lpszText);
}