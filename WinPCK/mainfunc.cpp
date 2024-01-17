//////////////////////////////////////////////////////////////////////
// mainfunc.cpp: WinPCK 界面线程部分
// 打开并显示pck文件、查找文件、记录浏览位置 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.4.10
// 2012.10.10.OK
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4995 )
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4005 )

#include "guipch.h"
#include "winmain.h"

BOOL TInstDlg::NewPckFile()
{
	std::wstring Filename(this->m_Filename);

	try {

		std::wstring ext = ::MakeFileExtFilter<wchar_t>({ { L"PCK压缩文件(*.pck;*.zup)", L"*.pck;*.zup" }, { L"所有文件(*.*)", L"*.*" } });
		::OpenSingleFile<wchar_t>(this->hWnd, Filename, ext);
	}
	catch (std::exception) {

		this->SetStatusBarInfo(::GetLoadStrW(IDS_STRING_OPENFAIL));
		return FALSE;
	}

	this->m_Filename = std::move(fs::path(Filename));
	return this->OpenPckFile(FALSE);
}

BOOL TInstDlg::OpenPckFile(std::wstring sFileToOpen, BOOL isReOpen)
{
	if (0 == sFileToOpen.size())
		return this->NewPckFile();
	else {
		this->m_Filename = std::move(fs::absolute(fs::path(sFileToOpen)));
		return this->OpenPckFile(isReOpen);
	}
}

BOOL TInstDlg::OpenPckFile(BOOL isReOpen)
{
	int		iListTopView;

	this->m_currentNodeOnShow = nullptr;

	if(!isReOpen) {

		this->m_FolderBrowsed.clear();
	} else {
		//记录位置，目前选中的是第几行
		iListTopView = ListView_GetTopIndex(this->GetDlgItem(IDC_LIST));
	}

	CStopWatch	timer;
	timer.start();

	//转换文件名格式 
	if (WINPCK_OK == pck_open(this->m_Filename.wstring().c_str())) {
		//成功打开
		timer.stop();

		//显示打开消耗时间
		this->SetStatusBarInfo(std::vformat(::GetLoadStrW(IDS_STRING_OPENOK), std::make_wformat_args(timer.getElapsedTime())).c_str());

		//显示文件名
		this->SetStatusBarTitle(this->m_Filename.filename().wstring().c_str());
		this->ClearStatusBarProgress();

		this->SetStatusBarFileSize(pck_filesize());
		this->SetStatusBarFileCount(pck_filecount());

		if (isReOpen) {
			this->ShowPckFiles(pck_getFileEntryByPath(this->m_FolderBrowsed.c_str()));
			ListView_EnsureVisible(this->GetDlgItem(IDC_LIST), iListTopView, NULL);
			ListView_EnsureVisible(this->GetDlgItem(IDC_LIST), iListTopView + ListView_GetCountPerPage(this->GetDlgItem(IDC_LIST)) - 1, NULL);
		}
		else
			this->ShowPckFiles(pck_getRootNode());

		return TRUE;
	}
	else {
		this->SetStatusBarInfo(::GetLoadStrW(IDS_STRING_PROCESS_ERROR));
		pck_close();
		return FALSE;
	}
}

void ShowFilelistCallback(void* _in_param, int sn, const wchar_t *lpszFilename, int entry_type, unsigned __int64 qwFileSize, unsigned __int64 qwFileSizeCompressed, void* fileEntry)
{
	TInstDlg * const pThis = (TInstDlg*)_in_param;

	const HWND	hList = pThis->GetDlgItem(IDC_LIST);

	if (PCK_ENTRY_TYPE_DOTDOT != (PCK_ENTRY_TYPE_DOTDOT & entry_type)) {
		std::wstring	szCompressionRatio;

		if (0 == qwFileSize)
			szCompressionRatio.assign(L"-");
		else
			szCompressionRatio = std::format(L"{:.1f}%", qwFileSizeCompressed / (float)qwFileSize * 100.0);

		pThis->InsertList(hList, sn,
			LVIF_PARAM | LVIF_IMAGE, (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & entry_type)) ? IMGLIST_FOLDER : IMGLIST_FILE,
			fileEntry, 4,
			lpszFilename,
			::wbyte_format(qwFileSize).c_str(),
			::wbyte_format(qwFileSizeCompressed).c_str(),
			szCompressionRatio.c_str());

	}
	else {
		//PCK_ENTRY_TYPE_DOTDOT 时只能是文件夹
		pThis->InsertList(hList, sn,
			LVIF_PARAM | LVIF_IMAGE, IMGLIST_FOLDER,
			fileEntry, 1,
			lpszFilename
		);
	}
#ifdef _DEBUG
	OutputDebugStringW(std::format(
		L"{}\t{}\t{}\t{}\r\n", 
		lpszFilename,
		(PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & entry_type)) ? L"Folder" : L"File", 
		qwFileSize, 
		qwFileSizeCompressed).c_str()
	);
#endif

}

VOID TInstDlg::SearchPckFiles()
{
	if(0 == pck_filecount())
		return;

	HWND	hList = this->GetDlgItem(IDC_LIST);

	//显示查找文字
	this->SetStatusBarInfo(std::vformat(::GetLoadStrW(IDS_STRING_SEARCHING), std::make_wformat_args(this->m_szStrToSearch)).c_str());

	ListView_DeleteAllItems(hList);

	//清除浏览记录
	this->m_FolderBrowsed.clear();

	::SendMessage(hList, WM_SETREDRAW, FALSE, 0);
	auto dwFoundCount = pck_searchByName(this->m_szStrToSearch, this, ShowFilelistCallback);
	::SendMessage(hList, WM_SETREDRAW, TRUE, 0);

	this->SetStatusBarInfo(std::vformat(::GetLoadStrW(IDS_STRING_SEARCHOK), std::make_wformat_args(this->m_szStrToSearch, dwFoundCount)).c_str());

}

VOID TInstDlg::ShowPckFiles(const PCK_UNIFIED_FILE_ENTRY*	lpNodeToShow)
{
	HWND	hList = this->GetDlgItem(IDC_LIST);

	if (nullptr == lpNodeToShow) {
		this->MessageBoxW(L"Node Not Found!!!\r\nShow Root Node");
		lpNodeToShow = pck_getRootNode();
	}

	this->m_currentNodeOnShow = lpNodeToShow;

	ListView_DeleteAllItems(hList);

	::SendMessage(hList, WM_SETREDRAW, FALSE, 0);

	pck_listByNode(lpNodeToShow, this, ShowFilelistCallback);

	::SendMessage(hList, WM_SETREDRAW, TRUE, 0);

}
