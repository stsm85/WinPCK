#include "showimgpch.h"
#include "ShowPictureWithZoom.h"

#include <mutex>
//std::map<HWND, WNDINFO> CShowPictureWithZoom::sm_WinClassMap;

static std::mutex pthis_mutex;
static std::map<HWND, CShowPictureWithZoom*> pThis;

bool CShowPictureWithZoom::Attach(HWND hWndShow, LPBYTE _buffer, size_t _bufsize, LPCWSTR _lpszFileTitle, PICFORMAT _picFormat)
{
	CShowPicture::Attach(hWndShow, _buffer, _bufsize, _lpszFileTitle, _picFormat);

	//auto OldMsgProc = (WNDPROC)::SetWindowLongPtr(hWndShow, GWLP_WNDPROC, (LONG_PTR)CShowPictureWithZoom::WinProc);
	//sm_WinClassMap.insert({ hWndShow, {.OldMsgProc = OldMsgProc, .pThis = this} });
	//this->m_hWndShow = hWndShow;

#if 1
	std::lock_guard<std::mutex> lock(pthis_mutex);
	pThis.insert({ hWndShow , this });

	this->m_OldMsgProc = (WNDPROC)::SetWindowLongPtr(hWndShow, GWLP_WNDPROC, (LONG_PTR)CShowPictureWithZoom::WinProcStatic);
#endif
	return this->IsDecoded();
}

void CShowPictureWithZoom::Detach()
{
	CShowPicture::Detach();

#if 1
	std::lock_guard<std::mutex> lock(pthis_mutex);

	auto it = pThis.find(this->m_hWndShow);
	if (it != pThis.end())
	{
		pThis.erase(it);
	}
	::SetWindowLongPtr(this->m_hWndShow, GWLP_WNDPROC, (LONG_PTR)this->m_OldMsgProc);
#endif
}

LRESULT CALLBACK CShowPictureWithZoom::WinProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto p_this = pThis[hWnd];
	return p_this->WinProc(hWnd, message, wParam, lParam);
}

LRESULT CShowPictureWithZoom::WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
		return this->EvPaint();
		break;

	case WM_SIZE:
		return this->EvSize((UINT)wParam, LOWORD(lParam), HIWORD(lParam));
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
		return this->EventButton(uMsg, (int)wParam, MAKEPOINTS(lParam));
		break;

	case WM_MOUSEWHEEL:
		return	this->EvMouseWheel(LOWORD(wParam), HIWORD(wParam), MAKEPOINTS(lParam));
		break;

	case WM_MOUSEMOVE:
		return EvMouseMove((UINT)wParam, MAKEPOINTS(lParam));
		break;

	}
	return ::CallWindowProcW(this->m_OldMsgProc, hWnd, uMsg, wParam, lParam);
}

#if 1
BOOL CShowPictureWithZoom::EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight)
{

	this->ChangeClientSize(nWidth, nHeight);

#ifdef _DEBUG
	//::OutputDebugStringA(std::format("func:{}, nWidth={}, nHeight={}\r\n", __FUNCTION__, nWidth, nHeight).c_str());
#endif
	return	FALSE;
}
#endif

BOOL CShowPictureWithZoom::EventButton(UINT uMsg, int nHitTest, POINTS pos)
{
	switch (uMsg) {
	case WM_LBUTTONUP:
		::ReleaseCapture();
		this->isMouseDown = false;
		break;

	case WM_LBUTTONDOWN:

		this->isMouseDown = true;
		::SetCapture(this->m_hWndShow);
		GetCursorPos(&this->pointMouse);

		break;

	case WM_RBUTTONDOWN:

		this->ZoomToOriginalResolution(pos.x, pos.y);
		break;
	case WM_LBUTTONDBLCLK:
		//µ¼³ö
		try {
			this->m_dbClickFunc();
		}
		catch (...)
		{
			;
		}
		break;

	}

	return FALSE;
}

BOOL CShowPictureWithZoom::EvMouseWheel(UINT nFlags, short zDelta, POINTS pos)
{
	//if (NULL != lpShowPicture) {

		POINT pointMouse = { pos.x , pos.y };
		ScreenToClient(this->m_hWndShow, &pointMouse);

		this->ZoomWithMousePoint(nFlags, zDelta, pointMouse.x, pointMouse.y);

		//FreshWindowTitle();
	//}
	return FALSE;
}

BOOL CShowPictureWithZoom::EvMouseMove(UINT fwKeys, POINTS pos)
{
	if (this->isMouseDown) {
		POINT	pointLast = this->pointMouse;
		GetCursorPos(&this->pointMouse);

		int		xOffset = pointLast.x - this->pointMouse.x;
		int		yOffset = pointLast.y - this->pointMouse.y;

		//if (NULL != lpShowPicture)
			this->MovePicture(xOffset, yOffset);

	}

	return FALSE;
}

BOOL CShowPictureWithZoom::EvPaint(void)
{
	this->Paint(false);
	return FALSE;
}