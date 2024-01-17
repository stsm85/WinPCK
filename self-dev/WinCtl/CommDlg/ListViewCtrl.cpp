//#include "pch.h"
#include "ListViewCtrl.h"
CListViewCtrl::CListViewCtrl(HWND hWnd) : m_hWnd(hWnd)
{
}

CListViewCtrl::~CListViewCtrl()
{
}

#pragma region 初始化

BOOL CListViewCtrl::InitListView(std::vector<INIT_LIST_PARAMS<std::string>> cols, std::vector<int> icons)
{
	return InitLVColsTemplate<std::string, LVCOLUMNA, LVM_INSERTCOLUMNA>(cols, icons);
}

BOOL CListViewCtrl::InitListView(std::vector<INIT_LIST_PARAMS<std::wstring>> cols, std::vector<int> icons)
{
	return InitLVColsTemplate<std::wstring, LVCOLUMNW, LVM_INSERTCOLUMNW>(cols, icons);
}

template<typename T, typename T_LVCOL, int T_LVM_INSERTCOL>
BOOL CListViewCtrl::InitLVColsTemplate(std::vector<INIT_LIST_PARAMS<T>> cols, std::vector<int> icons)
{
	HICON hiconItem;     // icon for list-view items 
	T_LVCOL lvcolumn;

	//full row select
	ListView_SetExtendedListViewStyle(m_hWnd, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	lvcolumn.iSubItem = -1;

	for (auto& col : cols) {

		lvcolumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvcolumn.fmt = col.ifmt;
		lvcolumn.cx = col.cx;
		lvcolumn.pszText = (decltype(lvcolumn.pszText))col.colTitle.c_str();
		lvcolumn.iSubItem++;
		//lvcolumn.iOrder = 0;
		if (::SendMessage(m_hWnd, T_LVM_INSERTCOL, (WPARAM)(int)lvcolumn.iSubItem, (LPARAM)(const T_LVCOL*)(&lvcolumn)) == -1)
			throw std::exception("插入列失败");
	}

	if (0 != icons.size()) {

		m_hSmall = ImageList_Create(m_item_height, m_item_height, ILC_COLOR32, icons.size(), 4);

		for (auto icon : icons) {

			hiconItem = LoadIconA(m_hInstance, MAKEINTRESOURCEA(icon));
			ImageList_AddIcon(m_hSmall, hiconItem);
			DestroyIcon(hiconItem);
		}

		ListView_SetImageList(m_hWnd, m_hSmall, LVSIL_SMALL);
	}

	isInited = true;
	return TRUE;
}

#pragma endregion

#pragma region 添加数据

template<typename T>
void CListViewCtrl::AppendListWithImage(CONST INT iImage, CONST LPVOID lParam, std::vector<T> items) const
{
	AppendList(LVIF_IMAGE | LVIF_TEXT | LVIF_PARAM, iImage, lParam, items);
}
template void CListViewCtrl::AppendListWithImage<std::string>(CONST INT iImage, CONST LPVOID lParam, std::vector<std::string> items)const;
template void CListViewCtrl::AppendListWithImage<std::wstring>(CONST INT iImage, CONST LPVOID lParam, std::vector<std::wstring> items)const;


template<typename T>
void CListViewCtrl::AppendListWithOutImage(CONST LPVOID lParam, std::vector<T> items) const
{
	AppendList(LVIF_TEXT | LVIF_PARAM, 0, lParam, items);
}
template void CListViewCtrl::AppendListWithOutImage<std::string>(CONST LPVOID lParam, std::vector<std::string> items)const;
template void CListViewCtrl::AppendListWithOutImage<std::wstring>(CONST LPVOID lParam, std::vector<std::wstring> items)const;


void CListViewCtrl::AppendList(CONST UINT uiMask, CONST INT iImage, CONST LPVOID lParam, std::vector<std::string> items) const
{
	const int itemcount = ListView_GetItemCount(m_hWnd);

	InsertList<char, LVITEMA, LVM_INSERTITEMA, LVM_SETITEMA>(itemcount, uiMask, iImage, lParam, items);
}

void CListViewCtrl::AppendList(CONST UINT uiMask, CONST INT iImage, CONST LPVOID lParam, std::vector<std::wstring> items) const
{
	const int itemcount = ListView_GetItemCount(m_hWnd);

	InsertList<wchar_t, LVITEMW, LVM_INSERTITEMW, LVM_SETITEMW>(itemcount, uiMask, iImage, lParam, items);
}

template<typename T, typename T_LVITEM, int T_LVM_INSERTITEM, int T_LVM_SETITEM,
	template<typename VELEM, typename = std::char_traits<VELEM>, typename = std::allocator<VELEM>> class TSTRING>
void CListViewCtrl::InsertList(CONST INT iIndex, CONST UINT uiMask, CONST INT iImage, CONST LPVOID lParam, std::vector<TSTRING<T>>& items)const
{

	if (0 == items.size())
		return;

	T_LVITEM	item{
		.iItem = iIndex,
		.iImage = iImage,
		.lParam = (LPARAM)lParam
	};

	for (int i = 0; i < items.size(); i++)
	{
		item.iSubItem = i;
		item.mask = (i == 0) ? uiMask : LVIF_TEXT;
		item.pszText = (decltype(item.pszText))items[i].c_str();
		::SendMessage(m_hWnd, i == 0 ? T_LVM_INSERTITEM : T_LVM_SETITEM, 0, (LPARAM)&item);
	}
}

#pragma endregion


#pragma region 删除数据

void CListViewCtrl::DeleteItem(const int iItem)
{
	ListView_DeleteItem(m_hWnd, iItem);
}

void CListViewCtrl::DeleteAllItems()
{
	ListView_DeleteAllItems(m_hWnd);
}

#pragma endregion

#pragma region 读取参数

void CListViewCtrl::SetItemHeight(int cx)
{
	if (isInited)
		throw std::exception("SetItemHeight should be seted before InitListView");

	m_item_height = cx;
}

LPARAM CListViewCtrl::GetListParam(const int index)
{
	LVITEMA item{
		.mask = LVIF_PARAM,
		.iItem = index,
	};

	ListView_GetItem(m_hWnd, &item);
	return item.lParam;

}

int CListViewCtrl::GetListIcon(const int index)
{
	LVITEMA item{
	.mask = LVIF_IMAGE,
	.iItem = index,
	};

	ListView_GetItem(m_hWnd, &item);
	return item.iImage;
}

void CListViewCtrl::SetListIcon(const int index, const int iIcon)
{
	LVITEMA item{
	.mask = LVIF_IMAGE,
	.iItem = index,
	.iImage = iIcon,
	};

	ListView_SetItem(m_hWnd, &item);
}

int CListViewCtrl::GetItemCount()
{
	return ListView_GetItemCount(m_hWnd);
}

std::vector<int> CListViewCtrl::GetSelectedItem()
{
	std::vector<int> list;
	int nCount = GetItemCount();

	if (0 == nCount)
		return list;

	if (0 == ListView_GetSelectedCount(m_hWnd))
		return list;

	LVITEMA item{
		.mask = LVIF_STATE,
		.iItem = 0,
		.stateMask = LVIS_SELECTED,
	};

	for (int i = 0; i < nCount; i++) {

		item.iItem = i;
		ListView_GetItem(m_hWnd, &item);

		if (isSelectedStat(item.state)) {
			list.push_back(i);
		}
	}
	return list;
}


void CListViewCtrl::SetListText(const int iItem, std::vector<int> iSubItems, std::vector<std::string> items) const
{
	SetListItemText<char, LVITEMA, LVM_SETITEMA>(iItem, iSubItems, items);
}

void CListViewCtrl::SetListText(const int iItem, std::vector<int> iSubItems, std::vector<std::wstring> items) const
{
	SetListItemText<wchar_t, LVITEMW, LVM_SETITEMW>(iItem, iSubItems, items);
}


template<typename T, typename T_LVITEM, int T_LVM_SETITEM,
	template<typename VELEM, typename = std::char_traits<VELEM>, typename = std::allocator<VELEM>> class TSTRING>
void CListViewCtrl::SetListItemText(CONST INT iItem, std::vector<int> iSubItems, std::vector<TSTRING<T>>& items) const
{
	if (0 == items.size())
		return;
	//如果iSubItems为空，就依次set subitem

	T_LVITEM	item{
		.mask = LVIF_TEXT,
		.iItem = iItem,
	};

	if (0 == iSubItems.size()){

		for (int i = 0; i < items.size(); i++) {

			item.iSubItem = i;
			item.pszText = (decltype(item.pszText))items[i].c_str();
			::SendMessage(m_hWnd, T_LVM_SETITEM, 0, (LPARAM)&item);

		}
	}
	else {

		if (iSubItems.size() != items.size())
			return;

		for (int i = 0; i < iSubItems.size(); i++) {

			item.iSubItem = iSubItems[i];
			item.pszText = (decltype(item.pszText))items[i].c_str();
			::SendMessage(m_hWnd, T_LVM_SETITEM, 0, (LPARAM)&item);

		}
	}
}

#pragma endregion


#pragma region 判断属性

bool CListViewCtrl::isItemChangedToSelected(LPNMLISTVIEW pNMLV)
{
	return (isStatChanged(pNMLV->uChanged) && isSelectedStat(pNMLV->uNewState));
}

bool CListViewCtrl::isStatChanged(const int changed)
{
	return (LVIF_STATE == (LVIF_STATE & changed));
}

bool CListViewCtrl::isFocusedStat(const int stat)
{
	return (LVIS_FOCUSED == (LVIS_FOCUSED & stat));
}

bool CListViewCtrl::isSelectedStat(const int stat)
{
	return (LVIS_SELECTED == (LVIS_SELECTED & stat));
}


#pragma endregion