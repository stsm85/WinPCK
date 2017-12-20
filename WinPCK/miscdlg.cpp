//////////////////////////////////////////////////////////////////////
// miscdlg.cpp: WinPCK 界面线程部分
// 对话框代码
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////
#pragma warning ( disable : 4244 )
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4018 )
#pragma warning ( disable : 4005 )


#include "globals.h"
#include "miscdlg.h"
//#include "miscpicdlg.h"

#include <stdio.h>
#include <shlwapi.h>

#include "PckHeader.h"
//#include "Pck.h"
#include "tapi32u8.h"


/*
	日志信息对话框
*/
BOOL TLogDlg::EvCreate(LPARAM lParam)
{
	RECT rcList;

	hWndList = GetDlgItem(IDC_LIST_LOG);

	GetClientRect(hWndList, &rcList);

	ListView_SetExtendedListViewStyle(hWndList, LVS_EX_FULLROWSELECT);

	HIMAGELIST hSmall;   // image list for other views 
	LVCOLUMN lvcolumn;
	HICON hiconItem;

	lvcolumn.mask = LVCF_FMT | LVCF_WIDTH /*| LVCF_TEXT*/ | LVCF_SUBITEM;
	lvcolumn.fmt = LVCFMT_LEFT;
	lvcolumn.cx = rcList.right - 24;
	//lvcolumn.pszText = TEXT("test");
	lvcolumn.iSubItem = 0;
		//lvcolumn.iOrder = 0;
	ListView_InsertColumn(hWndList, lvcolumn.iSubItem, & lvcolumn);

	hSmall = ImageList_Create(16, 16, ILC_COLOR32, 4, 1);



	hiconItem = LoadIcon(TApp::GetInstance(), MAKEINTRESOURCE(IDI_ICON_LOGI)); 
	ImageList_AddIcon(hSmall, hiconItem);	//LOGN
	ImageList_AddIcon(hSmall, hiconItem);	//LOGI
	DestroyIcon(hiconItem); 

	hiconItem = LoadIcon(TApp::GetInstance(), MAKEINTRESOURCE(IDI_ICON_LOGW)); 
	ImageList_AddIcon(hSmall, hiconItem); 
	DestroyIcon(hiconItem); 

	hiconItem = LoadIcon(TApp::GetInstance(), MAKEINTRESOURCE(IDI_ICON_LOGE)); 
	ImageList_AddIcon(hSmall, hiconItem); 
	DestroyIcon(hiconItem); 

	hiconItem = LoadIcon(TApp::GetInstance(), MAKEINTRESOURCE(IDI_ICON_LOGD)); 
	ImageList_AddIcon(hSmall, hiconItem); 
	DestroyIcon(hiconItem); 

	ListView_SetImageList(hWndList, hSmall, LVSIL_SMALL); 

	//当前exe目录
	GetModuleFileNameA(NULL, szExePath, MAX_PATH);
	char *lpexeTitle = strrchr(szExePath, '\\') + 1;
	*lpexeTitle = 0;

	//Show();

	return	TRUE;
}

BOOL TLogDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	//char szPrintf[300];
	//char *lpszFile;

	switch (wID)
	{
	case IDOK:
		return	TRUE;

	case IDCANCEL:
		return	TRUE;

	case ID_MENU_LOG_CLEAR:

		ListView_DeleteAllItems(hWndList);

		break;

	case ID_MENU_LOG_SAVE_SEL:
		{
			LVITEMA item = {0};

			item.mask = LVIF_STATE;
			item.stateMask = LVIS_SELECTED;

			FILE *fplog = fopen(pszLogFileName(), "w+");
			if(!fplog){

				::MessageBoxA(hWnd, "保存日志文件失败", "error", MB_OK);
				return TRUE;
			}

			int	nItemCount = ::SendMessageA(hWndList, LVM_GETITEMCOUNT, 0, 0);

			for(item.iItem=0;item.iItem<nItemCount;++item.iItem)
			{
				//ListView_GetItem(hWndList, &item);
				::SendMessageA(hWndList, LVM_GETITEMA, 0, (LPARAM)(LV_ITEMA*)&item);
				if( item.state & LVIS_SELECTED )
				{
					fprintf(fplog, "%s\r\n", pszTargetListLog(item.iItem));
				}
			}

			fclose(fplog);

		}
		break;

	case ID_MENU_LOG_SAVE_ALL:
		{
			LVITEMA item = {0};

			FILE *fplog = fopen(pszLogFileName(), "w+");
			if(!fplog){

				::MessageBoxA(hWnd, "保存日志文件失败", "error", MB_OK);
				return TRUE;
			}

			int	nItemCount = ::SendMessageA(hWndList, LVM_GETITEMCOUNT, 0, 0);

			for(int i=0;i<nItemCount;++i)
			{
				fprintf(fplog, "%s\r\n", pszTargetListLog(i));
			}

			fclose(fplog);

		}
		break;

	case ID_MENU_LOG_COPY:
		{
			char *lpszLogText = pszTargetListLog(m_iCurrentHotItem);
			size_t nLen = strlen(lpszLogText) + 1;

			//拷入剪切板
			if(OpenClipboard(hWnd))
			{ 
				HGLOBAL clipbuffer;
				char *buffer;
				EmptyClipboard();
				if(NULL != (clipbuffer = GlobalAlloc(GMEM_DDESHARE, nLen))){
					buffer = (char *)GlobalLock(clipbuffer);
					//StringCchCopyA(buffer, nLen, lpszLogText);
					memcpy(buffer, lpszLogText, nLen);
					GlobalUnlock(clipbuffer);

					SetClipboardData(CF_TEXT,clipbuffer);

					CloseClipboard();
					GlobalFree(clipbuffer);

				}else{
					CloseClipboard();

				}
			}
		}
		break;
	}
	return	FALSE;
}

BOOL TLogDlg::EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight)
{
	::SetWindowPos(hWndList, NULL, 0, 0, nWidth, nHeight, SWP_NOZORDER | SWP_NOMOVE); 
	ListView_SetColumnWidth(hWndList, 0, nWidth - 24);

	return TRUE;
}

BOOL TLogDlg::EvNotify(UINT ctlID, NMHDR *pNmHdr)
{
	switch(ctlID)
	{
	case IDC_LIST_LOG:

		switch(pNmHdr->code)
		{
		case NM_RCLICK:

			HMENU hMenuRClick = GetSubMenu(LoadMenu(TApp::GetInstance(), MAKEINTRESOURCE(IDR_MENU_LOG)), 0);

			m_iCurrentHotItem = ListView_GetHotItem(pNmHdr->hwndFrom);

			::EnableMenuItem(hMenuRClick, ID_MENU_LOG_SAVE_SEL, 0 != ListView_GetSelectedCount(pNmHdr->hwndFrom) ? MF_ENABLED : MF_GRAYED);
			::EnableMenuItem(hMenuRClick, ID_MENU_LOG_SAVE_ALL, 0 != ListView_GetItemCount(pNmHdr->hwndFrom) ? MF_ENABLED : MF_GRAYED);
			::EnableMenuItem(hMenuRClick, ID_MENU_LOG_CLEAR, 0 != ListView_GetItemCount(pNmHdr->hwndFrom) ? MF_ENABLED : MF_GRAYED);
			::EnableMenuItem(hMenuRClick, ID_MENU_LOG_COPY, -1 != m_iCurrentHotItem ? MF_ENABLED : MF_GRAYED);


			TrackPopupMenu(hMenuRClick, TPM_LEFTALIGN, LOWORD(GetMessagePos()), HIWORD(GetMessagePos()), 0, hWnd, NULL);

			break;
		}
		break;
	}

	return FALSE;
}

BOOL TLogDlg::EvClose()
{
	ShowWindow(SW_HIDE);
	return TRUE;
}

HWND TLogDlg::GetListWnd()
{
	return hWndList;
}

char*  TLogDlg::pszLogFileName()
{
	static char logfile[MAX_PATH];
	SYSTEMTIME systime;

	//创建文件exportlog_2015-05-04_13_22_33.log
	GetLocalTime (&systime);
	sprintf_s(logfile, "%sexportlog_%04d-%02d-%02d_%02d_%02d_%02d.log", szExePath, \
										systime.wYear, systime.wMonth, systime.wDay, \
										systime.wHour, systime.wMinute, systime.wSecond);
	return logfile;

}

char*  TLogDlg::pszTargetListLog(int iItem)
{
	static char szText[8192];

	LVITEMA item = {0};

	*(szText + 1) = ' ';

	item.iItem = iItem;
	item.mask = LVIF_IMAGE | LVIF_TEXT;
	item.pszText = szText + 2;
	item.cchTextMax = 8190;
	::SendMessageA(hWndList, LVM_GETITEMA, 0, (LPARAM)(LV_ITEMA*)&item);

	switch(item.iImage)
	{
	case LOG_IMAGE_INFO:

		*szText = LOG_FLAG_INFO;
		break;

	case LOG_IMAGE_WARNING:

		*szText = LOG_FLAG_WARNING;
		break;

	case LOG_IMAGE_ERROR:

		*szText = LOG_FLAG_ERROR;
		break;

#ifdef _DEBUG
	case LOG_IMAGE_DEBUG:

		*szText = LOG_FLAG_DEBUG;
		break;
#endif

	default:
		*szText = 0;
	}
	return szText;
}


/*
	附加信息对话框
*/
BOOL TInfoDlg::EvCreate(LPARAM lParam)
{
	SendDlgItemMessage(IDC_EDIT_INFO, EM_LIMITTEXT , PCK_ADDITIONAL_INFO_SIZE - 1, 0);
	SetDlgItemTextA(IDC_EDIT_INFO, dirBuf);
	return	TRUE;
}

BOOL TInfoDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	switch (wID)
	{
	case IDOK:
		memset(dirBuf, 0, PCK_ADDITIONAL_INFO_SIZE);
		GetDlgItemTextA(IDC_EDIT_INFO, dirBuf, PCK_ADDITIONAL_INFO_SIZE);
		EndDialog(wID);
		return	TRUE;

	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}

/*
	查找对话框
*/
BOOL TSearchDlg::EvCreate(LPARAM lParam)
{
	SendDlgItemMessage(IDC_EDIT_SEARCH, EM_LIMITTEXT , 255, 0);
	SetDlgItemTextA(IDC_EDIT_SEARCH, dirBuf);
	//SendDlgItemMessage(IDC_EDIT_SEARCH, EM_SETSEL, 0, -1);
	//::SetFocus(GetDlgItem(IDC_EDIT_SEARCH));
	return	TRUE;
}

BOOL TSearchDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	switch (wID)
	{
	case IDOK:
		GetDlgItemTextA(IDC_EDIT_SEARCH, dirBuf, 256);
		EndDialog(wID);
		return	TRUE;

	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}

/*
	压缩选项
*/
BOOL TCompressOptDlg::EvCreate(LPARAM lParam)
{
	char	szStr[8];

	SendDlgItemMessage(IDC_EDIT_MEM, EM_LIMITTEXT, 4, 0);

	SendDlgItemMessage(IDC_SLIDER_LEVEL, TBM_SETRANGE , FALSE, MAKELONG(1, 12));
	SendDlgItemMessage(IDC_SLIDER_THREAD, TBM_SETRANGE , FALSE, MAKELONG(1, lpParams->dwMTMaxThread));
	SendDlgItemMessage(IDC_SLIDER_LEVEL, TBM_SETPOS, TRUE, (LPARAM)lpParams->dwCompressLevel);
	SendDlgItemMessage(IDC_SLIDER_THREAD, TBM_SETPOS, TRUE, (LPARAM)lpParams->dwMTThread);

	SetDlgItemTextA(IDC_STATIC_LEVEL, ultoa(lpParams->dwCompressLevel, szStr, 10));
	SetDlgItemTextA(IDC_STATIC_THREAD, ultoa(lpParams->dwMTThread, szStr, 10));
	SetDlgItemTextA(IDC_EDIT_MEM, ultoa((lpParams->dwMTMaxMemory)>>20, szStr, 10));

	return	TRUE;
}

BOOL TCompressOptDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	char	szStr[8];

	switch (wID)
	{
	case IDOK:

		lpParams->dwCompressLevel = SendDlgItemMessage(IDC_SLIDER_LEVEL, TBM_GETPOS, 0, 0);
		lpParams->dwMTThread = SendDlgItemMessage(IDC_SLIDER_THREAD, TBM_GETPOS, 0, 0);

		GetDlgItemTextA(IDC_EDIT_MEM, szStr, 8);
		lpParams->dwMTMaxMemory = atoi(szStr);

		if(0 >= lpParams->dwMTMaxMemory)
			lpParams->dwMTMaxMemory = MT_MAX_MEMORY;
		else 
			(lpParams->dwMTMaxMemory)<<=20;

		EndDialog(wID);
		return	TRUE;

	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}

BOOL TCompressOptDlg::EventScroll(UINT uMsg, int nCode, int nPos, HWND scrollBar)
{
	int		iPos;
	char	szStr[4];

	switch (nCode) { 
        case TB_THUMBTRACK: 

		iPos = ::SendMessage(scrollBar, TBM_GETPOS, 0, 0);

		if(scrollBar == GetDlgItem(IDC_SLIDER_LEVEL))
		{
			SetDlgItemTextA(IDC_STATIC_LEVEL, ultoa(iPos, szStr, 10));
			break;
		}
		if(scrollBar == GetDlgItem(IDC_SLIDER_THREAD))
		{
			SetDlgItemTextA(IDC_STATIC_THREAD, ultoa(iPos, szStr, 10));
			break;
		}

		break;
	}
	return 0;
}

TViewDlg::TViewDlg(char **_buf, DWORD &_dwSize, char *_lpszFile, TWin *_win) : TDlg(IDD_DIALOG_VIEW, _win) 
{
	buf = _buf;
	dwSize = _dwSize;
	lpszFile = _lpszFile;

	//上限10MB	
	//*buf = (char*) malloc (dwSize + 2);
	if(0 != dwSize){
		if(VIEW_TEXT_MAX_BUFFER < dwSize)
			_dwSize = dwSize = VIEW_TEXT_MAX_BUFFER;
		*buf = (char*) malloc (dwSize + 2);
	}else{
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

	if(0 != dwSize){

		*(WORD*)(*buf + dwSize) = 0;

		if(0xfeff == *(WORD*)*buf)
		{
			textType = TEXT_TYPE_UCS2;
			sprintf_s(szTitle, MAX_PATH, "文本查看 - %s (Unicode)",lpszFile);
			lpszTextShow = *buf + 2;

		}else if((0xbbef == *(WORD*)*buf) && (0xbf == *(BYTE*)(*buf + 3))){
			textType = TEXT_TYPE_UTF8;
			sprintf_s(szTitle, MAX_PATH, "文本查看 - %s (UTF-8)",lpszFile);
			lpszTextShow = *buf + 3;

		}else{
			textType = DataType(*buf, dwSize);

			switch(textType)
			{
			case TEXT_TYPE_UTF8:

				//textType = TEXT_TYPE_UTF8;
				sprintf_s(szTitle, MAX_PATH, "文本查看 - %s (UTF-8)",lpszFile);
				break;

			case TEXT_TYPE_ANSI:

				//textType = TEXT_TYPE_ANSI;
				sprintf_s(szTitle, MAX_PATH, "文本查看 - %s",lpszFile);
				break;

			case TEXT_TYPE_RAW:

				if(VIEW_RAW_MAX_BUFFER < dwSize)
					dwSize = VIEW_RAW_MAX_BUFFER;

				sprintf_s(szTitle, MAX_PATH, "文本查看 - %s (RAW)",lpszFile);

				break;


			}

			lpszTextShow = *buf;
		}


		SendDlgItemMessage(IDC_RICHEDIT_VIEW, EM_SETTEXTMODE, TM_PLAINTEXT, 0);
		SendDlgItemMessage(IDC_RICHEDIT_VIEW, EM_EXLIMITTEXT , 0, -1);
		//SendDlgItemMessage(IDC_RICHEDIT_VIEW, EM_SETSEL, 0, -1);

		switch(textType){
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

	}else{

		sprintf_s(szTitle, MAX_PATH, "文本查看 - %s",lpszFile);
	}

	SetWindowTextA(szTitle);

	Show();

	SendDlgItemMessage(IDC_RICHEDIT_VIEW, EM_SETSEL, 0, 0);

	return	FALSE;
}

BOOL TViewDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	switch (wID)
	{
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

	while (*s) {
		if (*s <= 0x7f) {
		}
		else if (*s <= 0xdf) {
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN
		}
		else if (*s <= 0xef) {
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
		}
		else if (*s <= 0xf7) {
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
		}
		else if (*s <= 0xfb) {
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
		}
		else if (*s <= 0xfd) {
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
		}
		++s;
	}

	while (*s) {
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
	if(NULL == (buffer = (char*) malloc (dwDst))){
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

	for(i=BYTES_PER_LINE;i<nFullLine;i+=BYTES_PER_LINE)
	{
		memcpy(raw16, &lpbuf[i-BYTES_PER_LINE] ,BYTES_PER_LINE);

		sprintf(lpbuffer, ADDRESS_FMTSTR, i - BYTES_PER_LINE);
		lpbuffer += ADDRESS_STRLEN;

		for(int j=0;j<BYTES_PER_LINE;++j){

			memcpy(lpbuffer, lpszByte2Raw[raw16[j]], BYTEHEX_STRLEN);
			lpbuffer+=BYTEHEX_STRLEN;

		}

		for(int j=0;j<BYTES_PER_LINE;j++){
			if(0x20 > raw16[j])
				raw16[j] = '.';
		}
		memcpy(lpbuffer, raw16, DATAHEX_STRLEN);
		lpbuffer += DATAHEX_STRLEN;

	}

	int nLeftBytes = rawlength & 0xf;

	if(0 != nLeftBytes){
		memcpy(raw16, &lpbuf[i-BYTES_PER_LINE] ,nLeftBytes);

		sprintf(lpbuffer, ADDRESS_FMTSTR, i - BYTES_PER_LINE);
		lpbuffer += ADDRESS_STRLEN;

		for(int j=0;j<BYTES_PER_LINE;++j)
		{
			if(j < nLeftBytes)
				memcpy(lpbuffer, lpszByte2Raw[raw16[j]], BYTEHEX_STRLEN);
			else
				memcpy(lpbuffer, "   ", BYTEHEX_STRLEN);
			lpbuffer+=BYTEHEX_STRLEN;
		}

		for(int j=0;j<nLeftBytes;j++){
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
	hIC = CreateICA("DISPLAY", NULL, NULL, NULL) ; // information context
	lf.lfHeight				= 24;
	lf.lfWidth				= 0; 
	lf.lfEscapement			= 0; 
	lf.lfOrientation		= 0; 
	lf.lfWeight				= FW_DONTCARE; //default weight
	lf.lfItalic				= FALSE; 
	lf.lfUnderline			= FALSE; 
	lf.lfStrikeOut			= FALSE; 
	lf.lfCharSet			= DEFAULT_CHARSET; 
	lf.lfOutPrecision		= OUT_DEFAULT_PRECIS; 
	lf.lfClipPrecision		= CLIP_DEFAULT_PRECIS; 
	lf.lfQuality			= DEFAULT_QUALITY; 
	lf.lfPitchAndFamily		= DEFAULT_PITCH  | FF_DONTCARE;
	//StringCchCopy(lf.lfFaceName, LF_FACESIZE, TEXT("Fixedsys"));
	memcpy(lf.lfFaceName, "Fixedsys", 9);
	hFont = CreateFontIndirectA(&lf);
	SelectObject (hIC, hFont) ;
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


TAttrDlg::TAttrDlg(void *_lpPckInfo, void *_lpRootInfo, QWORD _qwPckFileSize, char *_lpszPath, BOOL _isSearchMode, TWin *_win) : TDlg(IDD_DIALOG_ATTR, _win) 
{
	lpPckInfo = _lpPckInfo;
	lpRootInfo = _lpRootInfo;
	lpszPath = _lpszPath;
	isSearchMode = _isSearchMode;
	qwPckFileSize = _qwPckFileSize;
	
}

//TAttrDlg::~TAttrDlg()
//{
//}

BOOL TAttrDlg::EvCreate(LPARAM lParam)
{
	//ultoa(lpPckNode->lpPckIndexTable->cFileIndex.dwFileCipherTextSize, szPrintf, 10));

	char	*lpszFilename;
	char	szPrintf[64];
	char	szPrintfBytesize[16];
	LPCSTR	lpszFormat = "%s (%llu 字节)";
	LPCSTR	lpszFileFormat = "%s (%u 字节)";



	if(isSearchMode)
	{
		LPPCKINDEXTABLE	lpPckIndex = (LPPCKINDEXTABLE)lpPckInfo;
		char	szFilename[MAX_PATH_PCK_260];

		memcpy(szFilename, lpPckIndex->cFileIndex.szFilename, MAX_PATH_PCK_260);

		SetDlgItemTextA(IDC_EDIT_ATTR_TYPE, "文件");
		
		if(NULL != (lpszFilename = strrchr(szFilename, '\\'))){

			*lpszFilename++ = 0;
			SetDlgItemTextA(IDC_EDIT_ATTR_PATH, szFilename);

		}else if(NULL != (lpszFilename = strrchr(szFilename, '/'))){

			*lpszFilename++ = 0;
			SetDlgItemTextA(IDC_EDIT_ATTR_PATH, szFilename);

		}else{
			
			lpszFilename = lpPckIndex->cFileIndex.szFilename;
			SetDlgItemTextA(IDC_EDIT_ATTR_PATH, "");
		}

		SetDlgItemTextA(IDC_EDIT_ATTR_NAME, lpszFilename);

		//压缩大小
		sprintf_s(szPrintf, 64, lpszFileFormat,
					StrFormatByteSizeA(lpPckIndex->cFileIndex.dwFileCipherTextSize, szPrintfBytesize, 16),
					lpPckIndex->cFileIndex.dwFileCipherTextSize);

		SetDlgItemTextA(IDC_EDIT_ATTR_CIPHER, szPrintf);
		
		//实际大小
		sprintf_s(szPrintf, 64, lpszFileFormat,
					StrFormatByteSizeA(lpPckIndex->cFileIndex.dwFileClearTextSize, szPrintfBytesize, 16),
					lpPckIndex->cFileIndex.dwFileClearTextSize);

		SetDlgItemTextA(IDC_EDIT_ATTR_SIZE, szPrintf);
		
		//压缩率
		if(0 == lpPckIndex->cFileIndex.dwFileClearTextSize)
			SetDlgItemTextA(IDC_EDIT_ATTR_CMPR, "-");
		else
		{
			sprintf_s(szPrintf, 64, "%.1f%%", lpPckIndex->cFileIndex.dwFileCipherTextSize * 100/ (double)lpPckIndex->cFileIndex.dwFileClearTextSize);
			SetDlgItemTextA(IDC_EDIT_ATTR_CMPR, szPrintf);
		}

		//包含
		SetDlgItemTextA(IDC_EDIT_ATTR_FILECOUNT, "-");

		//偏移地址
//		#ifdef PCKV202
		sprintf_s(szPrintf, 64, "%llu (0x%016llX)", lpPckIndex->cFileIndex.dwAddressOffset, lpPckIndex->cFileIndex.dwAddressOffset);
		SetDlgItemTextA(IDC_EDIT_ATTR_ADDR, szPrintf);

	}else{

		LPPCK_PATH_NODE	lpPckNode = (LPPCK_PATH_NODE)lpPckInfo;

		SetDlgItemTextA(IDC_EDIT_ATTR_NAME, lpszFilename = lpPckNode->szName);
		SetDlgItemTextA(IDC_EDIT_ATTR_PATH, lpszPath);

		if(NULL == lpPckNode->child)//文件
		{
			SetDlgItemTextA(IDC_EDIT_ATTR_TYPE, "文件");

			//压缩大小
			sprintf_s(szPrintf, 64, lpszFileFormat,
						StrFormatByteSizeA(lpPckNode->lpPckIndexTable->cFileIndex.dwFileCipherTextSize, szPrintfBytesize, 16),
						lpPckNode->lpPckIndexTable->cFileIndex.dwFileCipherTextSize);

			SetDlgItemTextA(IDC_EDIT_ATTR_CIPHER, szPrintf);


			//实际大小
			sprintf_s(szPrintf, 64, lpszFileFormat,
						StrFormatByteSizeA(lpPckNode->lpPckIndexTable->cFileIndex.dwFileClearTextSize, szPrintfBytesize, 16),
						lpPckNode->lpPckIndexTable->cFileIndex.dwFileClearTextSize);

			SetDlgItemTextA(IDC_EDIT_ATTR_SIZE, szPrintf);



			//压缩率
			if(0 == lpPckNode->lpPckIndexTable->cFileIndex.dwFileClearTextSize)
				SetDlgItemTextA(IDC_EDIT_ATTR_CMPR, "-");
			else
			{
				sprintf_s(szPrintf, 64, "%.1f%%", lpPckNode->lpPckIndexTable->cFileIndex.dwFileCipherTextSize / (double)lpPckNode->lpPckIndexTable->cFileIndex.dwFileClearTextSize * 100);
				SetDlgItemTextA(IDC_EDIT_ATTR_CMPR, szPrintf);
			}

			//包含
			SetDlgItemTextA(IDC_EDIT_ATTR_FILECOUNT, "-");

			//偏移地址
			sprintf_s(szPrintf, 64, "%llu (0x%016llX)", lpPckNode->lpPckIndexTable->cFileIndex.dwAddressOffset, lpPckNode->lpPckIndexTable->cFileIndex.dwAddressOffset);
			SetDlgItemTextA(IDC_EDIT_ATTR_ADDR, szPrintf);
		}else{

			SetDlgItemTextA(IDC_EDIT_ATTR_TYPE, "文件夹");

			//压缩大小
			sprintf_s(szPrintf, 64, lpszFormat,
						StrFormatByteSize64A(lpPckNode->child->qdwDirCipherTextSize, szPrintfBytesize, 16),
						lpPckNode->child->qdwDirCipherTextSize);
			SetDlgItemTextA(IDC_EDIT_ATTR_CIPHER, szPrintf);

			
			//实际大小
			sprintf_s(szPrintf, 64, lpszFormat,
						StrFormatByteSize64A(lpPckNode->child->qdwDirClearTextSize, szPrintfBytesize, 16),
						lpPckNode->child->qdwDirClearTextSize);

			SetDlgItemTextA(IDC_EDIT_ATTR_SIZE, szPrintf);


			//压缩率
			sprintf_s(szPrintf, 64, "%.1f%%", lpPckNode->child->qdwDirCipherTextSize / (double)lpPckNode->child->qdwDirClearTextSize * 100.0);
			SetDlgItemTextA(IDC_EDIT_ATTR_CMPR, szPrintf);

			//包含
			sprintf_s(szPrintf, 64, "%u 个文件，%u 个文件夹", lpPckNode->child->dwFilesCount, lpPckNode->child->dwDirsCount);
			SetDlgItemTextA(IDC_EDIT_ATTR_FILECOUNT, szPrintf);

			SetDlgItemTextA(IDC_EDIT_ATTR_ADDR, "-");

		}


	}

	//文件包信息
	LPPCK_PATH_NODE	lpRootNode = (LPPCK_PATH_NODE)lpRootInfo;
	//文件总大小
	sprintf_s(szPrintf, 64, lpszFormat,
				StrFormatByteSize64A(lpRootNode->child->qdwDirClearTextSize, szPrintfBytesize, 16),
				lpRootNode->child->qdwDirClearTextSize);

	SetDlgItemTextA(IDC_EDIT_ATTR_ALLSIZE, szPrintf);

	//压缩包大小
	sprintf_s(szPrintf, 64, lpszFormat,
				StrFormatByteSize64A(lpRootNode->child->qdwDirCipherTextSize, szPrintfBytesize, 16),
				lpRootNode->child->qdwDirCipherTextSize);
	SetDlgItemTextA(IDC_EDIT_ATTR_PCKSIZE, szPrintf);

	//压缩率
	sprintf_s(szPrintf, 64, "%.1f%%", lpRootNode->child->qdwDirCipherTextSize / (double)lpRootNode->child->qdwDirClearTextSize * 100.0);
	SetDlgItemTextA(IDC_EDIT_ATTR_PCKCMPR, szPrintf);

	//冗余数据量
	//int dwDirt = dwPckFileSize - lpRootNode->child->dwDirCipherTextSize;
	sprintf_s(szPrintf, 64, lpszFormat,
				StrFormatByteSize64A(qwPckFileSize, szPrintfBytesize, 16),
				qwPckFileSize);
	SetDlgItemTextA(IDC_EDIT_ATTR_DIRT, szPrintf);


	//窗口文字
	char	szTitle[MAX_PATH];
	sprintf_s(szTitle, MAX_PATH, "%s 属性",lpszFilename);
	SetWindowTextA(szTitle);

	Show();

	return	FALSE;
}

BOOL TAttrDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	switch (wID)
	{
	case IDOK:
		EndDialog(wID);
		return	TRUE;

	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////

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
		*buf = (char*) malloc (dwSize);
	}else{							//bmp,jpg,png..
		*buf = (char*)(hGlobal = GlobalAlloc(GMEM_FIXED, dwSize));
	}

	GdiplusStartup( &g_pGdiPlusToken, &g_oGdiPlusStartupInput, NULL );

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

	if(FMT_RAW != iFormat){
		if(NULL != *buf){
			free(*buf);
		}
	}else{

		if(NULL != hGlobal)
			GlobalFree(hGlobal);
	}

	if(NULL != cleanimage){
		free(cleanimage);
	}

	//DeleteObject( hbEditBack );
	//SelectObject(MemDC, pOldBit);
	DeleteObject(MemBitmap);
	DeleteDC(MemDC);

	GdiplusShutdown( g_pGdiPlusToken );
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
	UINT num= 0;
	UINT size= 0;

	ImageCodecInfo* pImageCodecInfo= NULL;

	GetImageEncodersSize(&num, &size);
	if(size== 0)
	{
		return -1;
	}
	pImageCodecInfo= (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo== NULL)
	{
		return -1;
	}

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j=0; j< num; ++j)
	{
		if(wcscmp(pImageCodecInfo[j].MimeType, format)== 0)
		{
			*pClsid= pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}

	free(pImageCodecInfo);
	return -1;
}

BOOL TPicDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	switch (wID)
	{
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
	
	ZeroMemory(& ofn, sizeof (OPENFILENAMEW));
	ofn.lStructSize       = sizeof (OPENFILENAMEW) ;
	ofn.hwndOwner         = hWnd;
	ofn.lpstrFilter       = TEXT_SAVE_FILTER;
	//ofn.lpstrFilter       = AtoW(TEXT_SAVE_FILTER);
	ofn.lpstrFile         = lpszFileName ;
	ofn.nMaxFile          = MAX_PATH ;
	ofn.Flags             = OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING;

	if(!GetSaveFileNameW( & ofn ))
	{
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

	switch(iFormat)
	{
	case FMT_DDS:

		if(!CreateBmpBufferByDds())
		{
			MessageBoxA(TEXT_SHOWPIC_ERROR, NULL, MB_OK | MB_ICONERROR);
			EndDialog(IDCANCEL);
			return FALSE;
		}

		free(*buf);
		*buf = NULL;

		lpmyImage = new Bitmap(picWidth, picHeight, stride, decodedDIBFormat, (BYTE*)cleanimage);
		
		break;

	case FMT_TGA:

		if(!CreateBmpBufferByTga())
		{
			MessageBoxA(TEXT_SHOWPIC_ERROR, NULL, MB_OK | MB_ICONERROR);
			EndDialog(IDCANCEL);
			return FALSE;
		}

		free(*buf);
		*buf = NULL;

		lpmyImage = new Bitmap(picWidth, picHeight, stride, decodedDIBFormat, (BYTE*)cleanimage);
		break;

	case FMT_RAW:	//bmp,jpg,png..

		CreateStreamOnHGlobal(hGlobal,false,&s);
		if (!s)
		{
			MessageBoxA(TEXT_SHOWPIC_ERROR, NULL, MB_OK | MB_ICONERROR);
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


	if(rectWin.right < picWidth || rectWin.bottom < picHeight)
	{

		if(rectWin.right < picWidth)
		{
			ShowScrollBar(hWnd, SB_HORZ, TRUE);
			SetScrollRange(hWnd, SB_HORZ, 0, picWidth - rectWin.right, TRUE);
		}else{

			ShowScrollBar(hWnd, SB_HORZ, FALSE);
		}

		GetClientRect(hWnd, &rectWin);

		if(rectWin.bottom < picHeight)
		{
			ShowScrollBar(hWnd, SB_VERT, TRUE);
			SetScrollRange(hWnd, SB_VERT, 0, picHeight - rectWin.bottom, TRUE);

			GetClientRect(hWnd, &rectWin);


			if(rectWin.right < picWidth)
			{
				ShowScrollBar(hWnd, SB_HORZ, TRUE);
				SetScrollRange(hWnd, SB_HORZ, 0, picWidth - rectWin.right, TRUE);
			}else{
				ShowScrollBar(hWnd, SB_HORZ, FALSE);
			}

		}else{
			ShowScrollBar(hWnd, SB_VERT, FALSE);
		}

	}else{

		::SetWindowPos(hWnd, NULL, 0, 0, picWidth + 20, picHeight + 50, SWP_NOZORDER | SWP_NOMOVE);
	}

	if(NULL != lpoGraph)
		delete lpoGraph;
	lpoGraph = NULL;

	//窗口文字
	if(FMT_RAW != iFormat)
		sprintf_s(szTitle, MAX_PATH, "图片查看 - %s, %dx%d, %s", lpszFile, picWidth, picHeight, szDdsFormat);
	else{
		sprintf_s(szTitle, MAX_PATH, "图片查看 - %s, %dx%d",lpszFile, picWidth, picHeight);

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

	switch(uMsg)
	{
	case WM_HSCROLL:

		nCurPos=GetScrollPos(hWnd, SB_HORZ);

		switch(nCode)
		{
		case SB_LINEUP:
				if(nCurPos>0)
				{
					nCurPos-=1;
					//ScrollWindow(hWnd, 1, 0, 0, 0);
				}
				break;
		case SB_LINEDOWN:
				if(nCurPos<20)
				{
					nCurPos+=1;
					//ScrollWindow(hWnd, -1, 0, 0, 0);
				}
				break;
		case SB_PAGEUP:
				nCurPos-=4;
				//ScrollWindow(hWnd, 4, 0, 0, 0);
				break;
		case SB_PAGEDOWN:
				nCurPos+=4;
				//ScrollWindow(hWnd, -4, 0, 0, 0);
				break;
		case SB_THUMBTRACK:
				//ScrollWindow(hWnd, nCurPos - nPos, 0, 0, 0);
				nCurPos=nPos;
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

		nCurPos=GetScrollPos(hWnd, SB_VERT);

		switch(nCode)
		{
		case SB_LINEUP:
				if(nCurPos>0)
				{
					nCurPos-=1;
					//ScrollWindow(hWnd, 0, 1, 0, 0);
				}
				break;
		case SB_LINEDOWN:
				if(nCurPos<20)
				{
					nCurPos+=1;
					//ScrollWindow(hWnd, 0,-1, 0, 0);
				}
				break;
		case SB_PAGEUP:
				nCurPos-=4;
				//ScrollWindow(hWnd, 0, 4, 0, 0);
				break;
		case SB_PAGEDOWN:
				nCurPos+=4;
				//ScrollWindow(hWnd, 0, -4, 0, 0);
				break;
		case SB_THUMBTRACK:
				//ScrollWindow(hWnd, 0, nCurPos - nPos, 0, 0);
				nCurPos=nPos;
				break;
		default:
				break;            
		}
		SetScrollPos(hWnd, SB_VERT,nCurPos, TRUE);

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

		if(nWidth < picWidth || nHeight < picHeight)
		{

			if(nWidth < picWidth)
			{
				ShowScrollBar(hWnd, SB_HORZ, TRUE);

				nCurPos = GetScrollPos(hWnd, SB_HORZ);

				if(nCurPos > picWidth - nWidth)
				{
					nCurPos = picWidth - nWidth;
					x = 0 - nCurPos;
				}

				SetScrollRange(hWnd, SB_HORZ, 0, picWidth - nWidth, TRUE);
			}else{
				x = 0;
				ShowScrollBar(hWnd, SB_HORZ, FALSE);
			}

			if(nHeight < picHeight)
			{
				ShowScrollBar(hWnd, SB_VERT, TRUE);

				nCurPos = GetScrollPos(hWnd, SB_VERT);

				if(nCurPos > picHeight - nHeight)
				{
					nCurPos = picHeight - nHeight;
					y = 0 - nCurPos;
				}

				SetScrollRange(hWnd, SB_VERT, 0, picHeight - nHeight, TRUE);

			}else{
				y = 0;
				ShowScrollBar(hWnd, SB_VERT, FALSE);
			}

		}else{
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
	switch(uMsg)
	{
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
		if(0 != iFormat)
		{	
			WCHAR	szFilename[MAX_PATH];
			::GetCurrentDirectoryW(MAX_PATH, szFilename);

			wcscat_s(szFilename, MAX_PATH, L"\\");
			wcscat_s(szFilename, MAX_PATH, AtoW(lpszFile));

			//*(DWORD*)strrchr(szFilename, '.') = *(DWORD*)".png";//0x706D622E;	//.bmp
			memcpy(wcsrchr(szFilename, L'.'), L".png\0\0", 12);
				
			if(SaveFile(szFilename))
			{
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

	if(isMouseDown){
		POINT	pointLast = pointMouse;
		GetCursorPos(&pointMouse);

		int		xOffset = pointLast.x - pointMouse.x;
		int		yOffset = pointLast.y - pointMouse.y;

		RECT	rectWin;
		GetClientRect(hWnd, &rectWin);


		int nCurPos, nCurPosMax;

		if(rectWin.right < picWidth)
		{
			GetScrollRange(hWnd, SB_HORZ, &nCurPos, &nCurPosMax);

			nCurPos = GetScrollPos(hWnd, SB_HORZ);
			nCurPos += xOffset;

			if(0 >nCurPos)nCurPos = 0;
			if(nCurPosMax < nCurPos)nCurPos = nCurPosMax;
		
			SetScrollPos(hWnd, SB_HORZ, nCurPos, TRUE);

			x = 0 - nCurPos;

		}

		if(rectWin.bottom < picHeight)
		{
			GetScrollRange(hWnd, SB_VERT, &nCurPos, &nCurPosMax);

			nCurPos = GetScrollPos(hWnd, SB_VERT);
			nCurPos += yOffset;

			if(0 >nCurPos)nCurPos = 0;
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
	SetBkColor(MemDCTemp, RGB(204,204,204));
	RECT thisrect = {0, 0, 24, 8};
	ExtTextOut(MemDCTemp, 0, 0, ETO_OPAQUE, &thisrect, NULL, 0, NULL);

	thisrect.left += 8;
	SetBkColor(MemDCTemp, RGB(255,255,255));
	ExtTextOut(MemDCTemp, 0, 0, ETO_OPAQUE, &thisrect, NULL, 0, NULL);

	thisrect.left += 8;
	SetBkColor(MemDCTemp, RGB(204,204,204));
	ExtTextOut(MemDCTemp, 0, 0, ETO_OPAQUE, &thisrect, NULL, 0, NULL);

	for(int j=0;j<picHeight;j+=8){
		for(int i = 0;i<picWidth;i+=16){
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