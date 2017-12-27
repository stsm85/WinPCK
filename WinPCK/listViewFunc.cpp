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

#define MAX_COLUMN_COUNT		64

#define SORT_FLAG_ASCENDING		0x00010000
#define SORT_FLAG_DESCENDING	0x00020000
#define SORT_FLAG_SEARCHMODE	0x00040000

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
	//Debug(L"lv:LVN_FIRST-%d,all:NM_FIRST-%d\r\n", LVN_FIRST-pNmHdr->code, NM_FIRST - pNmHdr->code);

	switch(pNmHdr->code) {
	case LVN_COLUMNCLICK:
		ProcessColumnClick(((LPNMLISTVIEW)pNmHdr)->hdr.hwndFrom, (LPNMLISTVIEW)pNmHdr, dwSortStatus);
		break;
	case LVN_ITEMCHANGED:
	case NM_CLICK:
		if(-1 != iCurrentHotItem) {
			m_lpPckCenter->SetListCurrentHotItem(iCurrentHotItem);
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
	LPPCKINDEXTABLE		lpIndexToShow1, lpIndexToShow2;
	LPPCK_PATH_NODE		lpNodeToShow1, lpNodeToShow2;
	DWORD dwSortStat = *(DWORD*)lParamSort;
	int iLastSort = dwSortStat & 0xffff;
	int iResult;

	if(0 != (SORT_FLAG_SEARCHMODE & dwSortStat)) {
		lpIndexToShow1 = (LPPCKINDEXTABLE)lParam1;
		lpIndexToShow2 = (LPPCKINDEXTABLE)lParam2;
		//如果存在上级目录(".."目录)
		if((0 == *lpIndexToShow1->cFileIndex.szFilename) || (0 == *lpIndexToShow1->cFileIndex.szFilename))
			return 0;

		switch(iLastSort) {
			//文件名
		case 0:
			iResult = lstrcmpiA(lpIndexToShow1->cFileIndex.szFilename, lpIndexToShow2->cFileIndex.szFilename);
			break;
		case 1:
			iResult = lpIndexToShow1->cFileIndex.dwFileClearTextSize - lpIndexToShow2->cFileIndex.dwFileClearTextSize;
			break;
		case 2:
			iResult = lpIndexToShow1->cFileIndex.dwFileCipherTextSize - lpIndexToShow2->cFileIndex.dwFileCipherTextSize;
			break;
		case 3:
			float a, b;

			if(0 == lpIndexToShow1->cFileIndex.dwFileClearTextSize)
				a = 0.0;
			else
				a = (float)lpIndexToShow1->cFileIndex.dwFileCipherTextSize / (float)lpIndexToShow1->cFileIndex.dwFileClearTextSize;
			if(0 == lpIndexToShow2->cFileIndex.dwFileClearTextSize)
				b = 0.0;
			else
				b = (float)lpIndexToShow2->cFileIndex.dwFileCipherTextSize / (float)lpIndexToShow2->cFileIndex.dwFileClearTextSize;

			iResult = (int)((a > b) ? 1 : -1);
			break;
		}

	} else {
		lpNodeToShow1 = (LPPCK_PATH_NODE)lParam1;
		lpNodeToShow2 = (LPPCK_PATH_NODE)lParam2;

		if(lpNodeToShow1 && lpNodeToShow2) {

			//如果存在上级目录(".."目录)
			if((NULL != lpNodeToShow1->parent) || (NULL != lpNodeToShow2->parent))
				return 0;

			//如果存在目录和文件比较
			//NULL == lpNodeToShow->lpPckIndexTable -> 目录
			if((NULL == lpNodeToShow1->lpPckIndexTable ? 1 : 0) ^ (NULL == lpNodeToShow2->lpPckIndexTable ? 1 : 0))
				return 0;

			switch(iLastSort) {
				//文件名
			case 0:
				iResult = lstrcmpiA(lpNodeToShow1->szName, lpNodeToShow2->szName);
				break;
			case 1:
				if(NULL == lpNodeToShow1->lpPckIndexTable)
					iResult = lpNodeToShow1->child->qdwDirClearTextSize - lpNodeToShow2->child->qdwDirClearTextSize;
				else
					iResult = lpNodeToShow1->lpPckIndexTable->cFileIndex.dwFileClearTextSize - lpNodeToShow2->lpPckIndexTable->cFileIndex.dwFileClearTextSize;
				break;
			case 2:
				if(NULL == lpNodeToShow1->lpPckIndexTable)
					iResult = lpNodeToShow1->child->qdwDirCipherTextSize - lpNodeToShow2->child->qdwDirCipherTextSize;
				else
					iResult = lpNodeToShow1->lpPckIndexTable->cFileIndex.dwFileCipherTextSize - lpNodeToShow2->lpPckIndexTable->cFileIndex.dwFileCipherTextSize;
				break;
			case 3:
				float a, b;
				if(NULL == lpNodeToShow1->lpPckIndexTable) {
					if(0 == lpNodeToShow1->child->qdwDirClearTextSize)
						a = 0.0;
					else
						a = (float)lpNodeToShow1->child->qdwDirCipherTextSize / (float)lpNodeToShow1->child->qdwDirClearTextSize;
					if(0 == lpNodeToShow2->child->qdwDirClearTextSize)
						b = 0.0;
					else
						b = (float)lpNodeToShow2->child->qdwDirCipherTextSize / (float)lpNodeToShow2->child->qdwDirClearTextSize;
				} else {
					if(0 == lpNodeToShow1->lpPckIndexTable->cFileIndex.dwFileClearTextSize)
						a = 0.0;
					else
						a = (float)lpNodeToShow1->lpPckIndexTable->cFileIndex.dwFileCipherTextSize / (float)lpNodeToShow1->lpPckIndexTable->cFileIndex.dwFileClearTextSize;
					if(0 == lpNodeToShow2->lpPckIndexTable->cFileIndex.dwFileClearTextSize)
						b = 0.0;
					else
						b = (float)lpNodeToShow2->lpPckIndexTable->cFileIndex.dwFileCipherTextSize / (float)lpNodeToShow2->lpPckIndexTable->cFileIndex.dwFileClearTextSize;

				}
				iResult = (int)((a > b) ? 1 : -1);
				break;
			}

		}
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

	if(m_lpPckCenter->GetListInSearchMode()) {

		dwSortStatus |= SORT_FLAG_SEARCHMODE;
	} else {
		dwSortStatus &= (~SORT_FLAG_SEARCHMODE);
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
	LPPCK_PATH_NODE		lpNodeToShow;
	LPPCKINDEXTABLE		lpIndexToShow;

	if(NULL == lParam)return TRUE;
	//isSearchMode = 2 == ((NMLVDISPINFO*) pNmHdr)->item.iImage ? TRUE : FALSE;

	size_t nLen, nAllowMaxLength;

	if(m_lpPckCenter->GetListInSearchMode()) {
		lpIndexToShow = (LPPCKINDEXTABLE)lParam;
		nLen = strnlen(lpIndexToShow->cFileIndex.szFilename, MAX_PATH_PCK_260);
		//StringCchLengthA(lpIndexToShow->cFileIndex.szFilename, MAX_PATH_PCK_260, &nLen);
		nAllowMaxLength = MAX_PATH_PCK_260 - 2;
	} else {

		lpNodeToShow = (LPPCK_PATH_NODE)lParam;
		if(NULL == lpNodeToShow->child) {
			nLen = strnlen(lpNodeToShow->lpPckIndexTable->cFileIndex.szFilename, MAX_PATH_PCK_260);
			//StringCchLengthA(lpNodeToShow->lpPckIndexTable->cFileIndex.szFilename, MAX_PATH_PCK_260, &nLen);
			nAllowMaxLength = MAX_PATH_PCK_260 - nLen + strlen(lpNodeToShow->szName) - 2;
		} else {
			nAllowMaxLength = MAX_PATH_PCK_260 - 2;
		}
	}

	HWND	hEdit;
	if(NULL == (hEdit = ListView_GetEditControl(hWndList)))return TRUE;
	::SendMessage(hEdit, EM_LIMITTEXT, nAllowMaxLength, 0);

	return FALSE;
}

BOOL TInstDlg::ListView_EndLabelEdit(const NMLVDISPINFO* pNmHdr)
{
	LPPCK_PATH_NODE		lpNodeToShow;
	LPPCKINDEXTABLE		lpIndexToShow;

	::SetWindowLong(pNmHdr->hdr.hwndFrom, GWL_STYLE, ::GetWindowLong(pNmHdr->hdr.hwndFrom, GWL_STYLE) & (LVS_EDITLABELS ^ 0xffffffff));

	char	szEditedText[MAX_PATH_PCK_260];
	memset(szEditedText, 0, MAX_PATH_PCK_260);

	if(NULL != pNmHdr->item.pszText) {
		if(0 == *pNmHdr->item.pszText)return FALSE;

		//isSearchMode = 2 == ((NMLVDISPINFO*) pNmHdr)->item.iImage ? TRUE : FALSE;

		WideCharToMultiByte(CP_ACP, 0, pNmHdr->item.pszText, -1, szEditedText, MAX_PATH_PCK_260, "_", 0);

		if(m_lpPckCenter->GetListInSearchMode()) {
			lpIndexToShow = (LPPCKINDEXTABLE)pNmHdr->item.lParam;

			if(0 == strcmp(lpIndexToShow->cFileIndex.szFilename, szEditedText))
				return FALSE;
		} else {
			lpNodeToShow = (LPPCK_PATH_NODE)pNmHdr->item.lParam;

			if(0 == strcmp(lpNodeToShow->szName, szEditedText))
				return FALSE;
		}

		char*	lpszInvalid;
		if(m_lpPckCenter->GetListInSearchMode()) {
			lpszInvalid = (char*)TEXT_INVALID_PATHCHAR + 2;
		} else {
			lpszInvalid = TEXT_INVALID_PATHCHAR;
		}

		while(0 != *lpszInvalid) {
			if(NULL != strchr(szEditedText, *lpszInvalid)) {
				char szPrintf[64];
				sprintf_s(szPrintf, 64, GetLoadStrA(IDS_STRING_INVALIDFILENAME), lpszInvalid);
				MessageBoxA(szPrintf, "提示", MB_OK | MB_ICONASTERISK);
				return FALSE;
			}

			lpszInvalid++;
		}

		//size_t	nBytesAllowToWrite;
		if(m_lpPckCenter->GetListInSearchMode()) {
			m_lpPckCenter->RenameIndex(lpIndexToShow, szEditedText);
			//lpszInvalid = lpIndexToShow->cFileIndex.szFilename;
			//nBytesAllowToWrite = MAX_PATH_PCK;
		} else {
			if(NULL == lpNodeToShow->child)
				m_lpPckCenter->RenameIndex(lpNodeToShow, szEditedText);
			else {
				if(!m_lpPckCenter->RenameNode(lpNodeToShow, szEditedText)) {
					MessageBox(GetLoadStr(IDS_STRING_RENAMEERROR), GetLoadStr(IDS_STRING_ERROR), MB_OK | MB_ICONERROR);
					OpenPckFile(m_Filename, TRUE);
					return FALSE;
				}
			}
		}

		//调用修改
		//mt_MaxMemoryCount = 0;
		lpPckParams->cVarParams.dwMTMemoryUsed = 0;
		_beginthread(RenamePckFile, 0, this);

		return TRUE;
	}
	return FALSE;
}

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

void TInstDlg::InsertList(CONST HWND hWndList, CONST INT iIndex, CONST UINT uiMask, CONST INT iImage, CONST LPVOID lParam, CONST INT nColCount, ...)
{

	if(0 == nColCount)return;

	LVITEMA	item;

	ZeroMemory(&item, sizeof(LVITEMA));

	va_list	ap;
	va_start(ap, nColCount);

	item.iItem = iIndex;			//从0开始
	item.iImage = iImage;
	item.iSubItem = 0;
	item.mask = LVIF_TEXT | uiMask;
	item.pszText = va_arg(ap, char*);
	item.lParam = (LPARAM)lParam;

	//ListView_InsertItem(hWndList,&item);
	::SendMessageA(hWndList, LVM_INSERTITEMA, 0, (LPARAM)&item);

	item.mask = LVIF_TEXT;

	for(item.iSubItem = 1; item.iSubItem < nColCount; item.iSubItem++) {
		//item.iItem = iIndex;			//从0开始
		//item.iSubItem = i;
		item.pszText = va_arg(ap, char*);

		//item.pszText = _ultow(i, szID, 10);
		//ListView_SetItem(hWndList,&item);
		::SendMessageA(hWndList, LVM_SETITEMA, 0, (LPARAM)&item);
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

