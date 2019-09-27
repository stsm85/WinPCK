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
#include "tPreviewDlg.h"
#include <stdio.h>
#include <tchar.h>
#include "OpenSaveDlg.h"
//#include "CharsCodeConv.h"

#define MIN_CX	640
#define MIN_CY	480

TPicDlg::TPicDlg(LPBYTE *_buf, UINT32 _dwSize, PICFORMAT _iFormat, const wchar_t *_lpszFile, TWin *_win) :
	TDlg(IDD_DIALOG_PIC, _win),
	lpShowPicture(NULL),
	isMouseDown(FALSE),
	buf(_buf),
	dwSize(_dwSize),
	lpszFile(_lpszFile),
	iFormat(_iFormat)
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
		wcscat_s(szFilename, MAX_PATH, lpszFile);
		wcscpy(wcsrchr(szFilename, L'.'), L".png\0\0");

		if(::SaveFile(hWnd, szFilename, L"png", TEXT_SAVE_FILTER)) {

			return lpShowPicture->Save(szFilename);
		}
	}
	
	return TRUE;
}

void TPicDlg::InitFixedShowPositionAndShowWindow()
{

	__int64 cx, cy;
	BOOL bNeedShowMax = lpShowPicture->CalcFixedRatioAndSizeOnInit(cx, cy, MIN_CX, MIN_CY);

	FreshWindowTitle();

	if(bNeedShowMax) {

		ShowWindow(SW_SHOWMAXIMIZED);
	} else {

		::SetWindowPos(hWnd, NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE);
		//显示窗口
		Show();
	}
}

void TPicDlg::FreshWindowTitle()
{
	wchar_t szTitle[MAX_PATH];
	swprintf_s(szTitle, L"%s (%d%%)", m_szTitle, (int)(lpShowPicture->GetZoomRatio() * 100.0 + 0.5));
	SetWindowTextW(szTitle);
}

BOOL TPicDlg::EvCreate(LPARAM lParam)
{

	lpShowPicture = new CShowPictureWithZoom(hWnd, *buf, dwSize, lpszFile, iFormat);

	if (!lpShowPicture->Decode())
		return FALSE;

	lpShowPicture->GetWindowTitle(m_szTitle, sizeof(m_szTitle) / sizeof(wchar_t));

	InitFixedShowPositionAndShowWindow();

	return	FALSE;
}


BOOL TPicDlg::EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight)
{
	if(NULL != lpShowPicture)
		lpShowPicture->ChangeClientSize(nWidth, nHeight);

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

		if(NULL != lpShowPicture) {
			lpShowPicture->ZoomToOriginalResolution(pos.x, pos.y);
			FreshWindowTitle();
		}

		break;
	case WM_LBUTTONDBLCLK:
		//导出
		this->SaveFile();
		break;

	}

	return FALSE;
}

BOOL TPicDlg::EvMouseWheel(UINT nFlags, short zDelta, POINTS pos)
{
	if(NULL != lpShowPicture) {

		POINT pointMouse = { pos.x , pos.y };
		ScreenToClient(hWnd, &pointMouse);

		lpShowPicture->ZoomWithMousePoint(nFlags, zDelta, pointMouse.x, pointMouse.y);

		FreshWindowTitle();
	}
	return FALSE;
}


BOOL TPicDlg::EvMouseMove(UINT fwKeys, POINTS pos)
{

	if(isMouseDown) {
		POINT	pointLast = pointMouse;
		GetCursorPos(&pointMouse);

		int		xOffset = pointLast.x - pointMouse.x;
		int		yOffset = pointLast.y - pointMouse.y;

		if(NULL != lpShowPicture) 
			lpShowPicture->MovePicture(xOffset, yOffset);

	}

	return FALSE;
}

BOOL TPicDlg::EvPaint(void)
{
	lpShowPicture->Paint();
	return TRUE;
}
