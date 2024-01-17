#include "WinControl.h"

#include <unordered_map>
#include <mutex>

static std::mutex pthis_mutex;
static std::unordered_map<HWND, CWinControl*> pThis;

CWinControl::CWinControl()
{
	
}

CWinControl::~CWinControl()
{
	this->Detach();
	//::SetWindowLongPtr(this->m_hWnd, GWLP_WNDPROC, (LONG_PTR)this->m_OldMsgProc);
}

void CWinControl::Attach(int id, HWND hDlg)
{
	m_hDlg = hDlg;
	m_id = id;
	m_hWnd = ::GetDlgItem(hDlg, id);

	//::GetClientRect(this->m_hWnd, &this->m_rect);

	std::lock_guard<std::mutex> lock(pthis_mutex);
	pThis.insert({ m_hWnd , this });
	this->m_OldMsgProc = (WNDPROC)::SetWindowLongPtr(this->m_hWnd, GWLP_WNDPROC, (LONG_PTR)CWinControl::WndProcTabcontrol);

}

void CWinControl::Detach()
{
	std::lock_guard<std::mutex> lock(pthis_mutex);

	auto it = pThis.find(this->m_hWnd);
	if (it != pThis.end())
	{
		pThis.erase(it);
	}
	::SetWindowLongPtr(this->m_hWnd, GWLP_WNDPROC, (LONG_PTR)this->m_OldMsgProc);

}

void CWinControl::SetText(const char* str)
{
	::SetWindowTextA(m_hWnd, str);
}

void CWinControl::SetText(const wchar_t* str)
{
	::SetWindowTextW(m_hWnd, str);
}

int CWinControl::GetWindowTextA(LPSTR lpString, int nMaxCount)
{
	return ::GetWindowTextA(m_hWnd, lpString, nMaxCount);
}

void CWinControl::SetWindowTextA(const char* str)
{
	::SetWindowTextA(m_hWnd, str);
}

const std::string CWinControl::GetTextA()
{
	auto size = ::GetWindowTextLengthA(this->m_hWnd);
	std::string str;
	str.reserve(size + 1);
	str.resize(size);

	::GetWindowTextA(m_hWnd, str.data(), size + 1);
	return std::move(str);
}

LRESULT CWinControl::SendMsgA(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return ::SendMessageA(m_hWnd, Msg, wParam, lParam);
}

LRESULT CWinControl::SendMsgW(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return ::SendMessageW(m_hWnd, Msg, wParam, lParam);
}

void CWinControl::EnableWindow(BOOL val)
{
	::EnableWindow(m_hWnd, val);
}

BOOL CWinControl::GetScrollInfo(int nBar, SCROLLINFO* lpsi)
{
	return ::GetScrollInfo(this->m_hWnd, nBar, lpsi);
}

int CWinControl::GetScrollPosX()
{
	SCROLLINFO si{
		.cbSize = sizeof(SCROLLINFO),
		.fMask = SIF_POS
	};

	this->GetScrollInfo(SB_HORZ, &si);
	return si.nPos;
}

int CWinControl::GetScrollPosY()
{
	SCROLLINFO si{
		.cbSize = sizeof(SCROLLINFO),
		.fMask = SIF_POS
	};

	this->GetScrollInfo(SB_VERT, &si);
	return si.nPos;
}

BOOL CWinControl::IsVScrollOnTop()
{
	return this->GetScrollPosY() < 4;
}

BOOL CWinControl::IsVScrollOnBottom()
{
	SCROLLINFO si{
		.cbSize = sizeof(SCROLLINFO),
		.fMask = SIF_ALL
	};

	if (!this->GetScrollInfo(SB_VERT, &si))
		return TRUE;

	if (si.nPos >= (si.nMax - si.nPage))
		return TRUE;

	return FALSE;
}


BOOL CWinControl::GetClientRect(RECT* rect)
{
	return ::GetClientRect(this->m_hWnd, rect);
}

LRESULT CALLBACK CWinControl::WndProcTabcontrol(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto p_this = pThis[hWnd];
	return p_this->WndProcThis(hWnd, message, wParam, lParam);
}

LRESULT CWinControl::WndProcThis(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL	done = FALSE;
	LRESULT	result = 0;

	switch (uMsg)
	{
	case WM_CLOSE:
		done = EvClose();
		break;

	case WM_COMMAND:
		done = EvCommand(HIWORD(wParam), LOWORD(wParam), lParam);
		break;

	case WM_SYSCOMMAND:
		done = EvSysCommand(wParam, MAKEPOINTS(lParam));
		break;

	case WM_TIMER:
		done = EvTimer(wParam, (TIMERPROC)lParam);
		break;

	case WM_MOUSEMOVE:
		done = EvMouseMove((UINT)wParam, MAKEPOINTS(lParam));
		break;

	case WM_MOUSEWHEEL:
		done = EvMouseWheel(LOWORD(wParam), HIWORD(wParam), MAKEPOINTS(lParam));
		break;

	case WM_PAINT:
		done = EvPaint();
		break;

	case WM_NCPAINT:
		done = EvNcPaint((HRGN)wParam);
		break;

	case WM_SIZE:
		done = EvSize((UINT)wParam, LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_DRAWITEM:
		result = done = EvDrawItem((UINT)wParam, (LPDRAWITEMSTRUCT)lParam);
		break;

	case WM_NOTIFY:
		result = done = EvNotify((UINT)wParam, (LPNMHDR)lParam);
		break;

	case WM_CONTEXTMENU:
		result = done = EvContextMenu((HWND)wParam, MAKEPOINTS(lParam));
		break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_NCLBUTTONUP:
	case WM_NCRBUTTONUP:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_NCLBUTTONDOWN:
	case WM_NCRBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_NCLBUTTONDBLCLK:
	case WM_NCRBUTTONDBLCLK:
		done = EventButton(uMsg, (int)wParam, MAKEPOINTS(lParam));
		break;

	case WM_CHAR:
	case WM_KEYUP:
	case WM_KEYDOWN:
		done = EventKey(uMsg, (int)wParam, (LONG)lParam);
		break;

	case WM_DROPFILES:
		done = EvDropFiles((HDROP)wParam);
		break;
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORMSGBOX:
	case WM_CTLCOLORSCROLLBAR:
	case WM_CTLCOLORSTATIC:
		done = EventCtlColor(uMsg, (HDC)wParam, (HWND)lParam, (HBRUSH*)&result);
		break;

	case WM_KILLFOCUS:
	case WM_SETFOCUS:
		done = EventFocus(uMsg, (HWND)wParam);
		break;

	default:
		if (uMsg >= WM_USER && uMsg < 0x7FFF || uMsg >= 0xC000 && uMsg <= 0xFFFF)
			result = done = EventUser(uMsg, wParam, lParam);
		else
			result = done = EventSystem(uMsg, wParam, lParam);
		break;
	}

	return done ? result : ::CallWindowProcW(this->m_OldMsgProc, hWnd, uMsg, wParam, lParam);
}

BOOL CWinControl::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	return	FALSE;
}

BOOL CWinControl::EvSysCommand(WPARAM uCmdType, POINTS pos)
{
	return	FALSE;
}

BOOL CWinControl::EvClose(void)
{
	return	FALSE;
}

BOOL CWinControl::EvTimer(WPARAM timerID, TIMERPROC proc)
{
	return	FALSE;
}

BOOL CWinControl::EvMouseMove(UINT fwKeys, POINTS pos)
{
	return	FALSE;
}

BOOL CWinControl::EvMouseWheel(UINT nFlags, short zDelta, POINTS pos)
{
	return	FALSE;
}

BOOL CWinControl::EvDrawItem(UINT ctlID, DRAWITEMSTRUCT* lpDis)
{
	return	FALSE;
}

BOOL CWinControl::EvPaint(void)
{
	return	FALSE;
}

BOOL CWinControl::EvNcPaint(HRGN hRgn)
{
	return	FALSE;
}

BOOL CWinControl::EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight)
{
	//::GetClientRect(this->m_hWnd, &this->m_rect);
	return	FALSE;
}

BOOL CWinControl::EvNotify(UINT ctlID, NMHDR* pNmHdr)
{
	return	FALSE;
}

BOOL CWinControl::EvContextMenu(HWND childWnd, POINTS pos)
{
	return	FALSE;
}

BOOL CWinControl::EventButton(UINT uMsg, int nHitTest, POINTS pos)
{
	return	FALSE;
}

BOOL CWinControl::EventKey(UINT uMsg, int nVirtKey, LONG lKeyData)
{
	return	FALSE;
}

BOOL CWinControl::EvDropFiles(HDROP hDrop)
{
	return	FALSE;
}

BOOL CWinControl::EventCtlColor(UINT uMsg, HDC hDcCtl, HWND hWndCtl, HBRUSH* result)
{
	return	FALSE;
}

BOOL CWinControl::EventFocus(UINT uMsg, HWND hFocusWnd)
{
	return	FALSE;
}

BOOL CWinControl::EventUser(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return	FALSE;
}

BOOL CWinControl::EventSystem(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return	FALSE;
}