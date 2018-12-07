//////////////////////////////////////////////////////////////////////
// listViewFunc.cpp: WinPCK 界面线程ListView部分
// ListView控件相关操作
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2019.12.25
//////////////////////////////////////////////////////////////////////
#include "tlib.h"
#include "resource.h"
#include "winmain.h"
#include <process.h>
#include <CommCtrl.h>

#define DEBUG_THIS 0
#if !DEBUG_THIS
#include "tDebug.h"
#endif

#define MAX_COLUMN_COUNT		64

#define SORT_FLAG_ASCENDING		0x00010000
#define SORT_FLAG_DESCENDING	0x00020000
//#define SORT_FLAG_SEARCHMODE	0x00040000

#ifdef _USE_CUSTOMDRAW_

#define	HB_GREEN					0
#define	HB_GRAY						1
#define	HB_SELECTED					2
//#define	HB_NOFOCUS					3
#define	HB_COUNT					3

//ListView Colors
#define FILL_COLOR_SELECTED			RGB(200,240,180)
//#define FRAME_COLOR_SELECTED		RGB(000,000,000)
//#define FILL_COLOR_NOFOCUS			RGB(200,240,255)
//#define FRAME_COLOR_NOFOCUS			RGB(100,100,100)
//#define FILL_COLOR_RED				RGB(255,220,220)
//#define FILL_COLOR_YELLOW			RGB(255,255,200)
#define FILL_COLOR_GRAY				RGB(237,238,250)
#define FILL_COLOR_GREEN			RGB(232,254,255)

typedef enum { R_NOTHINGS = 0, R_NORMAL, R_SELECT, R_SEL_NOFOCUS } redrawmode;
redrawmode redraw;
HICON				hIconList[2];
HBRUSH				hBrushs[HB_COUNT];
int					fmtTextAlign[MAX_COLUMN_COUNT];
#endif

DWORD dwSortStatus = 0;

BOOL TInstDlg::EvNotifyListView(const NMHDR *pNmHdr)
{
	int iCurrentHotItem = ((LPNMLISTVIEW)pNmHdr)->iItem;
	//Debug(L"D ListView:LVN_FIRST %d,all:NM_FIRST-%d\r\n", LVN_FIRST-pNmHdr->code, NM_FIRST - pNmHdr->code);
	switch(pNmHdr->code) {
	case LVN_COLUMNCLICK:
		ProcessColumnClick(((LPNMLISTVIEW)pNmHdr)->hdr.hwndFrom, (LPNMLISTVIEW)pNmHdr, dwSortStatus);
		break;
	case LVN_ITEMCHANGED:
	case NM_CLICK:
		if(-1 != iCurrentHotItem) {
			m_iListHotItem = iCurrentHotItem;
#ifdef _DEBUG
			const PCK_UNIFIED_FILE_ENTRY* lpFileEntry = GetFileEntryByItem(iCurrentHotItem);
			DebugA("nFilelenBytes/nNameSizeAnsi = %d\r\n", pck_getFilelenBytesOfEntry(lpFileEntry));
			DebugA("nFilelenLeftBytes/nMaxNameSizeAnsi = %d\r\n", pck_getFilelenLeftBytesOfEntry(lpFileEntry));
#endif
		}
		break;
	case NM_RCLICK:
		if(-1 != iCurrentHotItem) {
			PopupRightMenu(iCurrentHotItem);
		}
		break;
	case NM_DBLCLK:
		if(-1 != iCurrentHotItem) {
			DbClickListView(iCurrentHotItem);
		}
		break;
	case LVN_BEGINDRAG:
		SetCapture();
		SetCursor(m_hCursorAllow);
		m_isSearchWindow = TRUE;
		break;
	case LVN_BEGINLABELEDIT:
		return ListView_BeginLabelEdit(pNmHdr->hwndFrom, ((NMLVDISPINFO*)pNmHdr)->item.lParam);
		break;
	case LVN_ENDLABELEDIT:
		return ListView_EndLabelEdit((NMLVDISPINFO*)pNmHdr);
		break;
	case NM_RETURN:
		break;
	case LVN_KEYDOWN:

		DebugA("D ListView wVKey:%d(%c)\r\n", ((NMLVKEYDOWN*)pNmHdr)->wVKey, ((NMLVKEYDOWN*)pNmHdr)->wVKey);

		BOOL bCtrl = ::GetKeyState(VK_CONTROL) & 0x80;
		//BOOL bShift = ::GetKeyState(VK_SHIFT) & 0x80;  
		BOOL bAlt = ::GetKeyState(VK_MENU) & 0x80; 

		if(bCtrl) {
			switch(((NMLVKEYDOWN*)pNmHdr)->wVKey) {
			//case 'I':
			//	break;
			//case 'W':
			//	break;
			//case 'N':
			//	break;
			//case 'O':
			//	break;
			case 'A':
				SendMessage(WM_COMMAND, ID_MENU_SELECTALL, 0);
				break;
			case 'R':
				SendMessage(WM_COMMAND, ID_MENU_SELECTORP, 0);
				break;
			default:
				break;
			}
		} else if(bAlt) {
			switch(((NMLVKEYDOWN*)pNmHdr)->wVKey) {
			case VK_RETURN:
				SendMessage(WM_COMMAND, MAKELONG(ID_MENU_ATTR, 0), 0);
				break;
			default:
				break;
			}
		} else {
			switch(((NMLVKEYDOWN*)pNmHdr)->wVKey) {
			case VK_BACK:
				DbClickListView(0);
				break;
			case VK_APPS:
				PopupRightMenu(m_iListHotItem);
				break;
			case VK_F2:
				SendMessage(WM_COMMAND, ID_MENU_RENAME, 0);
				break;
			case VK_DELETE:
				SendMessage(WM_COMMAND, ID_MENU_DELETE, 0);
				break;
			default:
				break;
			}
		}

		break;
	}
	return FALSE;
}


void TInstDlg::ListView_Init()
{
	//const LPTSTR	lpszID_LIST[] =	{TEXT("文件名"), TEXT("实标大小"), TEXT("压缩大小"), TEXT("压缩率")/*, TEXT("未知2")*/};
	const LPTSTR	lpszID_LIST[] = { 
		GetLoadStr(IDS_STRING_LISTHEAD1), 
		GetLoadStr(IDS_STRING_LISTHEAD2), 
		GetLoadStr(IDS_STRING_LISTHEAD3), 
		GetLoadStr(IDS_STRING_LISTHEAD4) };
	const int		iID_LIST_cx[] = { 558, 80, 80, 60, -1 };
	const int		ifmt[] = { LVCFMT_LEFT, LVCFMT_RIGHT, LVCFMT_RIGHT, LVCFMT_RIGHT };
	InitListView(GetDlgItem(IDC_LIST), lpszID_LIST, iID_LIST_cx, ifmt);

#ifdef _USE_CUSTOMDRAW_
	//ListView所用的画刷
	const COLORREF		colorHB[] = { 
		FILL_COLOR_GREEN,
		FILL_COLOR_GRAY,
		FILL_COLOR_SELECTED/*,
		FILL_COLOR_NOFOCUS*/ };

	for(int i = 0;i < HB_COUNT;i++) {
		hBrushs[i] = CreateSolidBrush(colorHB[i]);
	}

	int colCount = sizeof(ifmt) / sizeof(int);

	for(int i = 0;i < colCount;i++) {

		switch(ifmt[i]) {

		case LVCFMT_RIGHT:
			fmtTextAlign[i] = DT_RIGHT;
			break;
		case LVCFMT_CENTER:
			fmtTextAlign[i] = DT_CENTER;
			break;
		case LVCFMT_LEFT:
		default:
			fmtTextAlign[i] = DT_LEFT;
			break;
		}
	}

	//ListView所用的图标
	hIconList[0] = LoadIcon(TApp::GetInstance(), MAKEINTRESOURCE(IDI_ICON_DIR));
	hIconList[1] = LoadIcon(TApp::GetInstance(), MAKEINTRESOURCE(IDI_ICON_FILE));
	//hIconList[2] = LoadIcon(TApp::GetInstance(), MAKEINTRESOURCE(IDI_ICON_FILE));

#endif
}

void TInstDlg::ListView_Uninit()
{
	ImageList_Destroy(m_imageList);
#ifdef _USE_CUSTOMDRAW_
	for(int i = 0; i < HB_COUNT; i++) {
		DeleteObject(hBrushs[i]);
	}

	for(UINT i = 0; i < 2; i++) {
		DestroyIcon(hIconList[i]);
	}

#endif
}

int CALLBACK ListViewCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{

	const PCK_UNIFIED_FILE_ENTRY* lpFileEntry1 = (LPPCK_UNIFIED_FILE_ENTRY)lParam1;
	const PCK_UNIFIED_FILE_ENTRY* lpFileEntry2 = (LPPCK_UNIFIED_FILE_ENTRY)lParam2;

	DWORD dwSortStat = *(DWORD*)lParamSort;
	int iLastSort = dwSortStat & 0xffff;
	int iResult;

	//如果是上级目录(".."目录)
	if (PCK_ENTRY_TYPE_DOTDOT == (PCK_ENTRY_TYPE_DOTDOT & (lpFileEntry1->entryType | lpFileEntry2->entryType)))
		return 0;

	//如果存在目录和文件比较
	if (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & (lpFileEntry1->entryType ^ lpFileEntry2->entryType)))
		return 0;

	switch (iLastSort) {
		//文件名
	case 0:
		iResult = wcsicmp(lpFileEntry1->szName, lpFileEntry2->szName);
		break;
	case 1:
		iResult = pck_getFileSizeInEntry(lpFileEntry1) - pck_getFileSizeInEntry(lpFileEntry2);
		break;
	case 2:
		iResult = pck_getCompressedSizeInEntry(lpFileEntry1) - pck_getCompressedSizeInEntry(lpFileEntry2);
		break;
	case 3:
		float a, b;

		if (0 == pck_getFileSizeInEntry(lpFileEntry1))
			a = 0.0;
		else
			a = (float)pck_getCompressedSizeInEntry(lpFileEntry1) / (float)pck_getFileSizeInEntry(lpFileEntry1);
		if (0 == pck_getFileSizeInEntry(lpFileEntry2))
			b = 0.0;
		else
			b = (float)pck_getCompressedSizeInEntry(lpFileEntry2) / (float)pck_getFileSizeInEntry(lpFileEntry2);

		iResult = (int)((a > b) ? 1 : -1);
		break;
	}

	if(SORT_FLAG_ASCENDING & dwSortStat)
		return iResult;
	else
		return -iResult;
}

void TInstDlg::ProcessColumnClick(CONST HWND hWndList, CONST NMLISTVIEW * pnmlistview, DWORD& dwSortStatus)
{

	int iLastSort = dwSortStatus & 0xffff;
	int iSortFlag = dwSortStatus & 0xffff0000;

	if(pnmlistview->iSubItem != iLastSort) {
		dwSortStatus = (pnmlistview->iSubItem) | SORT_FLAG_ASCENDING;
	} else {
		if(0 != (SORT_FLAG_ASCENDING & dwSortStatus)) {
			dwSortStatus = (SORT_FLAG_DESCENDING | iLastSort);
		} else {
			dwSortStatus = (SORT_FLAG_ASCENDING | iLastSort);
		}
	}

	ListView_SortItems(pnmlistview->hdr.hwndFrom, ListViewCompareProc, (LPARAM)&dwSortStatus);

	HWND hwndHeader = ListView_GetHeader(hWndList);
	HDITEM hditem;
	memset(&hditem, 0, sizeof(HDITEM));
	hditem.mask = HDI_FORMAT;
	hditem.fmt = (dwSortStatus & SORT_FLAG_ASCENDING ? HDF_SORTUP : HDF_SORTDOWN) | HDF_STRING;
	Header_SetItem(hwndHeader, pnmlistview->iSubItem, &hditem);

	UINT columnCount = Header_GetItemCount(hwndHeader);
	hditem.fmt = HDF_STRING;
	for(UINT i = 0;i < columnCount;i++) {
		if(i == pnmlistview->iSubItem) continue;
		Header_SetItem(hwndHeader, i, &hditem);
	}
}

BOOL TInstDlg::ListView_BeginLabelEdit(const HWND hWndList, LPARAM lParam)
{

	if (NULL == lParam)return TRUE;

	const PCK_UNIFIED_FILE_ENTRY* lpFileEntry = (LPPCK_UNIFIED_FILE_ENTRY)lParam;

	size_t nAllowMaxLength;

	if(PCK_ENTRY_TYPE_INDEX == lpFileEntry->entryType) {
		nAllowMaxLength = MAX_PATH_PCK_256;
	} else {
		nAllowMaxLength = pck_getFilelenLeftBytesOfEntry( lpFileEntry) + pck_getFilelenBytesOfEntry(lpFileEntry);

	}

	HWND	hEdit;
	if(NULL == (hEdit = ListView_GetEditControl(hWndList)))return TRUE;
	::SendMessage(hEdit, EM_LIMITTEXT, nAllowMaxLength, 0);

	m_isListviewRenaming = TRUE;

	return FALSE;
}

BOOL TInstDlg::ListView_EndLabelEdit(const NMLVDISPINFO* pNmHdr)
{

	m_isListviewRenaming = FALSE;

	::SetWindowLong(pNmHdr->hdr.hwndFrom, GWL_STYLE, ::GetWindowLong(pNmHdr->hdr.hwndFrom, GWL_STYLE) & (LVS_EDITLABELS ^ 0xffffffff));

	if(NULL != pNmHdr->item.pszText) {
		if(0 == *pNmHdr->item.pszText)return FALSE;

		PCK_UNIFIED_FILE_ENTRY* lpFileEntry = (LPPCK_UNIFIED_FILE_ENTRY)pNmHdr->item.lParam;

		if (0 == wcscmp(lpFileEntry->szName, pNmHdr->item.pszText))
			return FALSE;

		const wchar_t*	lpszInvalid;
		if(PCK_ENTRY_TYPE_INDEX == lpFileEntry->entryType) {
			lpszInvalid = (wchar_t*)TEXT( TEXT_INVALID_PATHCHAR ) + 2;
		} else {
			lpszInvalid = TEXT( TEXT_INVALID_PATHCHAR );
		}

		while(0 != *lpszInvalid) {
			if(NULL != _tcschr(pNmHdr->item.pszText, *lpszInvalid)) {
				TCHAR szPrintf[64];
				_stprintf_s(szPrintf, 64, GetLoadStr(IDS_STRING_INVALIDFILENAME), lpszInvalid);
				MessageBox(szPrintf, TEXT("提示"), MB_OK | MB_ICONASTERISK);
				return FALSE;
			}

			lpszInvalid++;
		}

		if (WINPCK_OK != pck_RenameEntry(lpFileEntry, pNmHdr->item.pszText)) {
				MessageBox(GetLoadStr(IDS_STRING_RENAMEERROR), GetLoadStr(IDS_STRING_ERROR), MB_OK | MB_ICONERROR);
				OpenPckFile(m_Filename, TRUE);
				return FALSE;
		}

		//调用修改
		//pck_setMTMemoryUsed(m_PckHandle, 0);
		_beginthread(RenamePckFile, 0, this);

		return TRUE;
	}
	return FALSE;
}

#ifdef _USE_CUSTOMDRAW_
BOOL TInstDlg::EvDrawItemListView(const DRAWITEMSTRUCT *lpDis)
{
	HWND						hWndHeader;
	RECT						rc, rcfillrect;
	HDC							hdc;
	LVITEM						item;
	//LVCOLUMN					column;
	int							iColumnCount;
	TCHAR						szStrPrintf[MAX_PATH];

	//LPPCK_PATH_NODE				lpNodeToShow = NULL;
	/////////////////一些变量的初始化//////////////////
	//LPDRAWITEMSTRUCT lpDis = (LPDRAWITEMSTRUCT)lParam; 
	//lpDis->CtlID;					控件ID
	//lpDis->CtlType;				控件类型，ODT_LISTVIEW;ODT_HEADER;ODT_TAB;
	//lpDis->hDC;					控件HDC
	//lpDis->hwndItem;				控件HWND
	//lpDis->itemAction;			动作类型？？？
	//lpDis->itemData;				DATA，listview第0个subitem的字串
	//lpDis->itemID;				ListView第多少行的数据
	//lpDis->itemState;				状态，ODS_SELECTED（选中）ODS_FOCUS（）ODS_CHECKED（）
	//lpDis->rcItem;

	//hWndListView = lpDis->hwndItem;
	hWndHeader = ListView_GetHeader(lpDis->hwndItem);
	rcfillrect = lpDis->rcItem;
	hdc = lpDis->hDC;

	/////////////得到 ListView Header 的数量/////////////////
	iColumnCount = Header_GetItemCount(hWndHeader);////////////////此处有待优化

	/////////////////////////////////////////////////////
	if(lpDis->CtlType == ODT_LISTVIEW) {
		//if(ctlID == IDC_LIST_ICON) {
		//	Debug((WCHAR*)lpDis->itemData);
		//}

		item.mask = /* | LVIF_STATE |*/ /*LVIF_PARAM | */LVIF_IMAGE;
		item.iItem = lpDis->itemID;
		item.iSubItem = 0;
		//item.stateMask = 0x000c;		// get all state flags
		ListView_GetItem(lpDis->hwndItem, &item);

		//lpNodeToShow = (LPPCK_PATH_NODE)item.lParam;

		if(lpDis->itemState & ODS_SELECTED) {
			//if( lpDis->hwndItem == GetFocus() )
			redraw = R_SELECT;
			//else
			//	redraw=R_SEL_NOFOCUS;
		} else
			redraw = R_NORMAL;

		rcfillrect.right++;
		rcfillrect.bottom--;

		SetBkMode(hdc, TRANSPARENT);

		//rcfillrect.right-=20;
		if(R_NORMAL == redraw)
			FillRect(hdc, &rcfillrect, hBrushs[item.iItem & 1]);
		else
			FillRect(hdc, &rcfillrect, hBrushs[redraw]);

		//rcfillrect.right+=20;
		DrawIconEx(hdc, rcfillrect.left + 4, rcfillrect.top, hIconList[item.iImage], 16, 16, 0, 0, DI_NORMAL);

		rcfillrect.left += 6;

		item.cchTextMax = 256;
		item.pszText = szStrPrintf;

		for(item.iSubItem = 0; item.iSubItem < iColumnCount; item.iSubItem++) {

			rcfillrect.right = rcfillrect.left + ListView_GetColumnWidth(lpDis->hwndItem, item.iSubItem);	//2.984

			/*			column.mask = LVCF_WIDTH | LVCF_FMT;
			ListView_GetColumn(lpDis->hwndItem, item.iSubItem, &column);
			rcfillrect.right = rcfillrect.left + column.cx;	*/									//3.015

			rc = rcfillrect;
			rc.right -= 5;

			//修正位置
			//if(1 == lpDis->CtlID)
			//{
			if(0 == item.iSubItem)
				rc.left += 16;
			//}
			//int count = SendMessage(lpDis->hwndItem, LVM_GETITEMTEXT, (WPARAM)lpDis->itemID, (LPARAM)(LV_ITEM *)&item);
			//rcfillrect.right-=20;

			DrawText(hdc, szStrPrintf, ::SendMessage(lpDis->hwndItem, LVM_GETITEMTEXT, (WPARAM)lpDis->itemID, (LPARAM)&item), &rc, DT_SINGLELINE | fmtTextAlign[item.iSubItem] | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
			//rcfillrect.right+=20;
			rcfillrect.left = rcfillrect.right;
		}
		return FALSE;
	}
	return FALSE;
}
#endif
void TInstDlg::InsertList(CONST HWND hWndList, CONST INT iIndex, CONST UINT uiMask, CONST INT iImage, CONST LPVOID lParam, CONST INT nColCount, ...)
{

	if(0 == nColCount)return;

	LVITEMW	item;

	ZeroMemory(&item, sizeof(item));

	va_list	ap;
	va_start(ap, nColCount);

	item.iItem = iIndex;			//从0开始
	item.iImage = iImage;
	item.iSubItem = 0;
	item.mask = LVIF_TEXT | uiMask;
	item.pszText = va_arg(ap, wchar_t*);
	item.lParam = (LPARAM)lParam;

	//ListView_InsertItem(hWndList,&item);
	::SendMessageW(hWndList, LVM_INSERTITEMW, 0, (LPARAM)&item);

	item.mask = LVIF_TEXT;

	for(item.iSubItem = 1; item.iSubItem < nColCount; item.iSubItem++) {
		//item.iItem = iIndex;			//从0开始
		//item.iSubItem = i;
		item.pszText = va_arg(ap, wchar_t*);

		//item.pszText = _ultow(i, szID, 10);
		//ListView_SetItem(hWndList,&item);
		::SendMessageW(hWndList, LVM_SETITEMW, 0, (LPARAM)&item);
	}

	va_end(ap);
}

BOOL TInstDlg::InitListView(CONST HWND hWndListView, const LPTSTR *lpszText, const int *icx, const int *ifmt)
{

#ifndef _USE_CUSTOMDRAW_
	HICON hiconItem;     // icon for list-view items 
#endif

	HIMAGELIST hSmall;   // image list for other views 
	LVCOLUMN lvcolumn;

	//full row select
	ListView_SetExtendedListViewStyle(hWndListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	const int *lpicx = icx;
	const int *lpifmt = ifmt;

	lvcolumn.iSubItem = -1;
	while(-1 != *(lpicx)) {
		lvcolumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvcolumn.fmt = *(lpifmt++);
		lvcolumn.cx = *(lpicx++);
		lvcolumn.pszText = *(lpszText++);
		lvcolumn.iSubItem++;
		//lvcolumn.iOrder = 0;
		if(ListView_InsertColumn(hWndListView, lvcolumn.iSubItem, &lvcolumn) == -1)
			return FALSE;
	}

#ifdef _USE_CUSTOMDRAW_
	hSmall = ImageList_Create(16, 16, ILC_COLOR32, 1, 0);

#else
	hSmall = ImageList_Create(16, 16, ILC_COLOR32, 1, 2);

	hiconItem = LoadIcon(TApp::GetInstance(), MAKEINTRESOURCE(IDI_ICON_DIR));
	ImageList_AddIcon(hSmall, hiconItem);
	DestroyIcon(hiconItem);

	hiconItem = LoadIcon(TApp::GetInstance(), MAKEINTRESOURCE(IDI_ICON_FILE));
	ImageList_AddIcon(hSmall, hiconItem);
	//ImageList_AddIcon(hSmall, hiconItem); 
	DestroyIcon(hiconItem);
#endif
	ListView_SetImageList(hWndListView, hSmall, LVSIL_SMALL);

	return TRUE;
}

