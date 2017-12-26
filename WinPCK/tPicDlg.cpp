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
#include "miscdlg.h"
#include <stdio.h>

TPicDlg::TPicDlg(char **_buf, UINT32 _dwSize, int _iFormat, char *_lpszFile, TWin *_win) : TDlg(IDD_DIALOG_PIC, _win)
{
	s = NULL;
	lpoGraph = NULL;
	lpmyImage = NULL;
	g_pGdiPlusToken = NULL;
	*szDdsFormat = 0;

	x = 0, y = 0;
	isMouseDown = FALSE;

	buf = _buf;
	dwSize = _dwSize;
	lpszFile = _lpszFile;

	//hG = NULL;
	*buf = NULL;
	cleanimage = NULL;

	//if(isDds = _isDds)
	if(FMT_RAW != (iFormat = _iFormat))	//dds,tga
	{
		*buf = (char*)malloc(dwSize);
	} else {							//bmp,jpg,png..
		*buf = (char*)(hGlobal = GlobalAlloc(GMEM_FIXED, dwSize));
	}

	GdiplusStartup(&g_pGdiPlusToken, &g_oGdiPlusStartupInput, NULL);

	//hbEditBack	=	CreateSolidBrush(RGB(233,248,247));


}

TPicDlg::~TPicDlg()
{
	if(NULL != lpoGraph)
		delete lpoGraph;

	if(NULL != lpmyImage)
		delete lpmyImage;

	if(NULL != s)
		s->Release();

	if(FMT_RAW != iFormat) {
		if(NULL != *buf) {
			free(*buf);
		}
	} else {

		if(NULL != hGlobal)
			GlobalFree(hGlobal);
	}

	if(NULL != cleanimage) {
		free(cleanimage);
	}

	//DeleteObject( hbEditBack );
	//SelectObject(MemDC, pOldBit);
	DeleteObject(MemBitmap);
	DeleteDC(MemDC);

	GdiplusShutdown(g_pGdiPlusToken);
}

//BOOL TPicDlg::EvClose()
//{
//
//	return FALSE;
//}

//BOOL TPicDlg::EvDrawItem(UINT ctlID, DRAWITEMSTRUCT *lpDis)
//{
//
//	if(NULL != lpoGraph)
//	{
//		EvPaint();
//	}
//	return FALSE;
//}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0;
	UINT size = 0;

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0) {
		return -1;
	}
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL) {
		return -1;
	}

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j) {
		if(wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}

	free(pImageCodecInfo);
	return -1;
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

BOOL TPicDlg::SaveFile(WCHAR * lpszFileName)
{
	OPENFILENAMEW ofn;
	//TCHAR szStrPrintf[260];

	ZeroMemory(&ofn, sizeof(OPENFILENAMEW));
	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = TEXT_SAVE_FILTER;
	//ofn.lpstrFilter       = AtoW(TEXT_SAVE_FILTER);
	ofn.lpstrFile = lpszFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING;

	if(!GetSaveFileNameW(&ofn)) {
		return FALSE;
	}
	return TRUE;

}

BOOL TPicDlg::EvCreate(LPARAM lParam)
{
	//HGLOBAL		hG;
	//调整static大小
	RECT	rectWin;
	GetClientRect(hWnd, &rectWin);
	//::SetWindowPos(GetDlgItem(IDC_STATIC_PIC), NULL, 0, 0, rectWin.right, rectWin.bottom, SWP_NOZORDER | SWP_NOMOVE);

	//读dds header

	switch(iFormat) {
	case FMT_DDS:

		if(!CreateBmpBufferByDds()) {
			MessageBoxA(TEXT_SHOWPIC_ERROR, NULL, MB_OK | MB_ICONERROR);
			assert(FALSE);
			EndDialog(IDCANCEL);
			return FALSE;
		}

		free(*buf);
		*buf = NULL;

		lpmyImage = new Bitmap(picWidth, picHeight, stride, decodedDIBFormat, (BYTE*)cleanimage);

		break;

	case FMT_TGA:

		if(!CreateBmpBufferByTga()) {
			MessageBoxA(TEXT_SHOWPIC_ERROR, NULL, MB_OK | MB_ICONERROR);
			assert(FALSE);
			EndDialog(IDCANCEL);
			return FALSE;
		}

		free(*buf);
		*buf = NULL;

		lpmyImage = new Bitmap(picWidth, picHeight, stride, decodedDIBFormat, (BYTE*)cleanimage);
		break;

	case FMT_RAW:	//bmp,jpg,png..

		CreateStreamOnHGlobal(hGlobal, false, &s);
		if(!s) {
			MessageBoxA(TEXT_SHOWPIC_ERROR, NULL, MB_OK | MB_ICONERROR);
			assert(FALSE);
			EndDialog(IDCANCEL);
			return FALSE;
		}

		//show picture
		lpmyImage = new Image(s);

		//看看图片大小和显示窗口大小
		picWidth = lpmyImage->GetWidth();
		picHeight = lpmyImage->GetHeight();

		break;

	default:

		MessageBoxA(TEXT_NOT_DEFINE, NULL, MB_OK | MB_ICONERROR);
		EndDialog(IDCANCEL);
		return FALSE;
		break;
	}

	DrawBlockOnDlg();

	lpoGraph = new Graphics(MemDC);

	lpoGraph->DrawImage(lpmyImage, x, y, picWidth, picHeight);

	ShowScrollBar(hWnd, SB_VERT, FALSE);
	ShowScrollBar(hWnd, SB_HORZ, FALSE);

	GetClientRect(hWnd, &rectWin);


	if(rectWin.right < picWidth || rectWin.bottom < picHeight) {

		if(rectWin.right < picWidth) {
			ShowScrollBar(hWnd, SB_HORZ, TRUE);
			SetScrollRange(hWnd, SB_HORZ, 0, picWidth - rectWin.right, TRUE);
		} else {

			ShowScrollBar(hWnd, SB_HORZ, FALSE);
		}

		GetClientRect(hWnd, &rectWin);

		if(rectWin.bottom < picHeight) {
			ShowScrollBar(hWnd, SB_VERT, TRUE);
			SetScrollRange(hWnd, SB_VERT, 0, picHeight - rectWin.bottom, TRUE);

			GetClientRect(hWnd, &rectWin);


			if(rectWin.right < picWidth) {
				ShowScrollBar(hWnd, SB_HORZ, TRUE);
				SetScrollRange(hWnd, SB_HORZ, 0, picWidth - rectWin.right, TRUE);
			} else {
				ShowScrollBar(hWnd, SB_HORZ, FALSE);
			}

		} else {
			ShowScrollBar(hWnd, SB_VERT, FALSE);
		}

	} else {

		::SetWindowPos(hWnd, NULL, 0, 0, picWidth + 20, picHeight + 50, SWP_NOZORDER | SWP_NOMOVE);
	}

	if(NULL != lpoGraph)
		delete lpoGraph;
	lpoGraph = NULL;

	//窗口文字
	if(FMT_RAW != iFormat)
		sprintf_s(szTitle, MAX_PATH, "图片查看 - %s, %dx%d, %s", lpszFile, picWidth, picHeight, szDdsFormat);
	else {
		sprintf_s(szTitle, MAX_PATH, "图片查看 - %s, %dx%d", lpszFile, picWidth, picHeight);

		if(NULL != lpmyImage)
			delete lpmyImage;
		lpmyImage = NULL;

		if(NULL != s)
			s->Release();
		s = NULL;

		if(NULL != hGlobal)
			GlobalFree(hGlobal);
		hGlobal = NULL;

	}

	SetWindowTextA(szTitle);

	//显示窗口
	Show();

	return	FALSE;
}

BOOL TPicDlg::EventScroll(UINT uMsg, int nCode, int nPos, HWND scrollBar)
{
	int nCurPos;
	//RECT	rect;

	switch(uMsg) {
	case WM_HSCROLL:

		nCurPos = GetScrollPos(hWnd, SB_HORZ);

		switch(nCode) {
		case SB_LINEUP:
			if(nCurPos > 0) {
				nCurPos -= 1;
				//ScrollWindow(hWnd, 1, 0, 0, 0);
			}
			break;
		case SB_LINEDOWN:
			if(nCurPos < 20) {
				nCurPos += 1;
				//ScrollWindow(hWnd, -1, 0, 0, 0);
			}
			break;
		case SB_PAGEUP:
			nCurPos -= 4;
			//ScrollWindow(hWnd, 4, 0, 0, 0);
			break;
		case SB_PAGEDOWN:
			nCurPos += 4;
			//ScrollWindow(hWnd, -4, 0, 0, 0);
			break;
		case SB_THUMBTRACK:
			//ScrollWindow(hWnd, nCurPos - nPos, 0, 0, 0);
			nCurPos = nPos;
			break;
		default:
			break;
		}
		SetScrollPos(hWnd, SB_HORZ, nCurPos, TRUE);

		//DebugA("nCurPos = %d\r\n", nCurPos);

		x = 0 - nCurPos;

		EvPaint();
		break;

	case WM_VSCROLL:

		nCurPos = GetScrollPos(hWnd, SB_VERT);

		switch(nCode) {
		case SB_LINEUP:
			if(nCurPos > 0) {
				nCurPos -= 1;
				//ScrollWindow(hWnd, 0, 1, 0, 0);
			}
			break;
		case SB_LINEDOWN:
			if(nCurPos < 20) {
				nCurPos += 1;
				//ScrollWindow(hWnd, 0,-1, 0, 0);
			}
			break;
		case SB_PAGEUP:
			nCurPos -= 4;
			//ScrollWindow(hWnd, 0, 4, 0, 0);
			break;
		case SB_PAGEDOWN:
			nCurPos += 4;
			//ScrollWindow(hWnd, 0, -4, 0, 0);
			break;
		case SB_THUMBTRACK:
			//ScrollWindow(hWnd, 0, nCurPos - nPos, 0, 0);
			nCurPos = nPos;
			break;
		default:
			break;
		}
		SetScrollPos(hWnd, SB_VERT, nCurPos, TRUE);

		y = 0 - nCurPos;

		EvPaint();

		break;
	}

	return	FALSE;
}

BOOL TPicDlg::EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight)
{
	//::SetWindowPos(GetDlgItem(IDC_STATIC_PIC), NULL, 0, 0, nWidth, nHeight, SWP_NOZORDER | SWP_NOMOVE);

	//if(NULL != lpoGraph)
	//{
	//delete lpoGraph;
	//lpoGraph = new Graphics(GetDlgItem(IDC_STATIC_PIC));

	int nCurPos = 0;

	if(nWidth < picWidth || nHeight < picHeight) {

		if(nWidth < picWidth) {
			ShowScrollBar(hWnd, SB_HORZ, TRUE);

			nCurPos = GetScrollPos(hWnd, SB_HORZ);

			if(nCurPos > picWidth - nWidth) {
				nCurPos = picWidth - nWidth;
				x = 0 - nCurPos;
			}

			SetScrollRange(hWnd, SB_HORZ, 0, picWidth - nWidth, TRUE);
		} else {
			x = 0;
			ShowScrollBar(hWnd, SB_HORZ, FALSE);
		}

		if(nHeight < picHeight) {
			ShowScrollBar(hWnd, SB_VERT, TRUE);

			nCurPos = GetScrollPos(hWnd, SB_VERT);

			if(nCurPos > picHeight - nHeight) {
				nCurPos = picHeight - nHeight;
				y = 0 - nCurPos;
			}

			SetScrollRange(hWnd, SB_VERT, 0, picHeight - nHeight, TRUE);

		} else {
			y = 0;
			ShowScrollBar(hWnd, SB_VERT, FALSE);
		}

	} else {
		x = y = 0;
		ShowScrollBar(hWnd, SB_VERT, FALSE);
		ShowScrollBar(hWnd, SB_HORZ, FALSE);

	}

	//EvPaint();
	//}

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

	case WM_LBUTTONDBLCLK:
		//导出
		if(0 != iFormat) {
			WCHAR	szFilename[MAX_PATH];
			::GetCurrentDirectoryW(MAX_PATH, szFilename);

			WCHAR *lpTail = szFilename + wcslen(szFilename) - 1;
			if('\\' == *lpTail) {
				*lpTail = 0;
			}

			wcscat_s(szFilename, MAX_PATH, L"\\");
			wcscat_s(szFilename, MAX_PATH, AtoW(lpszFile));

			//*(DWORD*)strrchr(szFilename, '.') = *(DWORD*)".png";//0x706D622E;	//.bmp
			memcpy(wcsrchr(szFilename, L'.'), L".png\0\0", 12);

			if(SaveFile(szFilename)) {
				//PrepareToSavePng(szFilename);
				CLSID pngClsid;
				GetEncoderClsid(L"image/png", &pngClsid);
				lpmyImage->Save(szFilename, &pngClsid, NULL);
			}
		}
		break;


	}

	return FALSE;
}


BOOL TPicDlg::EvMouseMove(UINT fwKeys, POINTS pos)
{

	/*
	SetCapture();
	SetCursor(m_hCursorAllow);
	m_isSearchWindow = TRUE;
	*/

	if(isMouseDown) {
		POINT	pointLast = pointMouse;
		GetCursorPos(&pointMouse);

		int		xOffset = pointLast.x - pointMouse.x;
		int		yOffset = pointLast.y - pointMouse.y;

		RECT	rectWin;
		GetClientRect(hWnd, &rectWin);


		int nCurPos, nCurPosMax;

		if(rectWin.right < picWidth) {
			GetScrollRange(hWnd, SB_HORZ, &nCurPos, &nCurPosMax);

			nCurPos = GetScrollPos(hWnd, SB_HORZ);
			nCurPos += xOffset;

			if(0 > nCurPos)nCurPos = 0;
			if(nCurPosMax < nCurPos)nCurPos = nCurPosMax;

			SetScrollPos(hWnd, SB_HORZ, nCurPos, TRUE);

			x = 0 - nCurPos;

		}

		if(rectWin.bottom < picHeight) {
			GetScrollRange(hWnd, SB_VERT, &nCurPos, &nCurPosMax);

			nCurPos = GetScrollPos(hWnd, SB_VERT);
			nCurPos += yOffset;

			if(0 > nCurPos)nCurPos = 0;
			if(nCurPosMax < nCurPos)nCurPos = nCurPosMax;

			SetScrollPos(hWnd, SB_VERT, nCurPos, TRUE);

			y = 0 - nCurPos;
		}

		EvPaint();
	}

	return FALSE;
}
//
//BOOL TPicDlg::EventCtlColor(UINT uMsg, HDC hDcCtl, HWND hWndCtl, HBRUSH *result)
//{
//	//switch(uMsg)
//	//{
//
//	//case WM_CTLCOLORSTATIC:
// //       SetBkMode(hDcCtl, TRANSPARENT); 
// //       //pDC->SetTextColor(RGB(125,0,0)); 
// //       //return (LRESULT)hbEditBack; 
//	//	*result = (HBRUSH)GetStockObject(HOLLOW_BRUSH); 
//	//	return TRUE;
//	//	break;
//
//	//}
//	EvPaint();
//	return FALSE;
//}

void TPicDlg::DrawBlockOnDlg()
{
	//HDC pDC = ::GetDC(GetDlgItem(IDC_STATIC_PIC));
	HDC pDC = ::GetDC(hWnd);
	//GetClientRect(hWnd, &rect);

	//CDC MemDC; //首先定义一个显示设备对象
	HDC MemDCTemp;
	//CBitmap MemBitmap;//定义一个位图对象
	HBITMAP MemBitmapTemp;
	//随后建立与屏幕显示兼容的内存显示设备  
	MemDC = CreateCompatibleDC(NULL);
	MemDCTemp = CreateCompatibleDC(NULL);
	//这时还不能绘图，因为没有地方画 ^_^ 
	//下面建立一个与屏幕显示兼容的位图，至于位图的大小嘛，可以用窗口的大小 
	MemBitmap = CreateCompatibleBitmap(pDC, picWidth, picHeight);
	MemBitmapTemp = CreateCompatibleBitmap(pDC, 24, 8);

	ReleaseDC(hWnd, pDC);

	//将位图选入到内存显示设备中 
	//只有选入了位图的内存显示设备才有地方绘图，画到指定的位图上 
	HBITMAP pOldBit = (HBITMAP)SelectObject(MemDC, MemBitmap);
	HBITMAP pOldBit1 = (HBITMAP)SelectObject(MemDCTemp, MemBitmapTemp);

	//先用背景色将位图清除干净，这里我用的是白色作为背景 
	//你也可以用自己应该用的颜色 
	SetBkColor(MemDCTemp, RGB(204, 204, 204));
	RECT thisrect = { 0, 0, 24, 8 };
	ExtTextOut(MemDCTemp, 0, 0, ETO_OPAQUE, &thisrect, NULL, 0, NULL);

	thisrect.left += 8;
	SetBkColor(MemDCTemp, RGB(255, 255, 255));
	ExtTextOut(MemDCTemp, 0, 0, ETO_OPAQUE, &thisrect, NULL, 0, NULL);

	thisrect.left += 8;
	SetBkColor(MemDCTemp, RGB(204, 204, 204));
	ExtTextOut(MemDCTemp, 0, 0, ETO_OPAQUE, &thisrect, NULL, 0, NULL);

	for(int j = 0;j < picHeight;j += 8) {
		for(int i = 0;i < picWidth;i += 16) {
			if(j & 8)
				BitBlt(MemDC, i, j, i + 16, j + 8, MemDCTemp, 0, 0, SRCCOPY);
			else
				BitBlt(MemDC, i, j, i + 16, j + 8, MemDCTemp, 8, 0, SRCCOPY);
		}
	}
	//MemBitmap.DeleteObject(); 
	SelectObject(MemDCTemp, pOldBit1);
	//a = DeleteObject(MemBitmap);
	DeleteObject(MemBitmapTemp);
	//MemDC.DeleteDC();
	DeleteDC(MemDCTemp);

}

BOOL TPicDlg::EvPaint(void)
{
	RECT rectDlg;

	HDC pDC = ::GetDC(hWnd);
	GetClientRect(hWnd, &rectDlg);

	PAINTSTRUCT ps;
	HDC pDC1 = BeginPaint(hWnd, &ps);

	BitBlt(pDC, x, y, picWidth, picHeight, MemDC, 0, 0, SRCCOPY);
	//BitBlt(pDC, 0, 0, picWidth, picHeight, MemDC, -x, -y, SRCCOPY);

	ReleaseDC(hWnd, pDC);
	EndPaint(hWnd, &ps);

	return TRUE;
}
