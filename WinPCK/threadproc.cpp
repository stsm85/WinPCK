//////////////////////////////////////////////////////////////////////
// threadproc.cpp: WinPCK 界面线程部分
// 压缩、解压、更新等大用时过程，需多线程调用 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.4.10
// 2012.10.10
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4244 )
#pragma warning ( disable : 4311 )
//#pragma warning ( disable : 4018 )
#pragma warning ( disable : 4005 )

#include "guipch.h"
#include "winmain.h"
#include "tRebuildOptDlg.h"
#include "tStripDlg.h"

fs::path GetPckFileNameBySource(fs::path src, BOOL isDirectory);

VOID TInstDlg::UpdatePckFile(VOID *pParam)
{

	TInstDlg	*pThis = (TInstDlg*)pParam;
	BOOL		bHasPckOpened = !pck_IsValidPck();
	std::wstring		szFilenameToSave;
	CStopWatch	timer;

	if (bHasPckOpened) {
		//此时没有打开文件，这时的操作相当于新建文档
		pThis->m_currentNodeOnShow = NULL;

		if (1 == pThis->m_lpszFilePath.size()) {
			szFilenameToSave = GetPckFileNameBySource(pThis->m_lpszFilePath[0], FALSE).c_str();
		}

		try {
			//选择保存的文件名
			int nSelectFilter = ::SaveFile<wchar_t>(pThis->hWnd, szFilenameToSave, std::wstring(L"pck"), pThis->BuildSaveDlgFilterString());

			//设定目标pck的版本
			if (WINPCK_OK != pck_setVersion(nSelectFilter))
				return;
		}
		catch (std::exception)
		{
			pck_close();
			return;
		}

	}
	else {
		szFilenameToSave.assign(pThis->m_Filename);
	}

	//开始计时
	timer.start();

	pThis->EnbaleButtons(FALSE);

	fs::path filepath(szFilenameToSave);

	pThis->SetStatusBarInfo(std::vformat(::GetLoadStrW(IDS_STRING_RENEWING), std::make_wformat_args(filepath.filename().wstring())).c_str());

	pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

	pck_StringArrayReset();
	for (int i = 0; i < pThis->m_lpszFilePath.size(); i++) {
		pck_StringArrayAppend(pThis->m_lpszFilePath[i].c_str());
	}

	if (WINPCK_OK == pck_UpdatePckFileSubmit(szFilenameToSave.c_str(), pThis->m_currentNodeOnShow)) {

		//计时结束
		timer.stop();

		if (pck_isLastOptSuccess()) {

			pck_logN(::GetLoadStrW(IDS_STRING_RENEWOK), timer.getElapsedTime());
		}
		else {
			pThis->SetStatusBarInfo(::GetLoadStrW(IDS_STRING_PROCESS_ERROR));
		}


		//整合报告
		// 打印报告
		// pck包中原有文件 %d 个\r\n
		// 新加入文件 %d 个，其中重复文件名 %d 个\r\n
		// 使用原数据地址 %d 个，新数据地址 %d 个\r\n
		// 通过成的新pck文件共 %d 个文件\r\n

		if (0 != pck_getUpdateResult_PrepareToAddFileCount()) {

			auto sUpdateDetail = std::format(
				L"此更新过程数据如下：\r\n"
				L"PCK 包中原有文件数： {}\r\n"
				L"计划更新文件数： {}\r\n"
				L"实际更新文件数： {}\r\n"
				L"重名文件数： {}\r\n"
				L"未更新文件数： {}\r\n"
				L"更新后 PCK 包中文件数： {}",
				pck_getUpdateResult_OldFileCount(),
				pck_getUpdateResult_PrepareToAddFileCount(),
				pck_getUpdateResult_ChangedFileCount(),
				pck_getUpdateResult_DuplicateFileCount(),
				pck_getUpdateResult_PrepareToAddFileCount() - pck_getUpdateResult_ChangedFileCount(),
				pck_getUpdateResult_FinalFileCount()
			);

			pThis->MessageBoxW(sUpdateDetail.c_str(), L"更新报告", MB_OK | MB_ICONINFORMATION);

			pck_logI(sUpdateDetail.c_str());
		}


		if (bHasPckOpened) {

			pThis->OpenPckFile(szFilenameToSave.c_str(), TRUE);
		}
		else {
			pThis->OpenPckFile(TRUE);
		}

	}
	else {

		pThis->SetStatusBarInfo(GetLoadStr(IDS_STRING_PROCESS_ERROR));
		if (bHasPckOpened) {
			pck_close();
		}
	}

	pThis->EnbaleButtons(TRUE);

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if ((!(pck_isLastOptSuccess())) && pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	//还原Drop状态
	pThis->m_lpszFilePath.clear();
	DragAcceptFiles(pThis->hWnd, TRUE);

	return;

}

VOID TInstDlg::RenamePckFile(VOID *pParam)
{

	TInstDlg	*pThis = (TInstDlg*)pParam;
	CStopWatch	timer;
	//开始计时
	timer.start();

	pThis->EnbaleButtons(FALSE);

	pThis->SetStatusBarInfo(std::vformat(::GetLoadStrW(IDS_STRING_RENEWING), std::make_wformat_args(pThis->m_Filename.filename().wstring())).c_str());

	pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

	if (WINPCK_OK == pck_RenameSubmit()) {

		//计时结束
		timer.stop();

		pThis->SetStatusBarInfo(std::vformat(::GetLoadStrW(IDS_STRING_RENEWOK), std::make_wformat_args(timer.getElapsedTime())).c_str());

		pThis->OpenPckFile(TRUE);

	}
	else {
		pThis->SetStatusBarInfo(::GetLoadStrW(IDS_STRING_PROCESS_ERROR));
		pck_close();
	}

	pThis->EnbaleButtons(TRUE);

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if (pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	return;
}

VOID TInstDlg::RebuildPckFile(VOID	*pParam)
{
	try {

		TInstDlg* pThis = (TInstDlg*)pParam;

		BOOL		bNeedCreatePck = !pck_IsValidPck();

		TCHAR		szScriptFile[MAX_PATH];
		CStopWatch	timer;

		if (bNeedCreatePck) {
			if (!pThis->NewPckFile()) {
				return;
			}
		}

		fs::path filepath(pThis->m_Filename);
		filepath.replace_filename(L"Rebuild_" + filepath.filename().wstring());

		std::wstring szFilenameToSave(filepath.wstring());

		//弹出选项对话框
		//调用对话框
		BOOL  bNeedRecompress;
		TRebuildOptDlg	dlg(szScriptFile, &bNeedRecompress, pThis);
		if (IDCANCEL == dlg.Exec())
			return;

		//选择保存的文件名
		int nSelectFilter = ::SaveFile<wchar_t>(pThis->hWnd, szFilenameToSave, std::wstring(L"pck"), pThis->BuildSaveDlgFilterString(), pck_getVersion());

		if (0 > nSelectFilter) {
			return;
		}

		if (WINPCK_OK != pck_setVersion(nSelectFilter))
			return;

		//开始计时
		timer.start();

		pThis->EnbaleButtons(FALSE);

		filepath = fs::path(szFilenameToSave);

		pThis->SetStatusBarInfo(std::vformat(::GetLoadStrW(IDS_STRING_REBUILDING), std::make_wformat_args(filepath.filename().wstring())).c_str());

		pThis->SetTimer(WM_TIMER_PROGRESS_100, 100, NULL);

		if (WINPCK_OK == pck_RebuildPckFileWithScript(szScriptFile, szFilenameToSave.c_str(), bNeedRecompress)) {

			//计时结束
			timer.stop();

			if (pck_isLastOptSuccess()) {

				pck_logN(GetLoadStr(IDS_STRING_REBUILDOK), timer.getElapsedTime());
			}
			else {
				pThis->SetStatusBarInfo(GetLoadStr(IDS_STRING_PROCESS_ERROR));
			}

		}
		else {
			pThis->SetStatusBarInfo(GetLoadStr(IDS_STRING_PROCESS_ERROR));
		}

		pThis->EnbaleButtons(TRUE);

		if (bNeedCreatePck) {
			ListView_DeleteAllItems(pThis->GetDlgItem(IDC_LIST));
			pck_close();
		}

		pThis->KillTimer(WM_TIMER_PROGRESS_100);
		pThis->RefreshProgress();

		if ((!(pck_isLastOptSuccess())) && pThis->bGoingToExit) {
			pThis->bGoingToExit = FALSE;
			pThis->SendMessage(WM_CLOSE, 0, 0);
		}
	}
	catch (std::exception ex)
	{
		pck_logIA(ex.what());
	}

	return;
}

VOID TInstDlg::StripPckFile(VOID *pParam)
{

	TInstDlg	*pThis = (TInstDlg*)pParam;
	BOOL		bNeedCreatePck = !pck_IsValidPck();
	CStopWatch	timer;

	if (bNeedCreatePck) {
		if (!pThis->NewPckFile()) {
			return;
		}
	}

	fs::path filepath(pThis->m_Filename);
	filepath.replace_filename(L"Striped_" + filepath.filename().wstring());

	std::wstring szFilenameToSave(filepath.wstring());

	//弹出选项对话框
	//调用对话框
	int stripFlag;
	TStripDlg	dlg(&stripFlag, pThis);
	if (IDCANCEL == dlg.Exec())
		return;

	try {
		//选择保存的文件名
		int nSelectFilter = ::SaveFile<wchar_t>(pThis->hWnd, szFilenameToSave, std::wstring(L"pck"), pThis->BuildSaveDlgFilterString(), pck_getVersion());

		if (WINPCK_OK != pck_setVersion(nSelectFilter))
			return;
	}
	catch (std::exception)
	{
		return;
	}

	//开始计时
	timer.start();

	pThis->EnbaleButtons(FALSE);

	filepath = fs::path(szFilenameToSave);
	pThis->SetStatusBarInfo(std::vformat(::GetLoadStrW(IDS_STRING_REBUILDING), std::make_wformat_args(filepath.filename().wstring())).c_str());

	pThis->SetTimer(WM_TIMER_PROGRESS_100, 100, NULL);

	if (WINPCK_OK == pck_StripPck(szFilenameToSave.c_str(), stripFlag)) {

		//计时结束
		timer.stop();

		if (pck_isLastOptSuccess()) {

			pck_logN(GetLoadStr(IDS_STRING_REBUILDOK), timer.getElapsedTime());
		}
		else {
			pThis->SetStatusBarInfo(GetLoadStr(IDS_STRING_PROCESS_ERROR));
		}

	}
	else {
		pThis->SetStatusBarInfo(GetLoadStr(IDS_STRING_PROCESS_ERROR));
	}

	pThis->EnbaleButtons(TRUE);

	if (bNeedCreatePck) {
		ListView_DeleteAllItems(pThis->GetDlgItem(IDC_LIST));
		pck_close();
	}

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if ((!(pck_isLastOptSuccess())) && pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}
	return;
}

VOID TInstDlg::CreateNewPckFile(VOID	*pParam)
{
	TInstDlg	*pThis = (TInstDlg*)pParam;

	BOOL		isNotOpenedPck = !pck_IsValidPck();

	CStopWatch	timer;

	try {
		//选择目录
		CDlgOpenDirectory cDlgOpenDirectory;
		pThis->m_CurrentPath = cDlgOpenDirectory.Show(pThis->hWnd);
	}
	catch (std::exception) {
		return;
	}

	pThis->m_lpszFilePath.push_back(pThis->m_CurrentPath);

	auto szFilenameToSave = GetPckFileNameBySource(pThis->m_CurrentPath, TRUE).wstring();

	try {
		//选择保存的文件名
		int nSelectFilter = ::SaveFile<wchar_t>(pThis->hWnd, szFilenameToSave, std::wstring(L"pck"), pThis->BuildSaveDlgFilterString());

		//设定目标pck的版本
		if (WINPCK_OK != pck_setVersion(nSelectFilter))
			return;
	}
	catch (std::exception) {
		return;
	}

	//开始计时
	timer.start();

	pThis->EnbaleButtons(FALSE);

	fs::path filenameToSave(szFilenameToSave);

	pThis->SetStatusBarInfo(std::vformat(::GetLoadStrW(IDS_STRING_COMPING), std::make_wformat_args( filenameToSave.filename().wstring())).c_str());

	pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

	pck_StringArrayReset();
	for (int i = 0; i < pThis->m_lpszFilePath.size(); i++) {
		pck_StringArrayAppend(pThis->m_lpszFilePath[i].c_str());
	}

	if (WINPCK_OK == pck_UpdatePckFileSubmit(szFilenameToSave.c_str(), pThis->m_currentNodeOnShow)) {

		//计时结束
		timer.stop();

		if (pck_isLastOptSuccess()) {
			pck_logN(GetLoadStr(IDS_STRING_REBUILDOK), timer.getElapsedTime());
		}
		else {
			pThis->SetStatusBarInfo(GetLoadStrW(IDS_STRING_PROCESS_ERROR));
		}

	}
	else {
		pThis->SetStatusBarInfo(GetLoadStrW(IDS_STRING_PROCESS_ERROR));
	}

	pThis->EnbaleButtons(TRUE);

	if (isNotOpenedPck)
		pck_close();

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if ((!(pck_isLastOptSuccess())) && pThis->bGoingToExit) {

		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}
	return;
}

VOID TInstDlg::ToExtractAllFiles(VOID	*pParam)
{
	TInstDlg	*pThis = (TInstDlg*)pParam;

	CStopWatch	timer;

	//开始计时
	timer.start();

	pThis->EnbaleButtons(FALSE);

	fs::path filepath(pThis->m_Filename);
	pThis->SetStatusBarInfo(std::vformat(::GetLoadStrW(IDS_STRING_EXPING), std::make_wformat_args(filepath.filename().wstring())).c_str());

	pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

	if (WINPCK_OK == pck_ExtractAllFiles(pThis->m_CurrentPath.c_str())) {
		//计时结束
		timer.stop();
		pThis->SetStatusBarInfo(std::vformat(::GetLoadStrW(IDS_STRING_EXPOK), std::make_wformat_args(timer.getElapsedTime())).c_str());

	}
	else {

		pThis->SetStatusBarInfo(GetLoadStr(IDS_STRING_PROCESS_ERROR));
	}

	pThis->EnbaleButtons(TRUE);

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if ((!(pck_isLastOptSuccess())) && pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	return;
}

VOID TInstDlg::ToExtractSelectedFiles(VOID	*pParam)
{
	TInstDlg	*pThis = (TInstDlg*)pParam;

	HWND	hList = pThis->GetDlgItem(IDC_LIST);

	CStopWatch	timer;

	const PCK_UNIFIED_FILE_ENTRY **lpFileEntryArray, **lpFileEntryArrayPtr;

	UINT uiSelectCount = ListView_GetSelectedCount(hList);

	if (0 != uiSelectCount) {
		if (NULL != (lpFileEntryArray = (const PCK_UNIFIED_FILE_ENTRY **)malloc(sizeof(PCK_UNIFIED_FILE_ENTRY *) * uiSelectCount))) {

			//取lpNodeToShow
			int	nCurrentItemCount = ListView_GetItemCount(hList);

			LVITEM item;
			item.mask = LVIF_PARAM | LVIF_STATE;
			item.iSubItem = 0;
			item.stateMask = LVIS_SELECTED;		// get all state flags

			lpFileEntryArrayPtr = lpFileEntryArray;

			uiSelectCount = 0;

			//从1开始，跳过..目录
			for (item.iItem = 1; item.iItem < nCurrentItemCount; item.iItem++) {
				ListView_GetItem(hList, &item);

				if (item.state & LVIS_SELECTED) {
					*lpFileEntryArrayPtr = (LPPCK_UNIFIED_FILE_ENTRY)item.lParam;
					lpFileEntryArrayPtr++;

					uiSelectCount++;
				}
			}

			if (0 == uiSelectCount)return;

			//开始计时
			timer.start();

			pThis->EnbaleButtons(FALSE);

			fs::path filepath(pThis->m_Filename);
			pThis->SetStatusBarInfo(std::vformat(::GetLoadStrW(IDS_STRING_EXPING), std::make_wformat_args(filepath.filename().wstring())).c_str());

			pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

			lpFileEntryArrayPtr = lpFileEntryArray;

			if (WINPCK_OK == pck_ExtractFilesByEntrys(lpFileEntryArray, uiSelectCount, pThis->m_CurrentPath.c_str())) {
				//计时结束
				timer.stop();
				pThis->SetStatusBarInfo(std::vformat(::GetLoadStrW(IDS_STRING_EXPOK), std::make_wformat_args(timer.getElapsedTime())).c_str());
			}
			else {
				pThis->SetStatusBarInfo(GetLoadStr(IDS_STRING_PROCESS_ERROR));
			}

			free(lpFileEntryArray);

			pThis->EnbaleButtons(TRUE);

			pThis->KillTimer(WM_TIMER_PROGRESS_100);
			pThis->RefreshProgress();

			if ((!(pck_isLastOptSuccess())) && pThis->bGoingToExit) {
				pThis->bGoingToExit = FALSE;
				pThis->SendMessage(WM_CLOSE, 0, 0);
			}
		}
	}
	return;
}


VOID TInstDlg::DeleteFileFromPckFile(VOID	*pParam)
{
	TInstDlg	*pThis = (TInstDlg*)pParam;

	HWND	hList = pThis->GetDlgItem(IDC_LIST);

	UINT uiSelectCount = ListView_GetSelectedCount(hList);

	if (0 != uiSelectCount) {

		int	nCurrentItemCount = ListView_GetItemCount(hList);

		LVITEMW item {
			.mask = LVIF_PARAM | LVIF_STATE,
			.iSubItem = 0,
			.stateMask = LVIS_SELECTED,		// get all state flags
		};

		uiSelectCount = 0;

		for (item.iItem = 1; item.iItem < nCurrentItemCount; item.iItem++) {
			ListView_GetItem(hList, &item);

			if (item.state & LVIS_SELECTED) {
				//WINPCK_OK
				pck_DeleteEntry((LPPCK_UNIFIED_FILE_ENTRY)item.lParam);
				uiSelectCount++;
			}
		}

		if (0 == uiSelectCount)return;

		CStopWatch	timer;
		//开始计时
		timer.start();

		pThis->EnbaleButtons(FALSE);

		pThis->SetStatusBarInfo(std::vformat(::GetLoadStrW(IDS_STRING_RENEWING), std::make_wformat_args(pThis->m_Filename.filename().wstring())).c_str());

		pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

		if (WINPCK_OK == pck_RenameSubmit()) {
			//计时结束
			timer.stop();
			pThis->SetStatusBarInfo(std::vformat(::GetLoadStrW(IDS_STRING_RENEWOK), std::make_wformat_args(timer.getElapsedTime())).c_str());
			pThis->OpenPckFile(TRUE);

		}
		else {

			pThis->SetStatusBarInfo(::GetLoadStrW(IDS_STRING_PROCESS_ERROR));
		}

		pThis->EnbaleButtons(TRUE);

		pThis->KillTimer(WM_TIMER_PROGRESS_100);
		pThis->RefreshProgress();

		if (pThis->bGoingToExit) {
			pThis->bGoingToExit = FALSE;
			pThis->SendMessage(WM_CLOSE, 0, 0);
		}

	}
	return;
}

//从拖入的源文件名推出预保存的pck文件名
fs::path GetPckFileNameBySource(fs::path src, BOOL isDirectory)
{
	constexpr wchar_t file_end_with[] = L".pck.files";
	constexpr auto filepath_min_len = sizeof("c:\\") - 1 + sizeof(file_end_with) / sizeof(wchar_t) - 1;

	fs::path dst = src;

	if (isDirectory) {

		if (dst.wstring().ends_with(file_end_with) && dst.wstring().size() > filepath_min_len)
		{
			dst.replace_extension("");
		}

	}
	else {
		dst.replace_extension(dst.extension().string() + ".pck");
	}
	return dst;
}