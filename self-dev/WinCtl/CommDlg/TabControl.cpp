#include "TabControl.h"
//#include "../../spdloghelper/spdloghelper/Logger.h"
//#include "../../spdloghelper/spdloghelper/LoggerInit.h"


//void CTabControl::Attach(int id, HWND hDlg)
//{
//	CWinControl::Attach(id, hDlg);
//}

void CTabControl::ChangeTab(int sel, bool close_to_change)
{

	//static CDlgTabBase* dlg_on_show = nullptr;

	//auto tab_wnd = GetDlgItem(IDC_TAB);
	auto count = this->GetItemCount();

	if ((!close_to_change) && (nullptr != this->m_showing_Wnd)) {

		//this->dlg_on_show->Hide();
		::ShowWindow(this->m_showing_Wnd, SW_HIDE);
	}

	TCITEM t{ .mask = TCIF_PARAM };
	TabCtrl_GetItem(this->m_hWnd, sel, &t);
	this->m_showing_Wnd = (HWND)t.lParam;

	// 计算TabCtrl控件的显示区域
	RECT rc;
	::GetClientRect(this->m_hWnd, &rc);  // 获得控件客服区大小
	::SendMessage(this->m_hWnd, TCM_ADJUSTRECT, 0, (LPARAM)&rc);  // 重新计算除了选项卡的显示区域
	::InflateRect(&rc, -2, -2);    // 缩小区域
	::MoveWindow(this->m_showing_Wnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE); // 把对话框移动到显示区域

	::ShowWindow(this->m_showing_Wnd, SW_SHOW);

}

//void CTabControl::add_log_tab()
//{
//	try {
//		std::unique_ptr<CDlgTabBase> dlg_logger(new CDlgLogger(this->m_hWnd));
//		add_tab(std::move(dlg_logger), L"日志");
//
//		Logger.i("打开日志窗口");
//	}
//	catch (std::exception ex) {
//		Logger.e(ex.what());
//	}
//}
//
//void CTabControl::add_tool_tab()
//{
//	try {
//		std::unique_ptr<CDlgTabBase> dlg_tool(new CDlgTools(this->m_hWnd));
//		add_tab(std::move(dlg_tool), L"工具");
//		Logger.i("打开工具窗口");
//	}
//	catch (std::exception ex) {
//		Logger.e(ex.what());
//	}
//}

void CTabControl::AddTab(HWND hWnd, const std::string& title)
{
	TCITEMA t = { .mask = TCIF_TEXT | TCIF_PARAM, 
		.pszText = (decltype(TCITEMA::pszText))title.c_str(),
		.lParam = (LPARAM)hWnd };

	auto tab_count = this->GetItemCount();
	::SendMessageA(this->m_hWnd, TCM_INSERTITEMA, tab_count, (LPARAM)&t);

	this->SetAddedDlg(hWnd, tab_count);
}

void CTabControl::AddTab(HWND hWnd, const std::wstring& title)
{
	TCITEMW t = { .mask = TCIF_TEXT | TCIF_PARAM, 
		.pszText = (decltype(TCITEMW::pszText))title.c_str(), 
		.lParam = (LPARAM)hWnd };

	auto tab_count = this->GetItemCount();
	::SendMessageW(this->m_hWnd, TCM_INSERTITEMW, tab_count, (LPARAM)&t);

	this->SetAddedDlg(hWnd, tab_count);
}

void CTabControl::SetAddedDlg(HWND hWnd, int index)
{
	this->SetCurSel(index);
	m_tab_dlgs.insert({ index, hWnd });

	::SetParent(hWnd, this->m_hWnd);

	//::GetWindowRect(parent, &rc);
	//pt.x = rc.left + 1;
	//pt.y = rc.top + 22;

	//ScreenToClient(parent, &pt);
	//SetWindowPos(0, pt.x, pt.y, rc.right - rc.left - 4, rc.bottom - rc.top - 24, SWP_NOZORDER);
	//SetWindowLong(GWL_EXSTYLE, GetWindowLong(GWL_EXSTYLE) | WS_EX_CONTROLPARENT);

	ChangeTab(index);
}

int CTabControl::SetCurSel(int index)
{
	return TabCtrl_SetCurSel(this->m_hWnd, index);
}

int CTabControl::GetCurSel()
{
	return TabCtrl_GetCurSel(this->m_hWnd);
}

int CTabControl::GetItemCount()
{
	return TabCtrl_GetItemCount(this->m_hWnd);
}


BOOL CTabControl::EvNotify(NMHDR* pNmHdr)
{
	switch (pNmHdr->code) {


	case TCN_SELCHANGE:

		auto cursel = this->GetCurSel();
		ChangeTab(cursel);
		break;
	}
	return FALSE;
}


BOOL CTabControl::EventButton(UINT uMsg, int nHitTest, POINTS pos)
{
	//switch (uMsg)
	//{

	//case IDC_TAB:

		//Logger->info("EventButton uMsg:{}, nHitTest:{}, pos:{},{}", uMsg, nHitTest, pos.x, pos.y);
		//break;
	//}
	return FALSE;
}
