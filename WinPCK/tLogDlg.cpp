//////////////////////////////////////////////////////////////////////
// tLogDlg.cpp: WinPCK 界面线程部分
// 对话框代码
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2017.12.26
//////////////////////////////////////////////////////////////////////


#include "tLogDlg.h"
#include <stdio.h>
#include "ShowLogOnDlgListView.h"

#define	LOG_BUFFER						8192

//日志
#define LOG_IMAGE_NOTICE				0
#define LOG_IMAGE_INFO					1
#define LOG_IMAGE_WARNING				2
#define LOG_IMAGE_ERROR					3
#define LOG_IMAGE_DEBUG					4
#define LOG_IMAGE_EMPTY					5
#define LOG_IMAGE_COUNT					6


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
	ListView_InsertColumn(hWndList, lvcolumn.iSubItem, &lvcolumn);

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
	GetModuleFileNameW(NULL, szExePath, MAX_PATH);
	wchar_t *lpexeTitle = wcsrchr(szExePath, '\\') + 1;
	*lpexeTitle = 0;

	//Show();

	return	TRUE;
}

BOOL TLogDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	//char szPrintf[300];
	//char *lpszFile;

	switch(wID) {
	case IDOK:
		return	TRUE;

	case IDCANCEL:
		return	TRUE;

	case ID_MENU_LOG_CLEAR:

		ListView_DeleteAllItems(hWndList);

		break;

	case ID_MENU_LOG_SAVE_SEL:
	{
		LVITEMW item = { 0 };

		item.mask = LVIF_STATE;
		item.stateMask = LVIS_SELECTED;

		FILE *fplog = _wfopen(pszLogFileName(), L"wb");
		if(!fplog) {

			::MessageBoxW(hWnd, L"保存日志文件失败", L"error", MB_OK);
			return TRUE;
		}

		int	nItemCount = ::SendMessage(hWndList, LVM_GETITEMCOUNT, 0, 0);

		for(item.iItem = 0;item.iItem < nItemCount;++item.iItem) {
			//ListView_GetItem(hWndList, &item);
			::SendMessageW(hWndList, LVM_GETITEMW, 0, (LPARAM)(LV_ITEMW*)&item);
			if(item.state & LVIS_SELECTED) {
				fwprintf(fplog, L"%s\r\n", pszTargetListLog(item.iItem));
			}
		}

		fclose(fplog);

	}
	break;

	case ID_MENU_LOG_SAVE_ALL:
	{
		LVITEMW item = { 0 };

		FILE *fplog = _wfopen(pszLogFileName(), L"wb");
		if(!fplog) {

			::MessageBoxW(hWnd, L"保存日志文件失败", L"error", MB_OK);
			return TRUE;
		}

		int	nItemCount = ::SendMessageA(hWndList, LVM_GETITEMCOUNT, 0, 0);

		char header[] = "\xff\xfe";
		fwrite(header, 1, sizeof(header), fplog);

		for(int i = 0;i < nItemCount;++i) {
			fwprintf(fplog, L"%s\r\n", pszTargetListLog(i));
		}

		fclose(fplog);
	}
	break;

	case ID_MENU_LOG_COPY:
	{
		wchar_t *lpszLogText = pszTargetListLog(m_iCurrentHotItem);
		size_t nLen = wcslen(lpszLogText) + 1;

		//拷入剪切板
		if(OpenClipboard(hWnd)) {
			HGLOBAL clipbuffer;
			char *buffer;
			EmptyClipboard();
			if(NULL != (clipbuffer = GlobalAlloc(GMEM_DDESHARE, nLen*2))) {
				buffer = (char *)GlobalLock(clipbuffer);
				//StringCchCopyA(buffer, nLen, lpszLogText);
				memcpy(buffer, lpszLogText, nLen * 2);
				GlobalUnlock(clipbuffer);

				SetClipboardData(CF_UNICODETEXT, clipbuffer);

				CloseClipboard();
				GlobalFree(clipbuffer);

			} else {
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
	switch(ctlID) {
	case IDC_LIST_LOG:

		switch(pNmHdr->code) {
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

wchar_t*  TLogDlg::pszLogFileName()
{
	static wchar_t logfile[MAX_PATH];
	SYSTEMTIME systime;

	//创建文件exportlog_2015-05-04_13_22_33.log
	GetLocalTime(&systime);
	swprintf_s(logfile, L"%sexportlog_%04d-%02d-%02d_%02d_%02d_%02d.log", szExePath, \
		systime.wYear, systime.wMonth, systime.wDay, \
		systime.wHour, systime.wMinute, systime.wSecond);
	return logfile;

}

wchar_t*  TLogDlg::pszTargetListLog(int iItem)
{
	static wchar_t szText[8192];

	LVITEMW item = { 0 };

	*(szText + 1) = ' ';

	item.iItem = iItem;
	item.mask = LVIF_IMAGE | LVIF_TEXT;
	item.pszText = szText + 2;
	item.cchTextMax = 8190;
	::SendMessageW(hWndList, LVM_GETITEMW, 0, (LPARAM)(LV_ITEMW*)&item);

	*szText = m_szLogPrefix[item.iImage];

	return szText;
}


void TLogDlg::InsertLogToList(const char _loglevel, const wchar_t *_logtext)
{

	if ('N' == _loglevel) {
		LogUnits.SetStatusBarInfo(_logtext);
	}
	else if ('E' == _loglevel) {
		ShowWindow(SW_SHOW);
	}

	wchar_t szPrintf[8192];
	size_t nTextLen = wcslen(_logtext);

	//查找\n 如果存在，\n后的内容换行（用新的列）显示
	const wchar_t *lpString2Show = _logtext, *lpString2Search = _logtext, *lpStringTail = _logtext + nTextLen;
	char loglevel = _loglevel;

	while (0 != *lpString2Search) {

		if ('\n' == *lpString2Search) {
			memcpy(szPrintf, lpString2Show, (lpString2Search - lpString2Show) * 2);
			*(szPrintf + (lpString2Search - lpString2Show)) = 0;
			_InsertLogIntoList(loglevel, szPrintf);
			lpString2Show = lpString2Search + 1;
			loglevel = ' ';
		}

		lpString2Search++;
	}
	if (0 != *lpString2Show) {
		memcpy(szPrintf, lpString2Show, (lpString2Search - lpString2Show) * 2);
		*(szPrintf + (lpString2Search - lpString2Show)) = 0;
		_InsertLogIntoList(loglevel, szPrintf);
	}
}

int TLogDlg::log_level_char_to_int(const char level)
{
	switch (level)
	{
	case 'N':
		return LOG_IMAGE_NOTICE;
	case 'I':
		return LOG_IMAGE_INFO;
	case 'W':
		return LOG_IMAGE_WARNING;
	case 'E':
		return LOG_IMAGE_ERROR;
	case 'D':
		return LOG_IMAGE_DEBUG;
	case ' ':
	default:
		return LOG_IMAGE_EMPTY;
	}
}

#define LEN_OF_PREFIX	9

void TLogDlg::_InsertLogIntoList(const char _loglevel, const wchar_t *_logtext)
{

	LVITEMW	item;
	wchar_t szPrintf[LOG_BUFFER + 9];
	//size_t nLenOfPrefix = wcslen(szPrintf);
	wchar_t *lpPointToPaestString = szPrintf + LEN_OF_PREFIX;
	//123456789
	//09:06:17 WinPCK v1.22.9  is started.
	if (' ' == _loglevel) {
		wcsnset(szPrintf, ' ', LEN_OF_PREFIX);
	}
	else {
		SYSTEMTIME systime;
		GetLocalTime(&systime);
		swprintf_s(szPrintf, L"%02d:%02d:%02d ", systime.wHour, systime.wMinute, systime.wSecond);
	}

	wcscpy_s(lpPointToPaestString, LOG_BUFFER, _logtext);

	ZeroMemory(&item, sizeof(LVITEMW));

	item.iItem = INT_MAX;			//从0开始
	item.iImage = log_level_char_to_int(_loglevel);
	//item.iSubItem = 0;
	item.mask = LVIF_TEXT | LVIF_IMAGE;
	item.pszText = szPrintf;

	if (-1 != (m_LogListCount = ::SendMessageW(hWndList, LVM_INSERTITEMW, 0, (LPARAM)&item))) {
		ListView_EnsureVisible(hWndList, m_LogListCount, 0);
		//++m_LogListCount;
	}
}
#undef LEN_OF_PREFIX