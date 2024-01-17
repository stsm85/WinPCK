#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#include <stdint.h>
#include <string>

class CWinControl
{
public:
	CWinControl();
	virtual ~CWinControl();

	//virtual void Attach(HWND hWnd);
	virtual void Attach(int id, HWND hDlg);
	virtual void Detach();

	const HWND Wnd() const {
		return this->m_hWnd;
	}

	virtual const std::string GetTextA();
	virtual void SetText(const char* str);
	virtual void SetText(const wchar_t* str);
	virtual int  GetWindowTextA(LPSTR lpString, int nMaxCount);
	virtual void SetWindowTextA(const char* str);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="nBar">指定要检索其参数的滚动条的类型。 此参数的取值可为下列值之一：
	//值 		含义
	//SB_CTL	检索滚动条控件的参数。 hwnd 参数必须是滚动条控件的句柄。
	//SB_HORZ	检索窗口标准水平滚动条的参数。
	//SB_VERT	检索窗口标准垂直滚动条的参数。 
	// </param>
	/// <param name="lpsi"></param>
	/// <returns></returns>
	virtual BOOL GetScrollInfo(int nBar, SCROLLINFO* lpsi);
	virtual int		GetScrollPosX();
	virtual int		GetScrollPosY();

	virtual BOOL GetClientRect(RECT* rect);

	virtual BOOL IsVScrollOnTop();
	virtual BOOL IsVScrollOnBottom();

	virtual LRESULT SendMsgA(UINT Msg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT SendMsgW(UINT Msg, WPARAM wParam, LPARAM lParam);

	virtual void EnableWindow(BOOL val);

protected:
	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvSysCommand(WPARAM uCmdType, POINTS pos);
	virtual BOOL	EvClose(void);
	virtual BOOL	EvTimer(WPARAM timerID, TIMERPROC proc);
	virtual BOOL	EvMouseMove(UINT fwKeys, POINTS pos);
	virtual BOOL	EvMouseWheel(UINT nFlags, short zDelta, POINTS pos);
	virtual BOOL	EvPaint(void);
	virtual BOOL	EvNcPaint(HRGN hRgn);
	virtual BOOL	EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight);
	virtual BOOL	EvDrawItem(UINT ctlID, DRAWITEMSTRUCT* lpDis);
	//virtual BOOL	EvMenuSelect(UINT uItem, UINT fuFlag, HMENU hMenu);
	virtual BOOL	EvDropFiles(HDROP hDrop);
	virtual BOOL	EvNotify(UINT ctlID, NMHDR* pNmHdr);
	virtual BOOL	EvContextMenu(HWND childWnd, POINTS pos);

	virtual BOOL	EventButton(UINT uMsg, int nHitTest, POINTS pos);
	virtual BOOL	EventKey(UINT uMsg, int nVirtKey, LONG lKeyData);
	//virtual BOOL	EventInitMenu(UINT uMsg, HMENU hMenu, UINT uPos, BOOL fSystemMenu);
	virtual BOOL	EventCtlColor(UINT uMsg, HDC hDcCtl, HWND hWndCtl, HBRUSH* result);
	virtual BOOL	EventFocus(UINT uMsg, HWND focusWnd);
	virtual BOOL	EventSystem(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual BOOL	EventUser(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:

	HWND m_hDlg = NULL;
	int m_id = 0;
	HWND m_hWnd = NULL;
	//std::string m_szText;

	//RECT m_rect;

	static LRESULT CALLBACK WndProcTabcontrol(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT WndProcThis(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	WNDPROC m_OldMsgProc = nullptr;

};

