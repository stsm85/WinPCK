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

#include "miscdlg.h"
#include <stdio.h>

TViewDlg::TViewDlg(char **_buf, DWORD &_dwSize, char *_lpszFile, TWin *_win) : TDlg(IDD_DIALOG_VIEW, _win)
{
	buf = _buf;
	dwSize = _dwSize;
	lpszFile = _lpszFile;

	//上限16MB	
	if(0 != dwSize) {
		if(VIEW_TEXT_MAX_BUFFER < dwSize)
			_dwSize = dwSize = VIEW_TEXT_MAX_BUFFER;
		*buf = (char*)malloc(dwSize + 2);
	} else {
		*buf = NULL;
	}

}

TViewDlg::~TViewDlg()
{
	if(*buf)
		free(*buf);
}

BOOL TViewDlg::EvCreate(LPARAM lParam)
{
	//窗口文字
	char szTitle[MAX_PATH];

	if(0 != dwSize) {

		*(WORD*)(*buf + dwSize) = 0;

		if(0xfeff == *(WORD*)*buf) {
			textType = TEXT_TYPE_UCS2;
			sprintf_s(szTitle, MAX_PATH, "文本查看 - %s (Unicode)", lpszFile);
			lpszTextShow = *buf + 2;

		} else if((0xbbef == *(WORD*)*buf) && (0xbf == *(BYTE*)(*buf + 3))) {
			textType = TEXT_TYPE_UTF8;
			sprintf_s(szTitle, MAX_PATH, "文本查看 - %s (UTF-8)", lpszFile);
			lpszTextShow = *buf + 3;

		} else {
			textType = DataType(*buf, dwSize);

			switch(textType) {
			case TEXT_TYPE_UTF8:

				//textType = TEXT_TYPE_UTF8;
				sprintf_s(szTitle, MAX_PATH, "文本查看 - %s (UTF-8)", lpszFile);
				break;

			case TEXT_TYPE_ANSI:

				//textType = TEXT_TYPE_ANSI;
				sprintf_s(szTitle, MAX_PATH, "文本查看 - %s", lpszFile);
				break;

			case TEXT_TYPE_RAW:

				if(VIEW_RAW_MAX_BUFFER < dwSize)
					dwSize = VIEW_RAW_MAX_BUFFER;

				sprintf_s(szTitle, MAX_PATH, "文本查看 - %s (RAW)", lpszFile);

				break;


			}

			lpszTextShow = *buf;
		}


		SendDlgItemMessage(IDC_RICHEDIT_VIEW, EM_SETTEXTMODE, TM_PLAINTEXT, 0);
		SendDlgItemMessage(IDC_RICHEDIT_VIEW, EM_EXLIMITTEXT, 0, -1);
		//SendDlgItemMessage(IDC_RICHEDIT_VIEW, EM_SETSEL, 0, -1);

		switch(textType) {
		case TEXT_TYPE_UCS2:
			//SendDlgItemMessageW(IDC_RICHEDIT_VIEW, EM_REPLACESEL, 0, (LPARAM)lpszTextShow);
			SetDlgItemTextW(IDC_RICHEDIT_VIEW, (wchar_t *)lpszTextShow);
			//SetDlgItemTextW(IDC_EDIT_VIEW, (wchar_t *)lpszTextShow);
			break;

		case TEXT_TYPE_ANSI:
			SetDlgItemTextA(IDC_RICHEDIT_VIEW, (char *)lpszTextShow);

			break;

		case TEXT_TYPE_UTF8:
			SetDlgItemTextW(IDC_RICHEDIT_VIEW, U8toW(lpszTextShow));
			U8toW("");
			break;

		case TEXT_TYPE_RAW:
			ShowRaw((LPBYTE)lpszTextShow, dwSize);
			break;

		}

		free(*buf);
		*buf = NULL;

	} else {

		sprintf_s(szTitle, MAX_PATH, "文本查看 - %s", lpszFile);
	}

	SetWindowTextA(szTitle);

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

#define ADDRESS_FMTSTR		"%06xh: "
#define ADDRESS_STRLEN		9
//strlen("+0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +a +b +c +d +e +f ")
#define	DATAHEX_STRLEN		19
#define	BYTEHEX_STRLEN		3
#define	BYTES_PER_LINE		16
#define	LINE_SPACECRLF_LEN	3
#define LINE_STRLEN			(ADDRESS_STRLEN + BYTEHEX_STRLEN * BYTES_PER_LINE + DATAHEX_STRLEN)
//#define LINE_STRLEN			76

void TViewDlg::ShowRaw(LPBYTE lpbuf, size_t rawlength)
{
	const char	*lpszByte2Raw[0x100] = {
		"00 ", "01 ", "02 ", "03 ", "04 ", "05 ", "06 ", "07 ", "08 ", "09 ", "0a ", "0b ", "0c ", "0d ", "0e ", "0f ",
		"10 ", "11 ", "12 ", "13 ", "14 ", "15 ", "16 ", "17 ", "18 ", "19 ", "1a ", "1b ", "1c ", "1d ", "1e ", "1f ",
		"20 ", "21 ", "22 ", "23 ", "24 ", "25 ", "26 ", "27 ", "28 ", "29 ", "2a ", "2b ", "2c ", "2d ", "2e ", "2f ",
		"30 ", "31 ", "32 ", "33 ", "34 ", "35 ", "36 ", "37 ", "38 ", "39 ", "3a ", "3b ", "3c ", "3d ", "3e ", "3f ",
		"40 ", "41 ", "42 ", "43 ", "44 ", "45 ", "46 ", "47 ", "48 ", "49 ", "4a ", "4b ", "4c ", "4d ", "4e ", "4f ",
		"50 ", "51 ", "52 ", "53 ", "54 ", "55 ", "56 ", "57 ", "58 ", "59 ", "5a ", "5b ", "5c ", "5d ", "5e ", "5f ",
		"60 ", "61 ", "62 ", "63 ", "64 ", "65 ", "66 ", "67 ", "68 ", "69 ", "6a ", "6b ", "6c ", "6d ", "6e ", "6f ",
		"70 ", "71 ", "72 ", "73 ", "74 ", "75 ", "76 ", "77 ", "78 ", "79 ", "7a ", "7b ", "7c ", "7d ", "7e ", "7f ",
		"80 ", "81 ", "82 ", "83 ", "84 ", "85 ", "86 ", "87 ", "88 ", "89 ", "8a ", "8b ", "8c ", "8d ", "8e ", "8f ",
		"90 ", "91 ", "92 ", "93 ", "94 ", "95 ", "96 ", "97 ", "98 ", "99 ", "9a ", "9b ", "9c ", "9d ", "9e ", "9f ",
		"a0 ", "a1 ", "a2 ", "a3 ", "a4 ", "a5 ", "a6 ", "a7 ", "a8 ", "a9 ", "aa ", "ab ", "ac ", "ad ", "ae ", "af ",
		"b0 ", "b1 ", "b2 ", "b3 ", "b4 ", "b5 ", "b6 ", "b7 ", "b8 ", "b9 ", "ba ", "bb ", "bc ", "bd ", "be ", "bf ",
		"c0 ", "c1 ", "c2 ", "c3 ", "c4 ", "c5 ", "c6 ", "c7 ", "c8 ", "c9 ", "ca ", "cb ", "cc ", "cd ", "ce ", "cf ",
		"d0 ", "d1 ", "d2 ", "d3 ", "d4 ", "d5 ", "d6 ", "d7 ", "d8 ", "d9 ", "da ", "db ", "dc ", "dd ", "de ", "df ",
		"e0 ", "e1 ", "e2 ", "e3 ", "e4 ", "e5 ", "e6 ", "e7 ", "e8 ", "e9 ", "ea ", "eb ", "ec ", "ed ", "ee ", "ef ",
		"f0 ", "f1 ", "f2 ", "f3 ", "f4 ", "f5 ", "f6 ", "f7 ", "f8 ", "f9 ", "fa ", "fb ", "fc ", "fd ", "fe ", "ff "
	};

	//char rawshow[140];
	char *buffer;

	DWORD dwDst = ((rawlength >> 4) + 2) * LINE_STRLEN;
	if(NULL == (buffer = (char*)malloc(dwDst))) {
		return;
	}

	int i;
	char *lpbuffer = buffer;
	BYTE raw16[32];
	memset(raw16, 0, 32);
	raw16[16] = ' ';
	raw16[17] = '\r';
	raw16[18] = '\n';

	int nFullLine = (rawlength & 0xfffffff0) + 0x10;

	memcpy(lpbuffer, "Address: +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +a +b +c +d +e +f 0123456789abcdef \r\n", LINE_STRLEN);
	lpbuffer += LINE_STRLEN;

	for(i = BYTES_PER_LINE;i < nFullLine;i += BYTES_PER_LINE) {
		memcpy(raw16, &lpbuf[i - BYTES_PER_LINE], BYTES_PER_LINE);

		sprintf(lpbuffer, ADDRESS_FMTSTR, i - BYTES_PER_LINE);
		lpbuffer += ADDRESS_STRLEN;

		for(int j = 0;j < BYTES_PER_LINE;++j) {

			memcpy(lpbuffer, lpszByte2Raw[raw16[j]], BYTEHEX_STRLEN);
			lpbuffer += BYTEHEX_STRLEN;

		}

		for(int j = 0;j < BYTES_PER_LINE;j++) {
			if(0x20 > raw16[j])
				raw16[j] = '.';
		}
		memcpy(lpbuffer, raw16, DATAHEX_STRLEN);
		lpbuffer += DATAHEX_STRLEN;

	}

	int nLeftBytes = rawlength & 0xf;

	if(0 != nLeftBytes) {
		memcpy(raw16, &lpbuf[i - BYTES_PER_LINE], nLeftBytes);

		sprintf(lpbuffer, ADDRESS_FMTSTR, i - BYTES_PER_LINE);
		lpbuffer += ADDRESS_STRLEN;

		for(int j = 0;j < BYTES_PER_LINE;++j) {
			if(j < nLeftBytes)
				memcpy(lpbuffer, lpszByte2Raw[raw16[j]], BYTEHEX_STRLEN);
			else
				memcpy(lpbuffer, "   ", BYTEHEX_STRLEN);
			lpbuffer += BYTEHEX_STRLEN;
		}

		for(int j = 0;j < nLeftBytes;j++) {
			if(0x20 > raw16[j])
				raw16[j] = '.';
		}

		memcpy(lpbuffer, raw16, nLeftBytes);
		lpbuffer += nLeftBytes;
	}

	*lpbuffer = 0;


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
	SetDlgItemTextA(IDC_EDIT_VIEW, buffer);

	//DeleteObject(hFont);

	free(buffer);
	//SendDlgItemMessage(IDC_RICHEDIT_VIEW, WM_SETREDRAW, TRUE, 0);
}

//
//BOOL TViewDlg::IsTextUTF8(char *inputStream, size_t size)
//{
//    int encodingBytesCount = 0;
//    BOOL allTextsAreASCIIChars = true;
// 
//    for (int i = 0; i < size; ++i)
//    {
//        byte current = inputStream[i];
// 
//        if ((current & 0x80) == 0x80)
//        {                   
//            allTextsAreASCIIChars = false;
//        }
//        // First byte
//        if(encodingBytesCount == 0)
//        {
//            if((current & 0x80) == 0)
//            {
//                // ASCII chars, from 0x00-0x7F
//                continue;
//            }
// 
//            if ((current & 0xC0) == 0xC0)
//            {
//                encodingBytesCount = 1;
//                current <<= 2;
// 
//                // More than two bytes used to encoding a unicode char.
//                // Calculate the real length.
//                while ((current & 0x80) == 0x80)
//                {
//                    current <<= 1;
//                    ++encodingBytesCount;
//                }
//            }                   
//            else
//            {
//                // Invalid bits structure for UTF8 encoding rule.
//                return FALSE;
//            }
//        }               
//        else
//        {
//            // Following bytes, must start with 10.
//            if ((current & 0xC0) == 0x80)
//            {                       
//                --encodingBytesCount;
//            }
//            else
//            {
//                // Invalid bits structure for UTF8 encoding rule.
//                return FALSE;
//            }
//        }
//    }
// 
//    if (encodingBytesCount != 0)
//    {
//        // Invalid bits structure for UTF8 encoding rule.
//        // Wrong following bytes count.
//        return FALSE;
//    }
// 
//    // Although UTF8 supports encoding for ASCII chars, we regard as a input stream, whose contents are all ASCII as default encoding.
//    return !allTextsAreASCIIChars;
//}
