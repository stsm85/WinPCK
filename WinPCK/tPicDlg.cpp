//////////////////////////////////////////////////////////////////////
// tPicDlg.cpp: WinPCK 界面线程部分
// 对话框代码
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2017.12.26
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4018 )

#include <assert.h>
#include "PreviewDlg.h"
#include <stdio.h>
#include <tchar.h>
#include "OpenSaveDlg.h"
#include "CharsCodeConv.h"

TPicDlg::TPicDlg(LPBYTE *_buf, UINT32 _dwSize, PICFORMAT _iFormat, const TCHAR *_lpszFile, TWin *_win) :
	TDlg(IDD_DIALOG_PIC, _win),
	lpShowPicture(NULL),
	isMouseDown(FALSE),
	buf(_buf),
	dwSize(_dwSize),
	lpszFile(_lpszFile),
	iFormat(_iFormat),
	m_dZoomRatio(1.0),
	m_ShowX(0),
	m_ShowY(0),
	m_ShowPicWidth(0),
	m_ShowPicHeight(0)
{ }

TPicDlg::~TPicDlg()
{
	if (NULL != lpShowPicture)
		delete lpShowPicture;
}

BOOL TPicDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	switch(wID) {
	case IDOK:
	case IDCANCEL:

		EndDialog(wID);
		return	TRUE;

	}
	return	FALSE;
}

BOOL TPicDlg::SaveFile()
{
	//导出
	if(FMT_RAW != iFormat) {
		WCHAR	szFilename[MAX_PATH];
		::GetCurrentDirectoryW(MAX_PATH, szFilename);

		WCHAR *lpTail = szFilename + wcslen(szFilename) - 1;
		if('\\' == *lpTail) {
			*lpTail = 0;
		}

		wcscat_s(szFilename, MAX_PATH, L"\\");
#ifdef UNICODE
		wcscat_s(szFilename, MAX_PATH, lpszFile);
#else
		CAnsi2Ucs cA2U;
		wcscat_s(szFilename, MAX_PATH, cA2U.GetString(lpszFile));
#endif
		wcscpy(wcsrchr(szFilename, L'.'), L".png\0\0");

		if(::SaveFile(hWnd, szFilename, L"png", TEXT_SAVE_FILTER)) {

			return lpShowPicture->Save(szFilename);
		}
	}
	
	return TRUE;
}

double TPicDlg::CalcFixedRatio(UINT uRealX, UINT uRealY, UINT uToX, UINT uToY)
{
	//计算一下多大是正好的缩放率
	 double zoomx = uToX / (double)uRealX;
	 double zoomy = uToX / (double)uRealX;

	 return zoomx > zoomy ? zoomy : zoomx;
}

void TPicDlg::InitFixedShowPositionAndShowWindow()
{
	//RECT	rectWin;

	//当前分辨率
	const int width = GetSystemMetrics(SM_CXSCREEN);
	const int height = GetSystemMetrics(SM_CYSCREEN);

	double screenx_d_realx = width / (double)m_RealPicWidth;
	double screeny_d_realy = height / (double)m_RealPicHeight;

	BOOL bNeedShowMax = TRUE;
	//比较screenx_d_realx和screeny_d_realy，哪个小说明哪个边先越界
	if(screenx_d_realx < screeny_d_realy) {

		if(width < m_RealPicWidth) {
			m_dZoomRatio = screenx_d_realx;

		} else {
			bNeedShowMax = FALSE;
			
		}
	} else {

		if(height < m_RealPicHeight) {
			m_dZoomRatio = screeny_d_realy;
			
		} else {
			bNeedShowMax = FALSE;

		}
	}
	m_ShowPicWidth = m_dZoomRatio * m_RealPicWidth;
	m_ShowPicHeight = m_dZoomRatio * m_RealPicHeight;

	FreshWindowTitle();

	if(bNeedShowMax) {
		ShowWindow(SW_SHOWMAXIMIZED);
	} else {
		::SetWindowPos(hWnd, NULL, 0, 0, m_RealPicWidth + 10, m_RealPicHeight + 30, SWP_NOZORDER | SWP_NOMOVE);
		//显示窗口
		Show();
	}
}

void TPicDlg::FreshWindowTitle()
{
	TCHAR szTitle[MAX_PATH];
	_stprintf_s(szTitle, TEXT("%s (%d%%)"), m_szTitle, (int)(m_dZoomRatio * 100.0 + 0.5));
	SetWindowText(szTitle);
}

BOOL TPicDlg::EvCreate(LPARAM lParam)
{
	//HGLOBAL		hG;
	//调整static大小
	RECT	rectWin;
	GetClientRect(hWnd, &rectWin);
	//::SetWindowPos(GetDlgItem(IDC_STATIC_PIC), NULL, 0, 0, rectWin.right, rectWin.bottom, SWP_NOZORDER | SWP_NOMOVE);

	lpShowPicture = new CShowPicture(hWnd, *buf, dwSize, lpszFile, iFormat);

	if (!lpShowPicture->Decode())
		return FALSE;

	lpShowPicture->GetWindowTitle(m_szTitle, sizeof(m_szTitle) / sizeof(TCHAR));

	m_RealPicWidth = lpShowPicture->GetWidth();
	m_RealPicHeight = lpShowPicture->GetHeight();

	InitFixedShowPositionAndShowWindow();

	return	FALSE;
}


BOOL TPicDlg::EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight)
{
	m_ShowX = ((int)nWidth - (int)m_ShowPicWidth) / 2;
	m_ShowY = ((int)nHeight - (int)m_ShowPicHeight) / 2;

	//计算一下多大是正好的缩放率
	m_fixedZoomRatio = CalcFixedRatio(m_RealPicWidth, m_RealPicHeight, nWidth, nHeight);

	DebugA("set1 nWidth = %d, m_ShowPicWidth = %d\r\n", nWidth, m_ShowPicWidth);
	DebugA("set1 m_ShowX = %d, m_ShowY = %d\r\n", m_ShowX, m_ShowY);

	EvPaint();
	return	FALSE;

}

BOOL TPicDlg::EventButton(UINT uMsg, int nHitTest, POINTS pos)
{
	switch(uMsg) {
	case WM_LBUTTONUP:
		//ReleaseCapture();
		isMouseDown = FALSE;
		break;

	case WM_LBUTTONDOWN:

		isMouseDown = TRUE;
		//SetCapture();
		GetCursorPos(&pointMouse);

		break;

	case WM_RBUTTONDOWN:

		ZoomPictureAtPoint(1.0, &pos);

		break;
	case WM_LBUTTONDBLCLK:
		//导出
		this->SaveFile();
		break;

	}

	return FALSE;
}

#define CHANGE_RATIO	1.2
#define MAX_ZOOM_RATIO	32.0

void TPicDlg::ZoomPictureAtPoint(double dZoomRatio, POINTS *pos)
{
	POINTS thispos;
	if(NULL == pos) {
		GetCursorPos(&pointMouse);
		thispos.x = pointMouse.x;
		thispos.y = pointMouse.y;
		pos = &thispos;
	}

	m_dZoomRatio = dZoomRatio;

	/*
	计算m_ShowX,m_ShowY
	当前显示大小大于窗口时：
	缩放以当前鼠标位置为中心缩放
	假设鼠标的点在图片上的位置为rx,ry, 鼠标在窗口上的位置为mx,my，图片0,0在窗口上的显示点为sx,sy，则rx=mx-sx
	缩放以当前鼠标位置为中心缩放，则鼠标在图片上的点不动，就是向量值不动，设x方向上的向量值为p,设当前图片宽度为sw，则p=rx/sw=(mx-sx)/sw
	缩放后图片宽度变化为sw1,显示点变化为sx1，p不变，p=(mx-sx1)/sw1，得到sx1 = mx - (mx-sx)*sw1/sw
	*/
	POINT pointMouse = { pos->x , pos->y };
	ScreenToClient(hWnd, &pointMouse);
	int mouseX = pointMouse.x, mouseY = pointMouse.y;
	//当鼠标点在图像之外时，以图像边界为准
	if(m_ShowX > mouseX)mouseX = m_ShowX;
	if(m_ShowY > mouseY)mouseY = m_ShowY;
	int iShowRight = m_ShowX + m_ShowPicWidth;
	int iShowBottom = m_ShowY + m_ShowPicHeight;
	if(iShowRight < mouseX)mouseX = iShowRight;
	if(iShowBottom < mouseY)mouseY = iShowBottom;

	//计算缩放后的m_ShowX, m_ShowY
	UINT	dwShowPicWidth = m_dZoomRatio * m_RealPicWidth;
	UINT	dwShowPicHeight = m_dZoomRatio * m_RealPicHeight;

	//当显示的图片宽度大于当前窗口时
	RECT	rectWin;
	GetClientRect(hWnd, &rectWin);

	if(rectWin.right < dwShowPicWidth) {
		m_ShowX = mouseX - (mouseX - m_ShowX) * dwShowPicWidth / m_ShowPicWidth;
		if(0 < m_ShowX)m_ShowX = 0;
	} else {

		m_ShowX = (rectWin.right - dwShowPicWidth) / 2;
	}

	//当显示的图片高度大于当前窗口时
	if(rectWin.bottom < dwShowPicHeight) {
		m_ShowY = mouseY - (mouseY - m_ShowY) * dwShowPicHeight / m_ShowPicHeight;
		if(0 < m_ShowY)m_ShowY = 0;
	} else {

		m_ShowY = (rectWin.bottom - dwShowPicHeight) / 2;
	}

	m_ShowPicWidth = dwShowPicWidth;
	m_ShowPicHeight = dwShowPicHeight;

	EvPaint();

	FreshWindowTitle();

}

BOOL TPicDlg::EvMouseWheel(UINT nFlags, short zDelta, POINTS pos)
{

	DebugA("%s: nFlags = %x, zDelta = %d\r\n", __FUNCTION__, nFlags, zDelta);
	//最大放大倍数为8倍，最小为适应当前窗口大小
	double dZoomChangeRatio = 1.0;
	//放大
	if(0 < zDelta) {

		dZoomChangeRatio *= (zDelta / 120.0 * CHANGE_RATIO);
	} else if(0 > zDelta)
	{
		dZoomChangeRatio /= (zDelta / (-120.0) * CHANGE_RATIO);
	}

	//限制缩放范围
	double dMinZoomRatio = (m_fixedZoomRatio > 0.99) ? 1.0 : m_fixedZoomRatio;

	if(((MAX_ZOOM_RATIO + 0.1) < m_dZoomRatio) || (dMinZoomRatio > (m_dZoomRatio + 0.1)))
		return FALSE;

	m_dZoomRatio *= dZoomChangeRatio;
	if(m_dZoomRatio > MAX_ZOOM_RATIO)m_dZoomRatio = MAX_ZOOM_RATIO;
	if(m_dZoomRatio < dMinZoomRatio) m_dZoomRatio = dMinZoomRatio;

	ZoomPictureAtPoint(m_dZoomRatio, &pos);

	return FALSE;
}

BOOL TPicDlg::EvMouseMove(UINT fwKeys, POINTS pos)
{

	if(isMouseDown) {
		POINT	pointLast = pointMouse;
		GetCursorPos(&pointMouse);

		int		xOffset = pointLast.x - pointMouse.x;
		int		yOffset = pointLast.y - pointMouse.y;

		RECT	rectWin;
		GetClientRect(hWnd, &rectWin);

		//当显示的图片宽度大于当前窗口时
		if(rectWin.right < m_ShowPicWidth) {

			int min_x = (int)rectWin.right - (int)m_ShowPicWidth;

			//当显示区域大于窗口时，图片左上角不能大于0
			//鼠标向左动
			if(xOffset > 0) {

				if(min_x <= m_ShowX) {
					m_ShowX -= xOffset;
					if(m_ShowX < min_x) m_ShowX = min_x;
				}
			} else {

				if(0 >= m_ShowX) {
					m_ShowX -= xOffset;
					if(m_ShowX > 0)m_ShowX = 0;
				}
			}

		}

		//当显示的图片高度大于当前窗口时
		if(rectWin.bottom < m_ShowPicHeight) {

			int min_y = (int)rectWin.bottom - (int)m_ShowPicHeight;

			//鼠标向上动
			if(yOffset > 0) {

				if(min_y <= m_ShowY) {
					m_ShowY -= yOffset;
					if(m_ShowY < min_y) m_ShowY = min_y;
				}
			} else {

				if(0 >= m_ShowY) {
					m_ShowY -= yOffset;
					if(m_ShowY > 0)m_ShowY = 0;
				}
			}

		}
		DebugA("set2 m_ShowX = %d, m_ShowY = %d\r\n", m_ShowX, m_ShowY);

		EvPaint();
	}

	return FALSE;
}

BOOL TPicDlg::EvPaint(void)
{
	DebugA("m_ShowX = %d, m_ShowY = %d\r\n", m_ShowX, m_ShowY);
	lpShowPicture->Paint(m_ShowX, m_ShowY, 0, 0, m_dZoomRatio);


	//计算无效区域
	//x
	RECT	rectWin;
	GetClientRect(hWnd, &rectWin);

	if(m_ShowX > 0) {

		RECT rectInvalid = { 0, 0, m_ShowX, rectWin.bottom };
		InvalidateRect(hWnd, &rectInvalid, TRUE);
	}

	if(m_ShowY > 0) {

		RECT rectInvalid = { m_ShowX >0 ? m_ShowX : 0, 0, rectWin.right, m_ShowY };
		InvalidateRect(hWnd, &rectInvalid, TRUE);
	}

	int iShowBottom = m_ShowY + m_ShowPicHeight;

	if(iShowBottom < rectWin.bottom) {

		RECT rectInvalid = { m_ShowX >0 ? m_ShowX : 0, iShowBottom, rectWin.right, rectWin.bottom };
		InvalidateRect(hWnd, &rectInvalid, TRUE);
	}

	int iShowRight = m_ShowX + m_ShowPicWidth;
	if(iShowRight < rectWin.right) {

		RECT rectInvalid = { iShowRight, m_ShowY >0 ? m_ShowY : 0,rectWin.right, iShowBottom < rectWin.bottom ? iShowBottom : rectWin.bottom };
		InvalidateRect(hWnd, &rectInvalid, TRUE);
	}


	return TRUE;
}
