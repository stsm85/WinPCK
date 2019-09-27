//////////////////////////////////////////////////////////////////////
// tViewDlg.cpp: WinPCK 界面线程部分
// 对话框代码
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2017.12.26
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4018 )
#pragma warning ( disable : 4267 )

#include "tPreviewDlg.h"
#include <stdio.h>
#include "Raw2HexString.h"
#include <tchar.h>
#include "CharsCodeConv.h"

TViewDlg::TViewDlg(LPBYTE *_buf, DWORD _dwSize, const wchar_t *_lpszFile, TWin *_win) : TDlg(IDD_DIALOG_VIEW, _win) ,
	buf(_buf),
	dwSize(_dwSize),
	lpszFile(_lpszFile)
{}

TViewDlg::~TViewDlg()
{
	free(*buf);
	*buf = nullptr;
}

BOOL TViewDlg::EvCreate(LPARAM lParam)
{
	//窗口文字
	wchar_t szTitle[MAX_PATH];

	if(0 != dwSize) {

		SendDlgItemMessage(IDC_RICHEDIT_VIEW, EM_SETTEXTMODE, TM_PLAINTEXT, 0);
		SendDlgItemMessage(IDC_RICHEDIT_VIEW, EM_EXLIMITTEXT, 0, -1);
		//SendDlgItemMessage(IDC_RICHEDIT_VIEW, EM_SETSEL, 0, -1);

		lpszTextShow = (char*)*buf;
		
		switch (textType = TextDataType(lpszTextShow, dwSize)) {

		case TEXT_TYPE_UCS2:
			swprintf_s(szTitle, MAX_PATH, L"文本查看 - %s (Unicode)", lpszFile);
			SetDlgItemTextW(IDC_RICHEDIT_VIEW, (wchar_t *)lpszTextShow);
			break;

		case TEXT_TYPE_UTF8:
			swprintf_s(szTitle, MAX_PATH, L"文本查看 - %s (UTF-8)", lpszFile);
			{
				CU82Ucs cU82U;
				SetDlgItemTextW(IDC_RICHEDIT_VIEW, cU82U.GetString(lpszTextShow));
			}
			break;
		case TEXT_TYPE_ANSI:
			swprintf_s(szTitle, MAX_PATH, L"文本查看 - %s", lpszFile);
			SetDlgItemTextA(IDC_RICHEDIT_VIEW, (char *)lpszTextShow);
			break;
		case TEXT_TYPE_RAW:
			if (VIEW_RAW_MAX_BUFFER < dwSize)
				dwSize = VIEW_RAW_MAX_BUFFER;

			swprintf_s(szTitle, MAX_PATH, L"文本查看 - %s (RAW)", lpszFile);
			ShowRaw((LPBYTE)lpszTextShow, dwSize);
			break;
		}

		free(*buf);
		*buf = nullptr;

	} else {

		swprintf_s(szTitle, MAX_PATH, L"文本查看 - %s", lpszFile);
	}

	SetWindowTextW(szTitle);

	Show();

	SendDlgItemMessage(IDC_RICHEDIT_VIEW, EM_SETSEL, 0, 0);

	return	FALSE;
}

BOOL TViewDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	switch(wID) {
	case IDOK:
		EndDialog(wID);
		return	TRUE;

	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}

#if 0
#define DATATYPE_UTF8_DETECT_RTN {if(0 == *s) return TEXT_TYPE_RAW;else	{isNotUTF8 = TRUE; break;}}	

int TViewDlg::DataType(const char *_s, size_t size)
{
	const u_char *s = (const u_char *)_s;
	BOOL  isNotUTF8 = FALSE;

	while(*s) {
		if(*s <= 0x7f) {
		} else if(*s <= 0xdf) {
			if((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN
		} else if(*s <= 0xef) {
				if((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
				if((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			} else if(*s <= 0xf7) {
				if((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
				if((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
				if((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			} else if(*s <= 0xfb) {
				if((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
				if((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
				if((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
				if((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			} else if(*s <= 0xfd) {
				if((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
				if((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
				if((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
				if((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
				if((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			}
			++s;
	}

	while(*s) {
		++s;
	}

	//int a = (char*)s - _s;

	if(size > ((char*)s - _s))
		return TEXT_TYPE_RAW;
	else
		return isNotUTF8 ? TEXT_TYPE_ANSI : TEXT_TYPE_UTF8;

}
#endif

void TViewDlg::ShowRaw(LPBYTE lpbuf, size_t rawlength)
{

	CRaw2HexString cHexStr(lpbuf, rawlength);

	//RECT viewrect;
	//GetClientRect(hWnd, &viewrect);

	HDC hIC;
	HFONT hFont;
	LOGFONTA lf;
	hIC = CreateICA("DISPLAY", NULL, NULL, NULL); // information context
	lf.lfHeight = 24;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = FW_DONTCARE; //default weight
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	//StringCchCopy(lf.lfFaceName, LF_FACESIZE, TEXT("Fixedsys"));
	memcpy(lf.lfFaceName, "Fixedsys", 9);
	hFont = CreateFontIndirectA(&lf);
	SelectObject(hIC, hFont);
	DeleteDC(hIC);

	//HWND hWndEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", NULL, WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN | WS_VISIBLE | WS_CHILD | ES_READONLY, 0, 0, viewrect.right, viewrect.bottom, hWnd, (HMENU)IDC_RICHEDIT_VIEW, TApp::GetInstance(), NULL);
	//::SendMessageA(hWndEdit, EM_LIMITTEXT, -1, 0);
	//::SendMessage(hWndEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));

	//::SetWindowTextA(hWndEdit, buffer);


	//SendDlgItemMessageA(IDC_RICHEDIT_VIEW, EM_REPLACESEL, 0, (LPARAM)buffer);
	//SetDlgItemTextA(IDC_RICHEDIT_VIEW, buffer);
	::ShowWindow(GetDlgItem(IDC_RICHEDIT_VIEW), SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_EDIT_VIEW), SW_SHOW);
	SendDlgItemMessageA(IDC_EDIT_VIEW, EM_LIMITTEXT, 0, -1);
	SendDlgItemMessageA(IDC_EDIT_VIEW, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
	SetDlgItemTextA(IDC_EDIT_VIEW, cHexStr.GetHexString());

	//DeleteObject(hFont);

	//SendDlgItemMessage(IDC_RICHEDIT_VIEW, WM_SETREDRAW, TRUE, 0);
}
