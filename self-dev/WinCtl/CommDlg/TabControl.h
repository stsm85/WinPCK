#pragma once
#include "WinControl.h"

#include <CommCtrl.h>
#include <map>

//#define WM_USER_CLOSE_TAB	(WM_USER + 1)

class CTabControl :
	public CWinControl
{
public:
	CTabControl() = default;
	virtual ~CTabControl() = default;

	//virtual void Attach(int id, HWND hDlg) override;

public:
	void AddTab(HWND hWnd, const std::string& title);
	void AddTab(HWND hWnd, const std::wstring& title);

private:
	void SetAddedDlg(HWND hWnd, int index);

public:
	int SetCurSel(int index);
	int GetCurSel();
	int GetItemCount();

	BOOL	EvNotify(NMHDR* pNmHdr);

//protected:
//	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
public:
	//virtual BOOL	EvNotify(UINT ctlID, NMHDR* pNmHdr);
protected:
	virtual BOOL	EventButton(UINT uMsg, int nHitTest, POINTS pos);
	//virtual BOOL	EventSystem(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//virtual BOOL	EventUser(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

	void ChangeTab(int sel, bool close_to_change = false);


	HWND m_showing_Wnd = nullptr;

	std::map<int, HWND> m_tab_dlgs;

};

