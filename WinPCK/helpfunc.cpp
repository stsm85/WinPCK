//////////////////////////////////////////////////////////////////////
// helpfunc.cpp: WinPCK 界面线程部分
// 拖放、打开保存文件、预览
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4995 )
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4005 )

#include "guipch.h"
#include "winmain.h"
#include "tPreviewDlg.h"
#include "tAttrDlg.h"
#include "ShowLogOnDlgListView.h"

void TInstDlg::UnpackFiles(int id, auto func)
{
	if (pck_IsValidPck()) {
		if (pck_isThreadWorking()) {
			return;
		}
		else {
			try {
				CDlgOpenDirectory cDlgOpenDirectory;
				this->m_CurrentPath = cDlgOpenDirectory.Show(this->hWnd);
				_beginthread(func, 0, this);
			}
			catch (std::exception)
			{
				;
			}
		}
	}
}

void TInstDlg::UnpackAllFiles()
{
	this->UnpackFiles(ID_MENU_UNPACK_ALL, TInstDlg::ToExtractAllFiles);
}

void TInstDlg::UnpackSelectedFiles()
{
	this->UnpackFiles(ID_MENU_UNPACK_SELECTED, TInstDlg::ToExtractSelectedFiles);
}

const PCK_UNIFIED_FILE_ENTRY* TInstDlg::GetFileEntryByItem(int itemIndex)
{
	LVITEMW item {
		.mask = LVIF_PARAM,
		.iItem = itemIndex
	};

	ListView_GetItem(this->GetDlgItem(IDC_LIST), &item);
	return reinterpret_cast<const PCK_UNIFIED_FILE_ENTRY*>(item.lParam);
}

void TInstDlg::DbClickListView(const int itemIndex)
{

	this->m_iListHotItem = itemIndex;

	auto lpFileEntry = this->GetFileEntryByItem(itemIndex);

	if (nullptr == lpFileEntry)
		return;

	int entry_type = lpFileEntry->entryType;

	//列表是否是以搜索状态显示
	if(PCK_ENTRY_TYPE_INDEX == entry_type) {

		if(0 != itemIndex) {
			this->ViewFile(lpFileEntry);
			return;
		}
	}

	//目录浏览下root目录下的..不可点
	if ((PCK_ENTRY_TYPE_ROOT | PCK_ENTRY_TYPE_DOTDOT) == ((PCK_ENTRY_TYPE_ROOT | PCK_ENTRY_TYPE_DOTDOT) & entry_type)) {
		return;
	}

	//本级是否是文件夹(NULL=文件夹)
	if (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & entry_type)) {

		wchar_t szEntryName[300];
		this->ShowPckFiles(lpFileEntry);
		pck_getNodeRelativePath(szEntryName, lpFileEntry);
		this->m_FolderBrowsed.assign(szEntryName);

	}
	else {
		this->ViewFile(lpFileEntry);
	}
}

void TInstDlg::PopupRightMenu(const int itemIndex)
{

	auto lpFileEntry = this->GetFileEntryByItem(itemIndex);

	this->m_iListHotItem = itemIndex;

	HMENU hMenuRClick = ::GetSubMenu(::LoadMenuW(TApp::GetInstance(), MAKEINTRESOURCEW(IDR_MENU_RCLICK)), 0);

	if(PCK_ENTRY_TYPE_INDEX != lpFileEntry->entryType) {

		if(NULL == lpFileEntry || pck_isThreadWorking()) {
			::EnableMenuItem(hMenuRClick, ID_MENU_VIEW, MF_GRAYED);
			::EnableMenuItem(hMenuRClick, ID_MENU_RENAME, MF_GRAYED);
			::EnableMenuItem(hMenuRClick, ID_MENU_DELETE, MF_GRAYED);
			::EnableMenuItem(hMenuRClick, ID_MENU_UNPACK_SELECTED, MF_GRAYED);

		} else {
			::EnableMenuItem(hMenuRClick, ID_MENU_VIEW, PCK_ENTRY_TYPE_FOLDER != (PCK_ENTRY_TYPE_FOLDER & lpFileEntry->entryType) ? MF_ENABLED : MF_GRAYED);
			::EnableMenuItem(hMenuRClick, ID_MENU_RENAME, 0 != itemIndex ? MF_ENABLED : MF_GRAYED);
			::EnableMenuItem(hMenuRClick, ID_MENU_DELETE, 0 != itemIndex ? MF_ENABLED : MF_GRAYED);
			::EnableMenuItem(hMenuRClick, ID_MENU_UNPACK_SELECTED, 0 != itemIndex ? MF_ENABLED : MF_GRAYED);

		}

	} else {
		::EnableMenuItem(hMenuRClick, ID_MENU_VIEW, 0 != itemIndex ? MF_ENABLED : MF_GRAYED);
		::EnableMenuItem(hMenuRClick, ID_MENU_RENAME, 0 != itemIndex ? MF_ENABLED : MF_GRAYED);
		::EnableMenuItem(hMenuRClick, ID_MENU_DELETE, 0 != itemIndex ? MF_ENABLED : MF_GRAYED);
		::EnableMenuItem(hMenuRClick, ID_MENU_UNPACK_SELECTED, 0 != itemIndex ? MF_ENABLED : MF_GRAYED);

	}

	::EnableMenuItem(hMenuRClick, ID_MENU_ATTR, 0 != itemIndex ? MF_ENABLED : MF_GRAYED);

	::TrackPopupMenu(hMenuRClick, TPM_LEFTALIGN, LOWORD(::GetMessagePos()), HIWORD(::GetMessagePos()), 0, hWnd, NULL);
}

VOID TInstDlg::ViewFileAttribute()
{
	if (0 == this->m_iListHotItem)
		return;

	if(pck_isThreadWorking())
		return;

	auto lpFileEntry = this->GetFileEntryByItem(this->m_iListHotItem);

	if(pck_IsValidPck()) {
		wchar_t	szPath[MAX_PATH_PCK_260];

		pck_getNodeRelativePath(szPath, this->m_currentNodeOnShow);

		TAttrDlg	dlg(lpFileEntry, szPath, this);
		dlg.Exec();
	}
}


VOID TInstDlg::ViewFile(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry)
{
	if(pck_isThreadWorking())
		return;

	CPriviewInDlg cPreview;
	cPreview.Show(lpFileEntry, this);
}

BOOL TInstDlg::AddFiles()
{

	if(pck_isThreadWorking())
		return FALSE;

	if(IDCANCEL == this->MessageBoxW(L"确定添加文件吗？", L"询问", MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2))return FALSE;

	if(::OpenFiles(this->hWnd, this->m_lpszFilePath)) {

		::DragAcceptFiles(this->hWnd, FALSE);
		_beginthread(UpdatePckFile, 0, this);
	}

	return FALSE;
}

int TInstDlg::MyFeedbackCallback(void* pTag, int32_t eventId, size_t wParam, ssize_t lParam)
{
	auto pThis = reinterpret_cast<TInstDlg*>(pTag);

	switch (eventId)
	{
	case PCK_FILE_OPEN_SUCESS:

		pThis->SetWindowTextW(std::format(L"{} - {}", THIS_MAIN_CAPTION, reinterpret_cast<const wchar_t*>(lParam)).c_str());
		break;

	case PCK_FILE_CLOSE:

		pThis->SetWindowTextW(THIS_MAIN_CAPTION);
		break;
	}

	return 0;
}

void TInstDlg::SetupRegAsso(bool isInstall) const
{
	constexpr struct REG_ASSO_INFO {
		const wchar_t* ext;
		const wchar_t* dir;
		const wchar_t* desc;
		const wchar_t* menustr;
	} pck_assos[] = {
		{L".pck", L"pckfile",		L"Angelica File Package",	L"使用 WinPCK 打开"},
		{L".zup", L"ZPWUpdatePack", L"诛仙更新包",				L"使用 WinPCK 打开"},
	};

	const auto& check_installed = []() {
		HKEY	hRegKey;
		if (ERROR_SUCCESS == ::RegOpenKeyExW(HKEY_CLASSES_ROOT,
			L"pckfile\\shell\\open\\command",
			0,
			KEY_READ,
			&hRegKey)) {
			//存在
			//1.如果程序包含patcher.exe，新加
			//result = RegQueryValueEx(hRegKey, NULL, NULL, &dwType, reinterpret_cast<LPBYTE>(szString), &dwDataLength)
			//2.否则替换
			//3.修改打开方式索引
			::RegCloseKey(hRegKey);
			return true;
		}
		return false;
	};

	const auto& unistall_reg_asso = [](auto& pck_asso) {

		const auto& RecurseDeleteKey = [](HKEY hRegKey, const wchar_t* lpszKey)
		{
			const auto& recurse_deleteKey = [](auto&& self, HKEY hRegKey, const wchar_t* lpszKey)
			{
				constexpr auto reg_name_size = 1024;
				HKEY hSubRegKey;
				LONG lRes = ::RegOpenKeyExW(hRegKey, lpszKey, 0, KEY_READ | KEY_WRITE, &hSubRegKey);
				if (lRes != ERROR_SUCCESS) {
					return lRes;
				}

				FILETIME time;
				DWORD dwSize = reg_name_size;
				wchar_t szBuffer[reg_name_size];
				while (::RegEnumKeyExW(hSubRegKey, 0, szBuffer, &dwSize, NULL, NULL, NULL,
					&time) == ERROR_SUCCESS) {
					lRes = self(self, hSubRegKey, szBuffer);
					if (lRes != ERROR_SUCCESS)
						return lRes;
					dwSize = reg_name_size;
				}

				::RegCloseKey(hSubRegKey);
				return ::RegDeleteKeyW(hRegKey, lpszKey);
			};

			recurse_deleteKey(recurse_deleteKey, hRegKey, lpszKey);
		};

		RecurseDeleteKey(HKEY_CLASSES_ROOT, pck_asso.ext);
		RecurseDeleteKey(HKEY_CLASSES_ROOT, pck_asso.dir);
	};

	auto const& install_reg_asso = [](auto& pck_asso, auto& icon, auto& exec) {

		const auto& CreateAndSetDefaultValue = [](const std::wstring& pszValueName, const std::wstring& pszValue)
		{
			HKEY	hRegKey;

			if (ERROR_SUCCESS == ::RegCreateKeyExW(HKEY_CLASSES_ROOT,
				pszValueName.c_str(),
				0,
				REG_NONE,
				REG_OPTION_NON_VOLATILE,
				KEY_READ | KEY_WRITE,
				NULL,
				&hRegKey,
				NULL)) {
				if (0 != pszValue.size())
					::RegSetValueExW(hRegKey, NULL, NULL, REG_SZ, reinterpret_cast<const BYTE*>(pszValue.data()), (pszValue.size() + 1) * sizeof(std::wstring::value_type));

				::RegCloseKey(hRegKey);
			}

		};

		std::wstring dir = pck_asso.dir;

		CreateAndSetDefaultValue(pck_asso.ext, dir);
		CreateAndSetDefaultValue(dir, pck_asso.desc);
		CreateAndSetDefaultValue(dir.append(L"\\DefaultIcon"), icon);
		CreateAndSetDefaultValue(dir.append(L"\\shell"), L"");
		CreateAndSetDefaultValue(dir.append(L"\\shell\\open"), pck_asso.menustr);
		CreateAndSetDefaultValue(dir.append(L"\\shell\\open\\command"), exec);
	};

	if (isInstall) {
		auto szStringIcon = this->m_MyFileName + L",0";
		auto szStringExec = L"\"" + this->m_MyFileName + LR"(" "%1")";

		if (check_installed())
			for (auto& asso : pck_assos)
				unistall_reg_asso(asso);

		for (auto& asso : pck_assos)
			install_reg_asso(asso, szStringIcon, szStringExec);
	}
	else {

		for (auto& asso : pck_assos)
			unistall_reg_asso(asso);
	}
}

void TInstDlg::AddSetupReg()
{
	this->SetupRegAsso(true);
	this->MessageBoxW(L"安装完成。", L"信息", MB_OK | MB_ICONASTERISK);
}

void TInstDlg::DeleteSetupReg()
{
	this->SetupRegAsso(false);
	this->MessageBoxW(L"卸载完成。", L"信息", MB_OK | MB_ICONASTERISK);
}

void TInstDlg::InitLogWindow()
{

	this->m_logdlg.Create();
#if 0
	//绑定函数
	LogUnits.setInsertLogFunc(std::bind(&TLogDlg::InsertLogToList, &this->m_logdlg, std::placeholders::_1, std::placeholders::_2));
	LogUnits.setSetStatusBarInfoFunc(std::bind(&TInstDlg::SetStatusBarInfo, this, std::placeholders::_1));

	//日志函数绑定
	log_regShowFunc(PreInsertLogToList);

	//启动日志
	pck_logIW(THIS_MAIN_CAPTION " is started.");
#endif
}


void TInstDlg::RefreshProgress()
{
	std::wstring szTimerProcessingFormatString = ::GetLoadStrW(IDS_STRING_TIMERING);

	auto const 	dwUIProgress = pck_getUIProgress();
	auto	 	dwUIProgressUpper = pck_getUIProgressUpper();
	auto const 	dwMTMemoryUsed = pck_getMTMemoryUsed();
	auto const 	dwMTMaxMemory = pck_getMTMaxMemory();

	if(0 == dwUIProgressUpper)
		dwUIProgressUpper = 1;

	auto iNewPos = (int)((dwUIProgress << 10) / dwUIProgressUpper);

	SendDlgItemMessage(IDC_PROGRESS, PBM_SETPOS, (WPARAM)iNewPos, (LPARAM)0);

	if(nullptr != this->m_pTaskBarlist)
		this->m_pTaskBarlist->SetProgressValue(hWnd, dwUIProgress, dwUIProgressUpper);

	this->SetStatusBarProgress(std::vformat(
		szTimerProcessingFormatString,
		std::make_wformat_args(dwUIProgress,
		dwUIProgressUpper,
		dwUIProgress * 100.0 / dwUIProgressUpper,
		::wbyte_format(dwMTMemoryUsed),
		::wbyte_format(dwMTMaxMemory),
		(dwMTMemoryUsed >> 10) * 100.0 / (dwMTMaxMemory >> 10))).c_str()
	);
}

std::wstring TInstDlg::BuildSaveDlgFilterString()
{
	//static std::wstring szSaveDlgFilterString = []() {

		std::wstring filter_str;
		const auto nPckVersionCount = pck_getVersionCount();

		for (int i = 0; i < nPckVersionCount; i++)
			filter_str.append(std::format(L"{} PCK文件(*.pck)|*.pck|", pck_getVersionNameById(i)));

		filter_str.append(L"|");

		for (auto& c : filter_str)
			if ('|' == c)
				c = 0;

		return filter_str;
	//}();

	//return szSaveDlgFilterString;
}