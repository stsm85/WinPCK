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

#include "tlib.h"
#include "resource.h"
//#include "globals.h"
#include "winmain.h"
#include "miscdlg.h"
//#include <shlobj.h>
//#include <strsafe.h>
#include <process.h>
#include "CharsCodeConv.h"
#include "OpenSaveDlg.h"

inline LONG RecurseDeleteKey(HKEY hRegKey, LPCTSTR lpszKey);
inline void CreateAndSetDefaultValue(LPCTSTR pszValueName, LPCTSTR pszValue);

void TInstDlg::UnpackAllFiles()
{
	if(m_lpPckCenter->IsValidPck()) {
		if(lpPckParams->cVarParams.bThreadRunning) {
			lpPckParams->cVarParams.bThreadRunning = FALSE;
			EnableButton(ID_MENU_UNPACK_ALL, FALSE);
		} else {
			if(BrowseForFolderByPath(hWnd, m_CurrentPath)) {
				lpPckParams->cVarParams.dwMTMemoryUsed = 0;
				SetCurrentDirectory(m_CurrentPath);
				_beginthread(ToExtractAllFiles, 0, this);
			}
		}
	}
}

void TInstDlg::UnpackSelectedFiles()
{
	if(m_lpPckCenter->IsValidPck()) {
		if(lpPckParams->cVarParams.bThreadRunning) {
			lpPckParams->cVarParams.bThreadRunning = FALSE;
			EnableButton(ID_MENU_UNPACK_SELECTED, FALSE);
		} else {
			if(BrowseForFolderByPath(hWnd, m_CurrentPath)) {
				lpPckParams->cVarParams.dwMTMemoryUsed = 0;
				SetCurrentDirectory(m_CurrentPath);
				_beginthread(ToExtractSelectedFiles, 0, this);
			}
		}
	}
}

void TInstDlg::DbClickListView(const int itemIndex)
{

	LVITEM						item;
	LPPCK_PATH_NODE		lpNodeToShow;

	m_lpPckCenter->SetListCurrentHotItem(itemIndex);

	item.mask = LVIF_PARAM;
	item.iItem = itemIndex;
	item.iSubItem = 0;
	item.stateMask = 0;
	ListView_GetItem(GetDlgItem(IDC_LIST), &item);

	//isSearchMode = 2 == item.iImage ? TRUE : FALSE;

	//列表是否是以搜索状态显示
	if(m_lpPckCenter->GetListInSearchMode()) {
		//memset(&m_PathDirs, 0, sizeof(m_PathDirs));
		//*m_PathDirs.lpszDirNames = m_PathDirs.szPaths;
		//m_PathDirs.nDirCount = 0;

		if(0 != itemIndex) {
			ViewFile();
			return;
		}
	}

	lpNodeToShow = (LPPCK_PATH_NODE)item.lParam;

	if(NULL == lpNodeToShow)return;

	//本级是否是文件夹(NULL=文件夹)
	if(NULL == lpNodeToShow->lpPckIndexTable) {
		//是否是上级目录(".."目录)
		if(NULL == lpNodeToShow->parentfirst) {
			//进入目录中(下一级)
			if(NULL != lpNodeToShow->child) {
				TCHAR **lpCurrentDir = m_PathDirs.lpszDirNames + m_PathDirs.nDirCount;

				_tcscpy_s(*lpCurrentDir, MAX_PATH - (*lpCurrentDir - *m_PathDirs.lpszDirNames), lpNodeToShow->szName);

				*(lpCurrentDir + 1) = *lpCurrentDir + _tcslen(*lpCurrentDir) + 1;

				m_PathDirs.nDirCount++;
			}

			ShowPckFiles(lpNodeToShow->child);
		} else {
			//上一级
			m_PathDirs.nDirCount--;
			TCHAR **lpCurrentDir = m_PathDirs.lpszDirNames + m_PathDirs.nDirCount;
			memset(*lpCurrentDir, 0, _tcslen(*lpCurrentDir) * sizeof(TCHAR));

			ShowPckFiles(lpNodeToShow->parentfirst);
		}
	} else {
		ViewFile();
	}
}

void TInstDlg::PopupRightMenu(const int itemIndex)
{

	LVITEM						item;
	LPPCK_PATH_NODE		lpNodeToShow;

	m_lpPckCenter->SetListCurrentHotItem(itemIndex);

	HMENU hMenuRClick = GetSubMenu(LoadMenu(TApp::GetInstance(), MAKEINTRESOURCE(IDR_MENU_RCLICK)), 0);

	item.mask = LVIF_PARAM;
	item.iItem = itemIndex;
	item.iSubItem = 0;
	item.stateMask = 0;
	ListView_GetItem(GetDlgItem(IDC_LIST), &item);

	//isSearchMode = 2 == item.iImage ? TRUE : FALSE;

	if(!m_lpPckCenter->GetListInSearchMode()) {

		lpNodeToShow = (LPPCK_PATH_NODE)item.lParam;

		if(NULL == lpNodeToShow || lpPckParams->cVarParams.bThreadRunning) {
			::EnableMenuItem(hMenuRClick, ID_MENU_VIEW, MF_GRAYED);
			::EnableMenuItem(hMenuRClick, ID_MENU_RENAME, MF_GRAYED);
			::EnableMenuItem(hMenuRClick, ID_MENU_DELETE, MF_GRAYED);
			::EnableMenuItem(hMenuRClick, ID_MENU_UNPACK_SELECTED, MF_GRAYED);

		} else {
			::EnableMenuItem(hMenuRClick, ID_MENU_VIEW, NULL != lpNodeToShow->lpPckIndexTable ? MF_ENABLED : MF_GRAYED);
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

	TrackPopupMenu(hMenuRClick, TPM_LEFTALIGN, LOWORD(GetMessagePos()), HIWORD(GetMessagePos()), 0, hWnd, NULL);
}

VOID TInstDlg::ViewFileAttribute()
{
	if(lpPckParams->cVarParams.bThreadRunning)return;

	HWND	hWndList = GetDlgItem(IDC_LIST);
	//int		iHotitem = ListView_GetHotItem(hWndList);
	int		iHotitem = m_lpPckCenter->GetListCurrentHotItem();

	if(0 == iHotitem)return;

	LVITEM	item = { 0 };

	item.mask = LVIF_PARAM;
	item.iItem = iHotitem;
	ListView_GetItem(hWndList, &item);


	if(m_lpPckCenter->IsValidPck()) {
		char	szPath[MAX_PATH_PCK_260];

		m_lpPckCenter->GetCurrentNodeString(szPath, m_currentNodeOnShow);

		TAttrDlg	dlg((void*)item.lParam, (void*)m_lpPckCenter->m_lpPckRootNode, m_lpPckCenter->GetPckRedundancyDataSize(), szPath, m_lpPckCenter->GetListInSearchMode(), this);
		dlg.Exec();
	}

}


VOID TInstDlg::ViewFile()
{
	if(lpPckParams->cVarParams.bThreadRunning)return;

	HWND	hWndList = GetDlgItem(IDC_LIST);
	int		iHotitem = m_lpPckCenter->GetListCurrentHotItem();

	DWORD	dwFilesizeToView;

	//LPPCK_PATH_NODE		lpNodeToShow;
	//LPPCKINDEXTABLE		lpIndexToShow;
	LPPCKINDEXTABLE		lpPckFileIndexToShow;

	LVITEM	item = { 0 };

	item.mask = LVIF_PARAM;
	item.iItem = iHotitem;
	ListView_GetItem(hWndList, &item);

	if(m_lpPckCenter->GetListInSearchMode()) {
		lpPckFileIndexToShow = (LPPCKINDEXTABLE)item.lParam;

	} else {
		lpPckFileIndexToShow = ((LPPCK_PATH_NODE)item.lParam)->lpPckIndexTable;
	}

	dwFilesizeToView = lpPckFileIndexToShow->cFileIndex.dwFileClearTextSize;


	char *buf;

	TDlg *dlg;
	//BOOL	isDds;

	const char	*lpszFileExt = strrchr(lpPckFileIndexToShow->cFileIndex.szFilename, '.');
	const TCHAR	*lpszFileTitle;
	//BOOL ((CPckControlCenter::*GetSingleFileData)(LPVOID, LPVOID, char *, size_t)) = *(m_lpPckCenter->GetSingleFileData);

	if(m_lpPckCenter->GetListInSearchMode()) {
		if(NULL == (lpszFileTitle = _tcsrchr(lpPckFileIndexToShow->cFileIndex.sztFilename, '\\')))
			if(NULL == (lpszFileTitle = _tcsrchr(lpPckFileIndexToShow->cFileIndex.sztFilename, '/')))
				lpszFileTitle = lpPckFileIndexToShow->cFileIndex.sztFilename - 1;

		//把前面的'\\'或'/'去掉
		lpszFileTitle++;
	} else {
//#ifdef UNICODE
//		CUcs2Ansi cU2A;
//		lpszFileTitle = cU2A.GetString(((LPPCK_PATH_NODE)item.lParam)->szName);
//#else
		lpszFileTitle = ((LPPCK_PATH_NODE)item.lParam)->szName;
//#endif
	}
		

	if(NULL != lpszFileExt) {
		char szExt[8];
		strcpy_s(szExt, lpszFileExt);
		//转化lpszFileExt为小写,当strlen(lpszFileExt) > 6 时报错，修改
		if(4 == strnlen_s(szExt, 6)) {
			_strlwr_s(szExt, 6);

			if(NULL != strstr(GetLoadStrA(IDS_STRING_FILE_EXT_PIC), szExt)) {
				if(0 == strcmpi(szExt, ".dds"))
					dlg = new TPicDlg(&buf, dwFilesizeToView, FMT_DDS, lpszFileTitle, this);
				else if(0 == strcmpi(szExt, ".tga"))
					dlg = new TPicDlg(&buf, dwFilesizeToView, FMT_TGA, lpszFileTitle, this);
				else
					dlg = new TPicDlg(&buf, dwFilesizeToView, FMT_RAW, lpszFileTitle, this);

			} else {
				dlg = new TViewDlg(&buf, dwFilesizeToView, lpszFileTitle, this);
			}
		} else {
			dlg = new TViewDlg(&buf, dwFilesizeToView, lpszFileTitle, this);
		}
	} else {
		dlg = new TViewDlg(&buf, dwFilesizeToView, lpszFileTitle, this);
	}

	if(0 != dwFilesizeToView) {
		if(NULL == buf) {
			m_lpPckCenter->PrintLogEL(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
			delete dlg;
			return;
		}

		m_lpPckCenter->GetSingleFileData(NULL, lpPckFileIndexToShow, buf, dwFilesizeToView);
	}

	dlg->Exec();

	delete dlg;

}

BOOL TInstDlg::AddFiles()
{

	if(lpPckParams->cVarParams.bThreadRunning)return FALSE;

	if(IDCANCEL == MessageBoxA("确定添加文件吗？", "询问", MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2))return FALSE;

	//TCHAR	(*lpszFilePathPtr)[MAX_PATH];
	//DWORD	dwFileCount;

	LPVOID	lpszFilePathPtr;

	if(OpenFiles(lpszFilePathPtr, m_DropFileCount)) {

		m_lpszFilePath = (TCHAR(*)[MAX_PATH])lpszFilePathPtr;
		DragAcceptFiles(hWnd, FALSE);

		//mt_MaxMemoryCount = mt_MaxMemory;
		_beginthread(UpdatePckFile, 0, this);


	}

	return FALSE;
}


BOOL TInstDlg::OpenFiles(LPVOID &lpszFilePathArray, DWORD &dwFileCount)
{

	OPENFILENAME ofn;
	TCHAR * szBuffer, *szBufferPart;
	//FILEINFO * pFileinfo, * pFileinfo_previtem;
	size_t stStringLength;

	szBuffer = (TCHAR *)malloc(MAX_BUFFER_SIZE_OFN * sizeof(TCHAR));
	if(szBuffer == NULL) {
		m_lpPckCenter->PrintLogEL(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	_tcscpy_s(szBuffer, MAX_BUFFER_SIZE_OFN, TEXT(""));

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = TEXT("所有文件\0*.*\0\0");
	ofn.lpstrFile = szBuffer;
	ofn.nMaxFile = MAX_BUFFER_SIZE_OFN;
	ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_ENABLESIZING | OFN_FILEMUSTEXIST;

	if(!GetOpenFileName(&ofn)) {
		if(CommDlgExtendedError() == FNERR_BUFFERTOOSMALL) {
			MessageBox(TEXT("选择的文件太多. 缓冲区无法装下所有文件的文件名。"),
				TEXT("缓冲区不够"), MB_OK);
		}
		free(szBuffer);
		return FALSE;
	}

	dwFileCount = 0;

	// if first part of szBuffer is a directory the user selected multiple files
	// otherwise szBuffer is filename + path
	if(GetFileAttributes(szBuffer) & FILE_ATTRIBUTE_DIRECTORY) {
		// szBuffer 中第一个部分是目录 
		// 其他部分是 FileTitle


		TCHAR		**szFilenamePtrArray, **szFilenamePtrArrayPtr;
		if(NULL == (szFilenamePtrArray = (TCHAR**)malloc(MAX_BUFFER_SIZE_OFN * sizeof(TCHAR**)))) {
			m_lpPckCenter->PrintLogEL(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
			free(szBuffer);
			return FALSE;

		}

		szFilenamePtrArrayPtr = szFilenamePtrArray;

		szBufferPart = szBuffer + ofn.nFileOffset;
		*szFilenamePtrArrayPtr = szBuffer + ofn.nFileOffset;

		//根目录下时目录名中会带'\'
		if(4 == ofn.nFileOffset) {
			ofn.nFileOffset--;
			szBuffer[2] = 0;
		}


		while(0 != *szBufferPart) {

			stStringLength = _tcsnlen(szBufferPart, MAX_PATH);
			szBufferPart += stStringLength + 1;

			*(++szFilenamePtrArrayPtr) = szBufferPart;

			dwFileCount++;

		}

		*szFilenamePtrArrayPtr = 0;

		if(NULL == (lpszFilePathArray = /*(TCHAR(*)[MAX_PATH])*/ malloc(sizeof(TCHAR) * MAX_PATH * dwFileCount))) {
			m_lpPckCenter->PrintLogEL(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
			free(szFilenamePtrArray);
			free(szBuffer);
			return	FALSE;
		}

		TCHAR(*lpszFilePathArrayPtr)[MAX_PATH] = (TCHAR(*)[MAX_PATH])lpszFilePathArray;

		szFilenamePtrArrayPtr = szFilenamePtrArray;

		while(0 != *szFilenamePtrArrayPtr) {
			_stprintf_s(*lpszFilePathArrayPtr, MAX_PATH, TEXT("%s\\%s"), szBuffer, *szFilenamePtrArrayPtr);

			lpszFilePathArrayPtr++;
			szFilenamePtrArrayPtr++;
		}

		free(szFilenamePtrArray);
	} else { // 中选中了一个文件

		dwFileCount = 1;

		if(NULL == (lpszFilePathArray = /*(TCHAR(*)[MAX_PATH])*/ malloc(sizeof(TCHAR) * MAX_PATH))) {
			m_lpPckCenter->PrintLogEL(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
			free(szBuffer);
			return	FALSE;
		}

		_tcscpy_s((TCHAR*)lpszFilePathArray, MAX_PATH, szBuffer);

	}

	// 删除缓冲区

	free(szBuffer);

	return TRUE;
}

void TInstDlg::AddSetupReg()
{

	//BOOL	isExistOldReg;
	HKEY	hRegKey;
	LONG	result;
	//DWORD	dwType;

	//TCHAR	szString[MAX_PATH];
	TCHAR	szStringIcon[MAX_PATH];
	TCHAR	szStringExec[MAX_PATH];

	_tcscpy_s(szStringIcon, m_MyFileName);
	_tcscat_s(szStringIcon, TEXT(",0"));


	_tcscpy_s(szStringExec, TEXT("\""));
	_tcscat_s(szStringExec, m_MyFileName);
	_tcscat_s(szStringExec, TEXT("\" \"%1\""));

	//DWORD	dwDataLength;
	//m_MyFileName

	//检查是否存在[HKEY_CLASSES_ROOT\pckfile]
	if(ERROR_SUCCESS == (result = RegOpenKeyEx(HKEY_CLASSES_ROOT,
		TEXT("pckfile\\shell\\open\\command"),
		0,
		KEY_READ,
		&hRegKey))) {
		//存在
		//1.如果程序包含patcher.exe，新加
		//result = RegQueryValueEx(hRegKey, NULL, NULL, &dwType, reinterpret_cast<LPBYTE>(szString), &dwDataLength)
		//2.否则替换
		//3.修改打开方式索引
		RegCloseKey(hRegKey);

		RecurseDeleteKey(HKEY_CLASSES_ROOT, TEXT(".pck"));
		RecurseDeleteKey(HKEY_CLASSES_ROOT, TEXT("pckfile"));
		RecurseDeleteKey(HKEY_CLASSES_ROOT, TEXT(".zup"));
		RecurseDeleteKey(HKEY_CLASSES_ROOT, TEXT("ZPWUpdatePack"));

	}

	CreateAndSetDefaultValue(TEXT(".pck"), TEXT("pckfile"));
	CreateAndSetDefaultValue(TEXT("pckfile"), TEXT("Angelica File Package"));
	CreateAndSetDefaultValue(TEXT("pckfile\\DefaultIcon"), szStringIcon);
	CreateAndSetDefaultValue(TEXT("pckfile\\shell"), NULL);
	CreateAndSetDefaultValue(TEXT("pckfile\\shell\\open"), TEXT("使用 WinPCK 打开"));
	CreateAndSetDefaultValue(TEXT("pckfile\\shell\\open\\command"), szStringExec);

	CreateAndSetDefaultValue(TEXT(".zup"), TEXT("ZPWUpdatePack"));
	CreateAndSetDefaultValue(TEXT("ZPWUpdatePack"), TEXT("诛仙更新包"));
	CreateAndSetDefaultValue(TEXT("ZPWUpdatePack\\DefaultIcon"), szStringIcon);
	CreateAndSetDefaultValue(TEXT("ZPWUpdatePack\\shell"), NULL);
	CreateAndSetDefaultValue(TEXT("ZPWUpdatePack\\shell\\open"), TEXT("使用 WinPCK 打开"));
	CreateAndSetDefaultValue(TEXT("ZPWUpdatePack\\shell\\open\\command"), szStringExec);

	MessageBox(TEXT("安装完成。"), TEXT("信息"), MB_OK | MB_ICONASTERISK);
}

void TInstDlg::DeleteSetupReg()
{
	RecurseDeleteKey(HKEY_CLASSES_ROOT, TEXT(".pck"));
	RecurseDeleteKey(HKEY_CLASSES_ROOT, TEXT("pckfile"));
	RecurseDeleteKey(HKEY_CLASSES_ROOT, TEXT(".zup"));
	RecurseDeleteKey(HKEY_CLASSES_ROOT, TEXT("ZPWUpdatePack"));

	MessageBox(TEXT("卸载完成。"), TEXT("信息"), MB_OK | MB_ICONASTERISK);
}

inline void CreateAndSetDefaultValue(LPCTSTR pszValueName, LPCTSTR pszValue)
{
	HKEY	hRegKey;

	if(ERROR_SUCCESS == RegCreateKeyEx(HKEY_CLASSES_ROOT,
		pszValueName,
		0,
		REG_NONE,
		REG_OPTION_NON_VOLATILE,
		KEY_READ | KEY_WRITE,
		NULL,
		&hRegKey,
		NULL)) {
		if(NULL != pszValue)
			RegSetValueEx(hRegKey, NULL, NULL, REG_SZ, reinterpret_cast<const BYTE*>(pszValue), (lstrlen(pszValue) + 1) * sizeof(TCHAR));

		RegCloseKey(hRegKey);
	}

}

inline LONG RecurseDeleteKey(HKEY hRegKey, LPCTSTR lpszKey)
{
	HKEY hSubRegKey;
	LONG lRes = RegOpenKeyEx(hRegKey, lpszKey, 0, KEY_READ | KEY_WRITE, &hSubRegKey);
	if(lRes != ERROR_SUCCESS) {
		return lRes;
	}

	FILETIME time;
	DWORD dwSize = 256;
	TCHAR szBuffer[256];
	while(RegEnumKeyEx(hSubRegKey, 0, szBuffer, &dwSize, NULL, NULL, NULL,
		&time) == ERROR_SUCCESS) {
		lRes = RecurseDeleteKey(hSubRegKey, szBuffer);
		if(lRes != ERROR_SUCCESS)
			return lRes;
		dwSize = 256;
	}

	RegCloseKey(hSubRegKey);

	return RegDeleteKey(hRegKey, lpszKey);


}

void TInstDlg::InitLogWindow()
{

	//Log windows
	logdlg = new TLogDlg(this);
	logdlg->Create();
	m_lpPckCenter->SetLogListWnd(logdlg->GetListWnd());

	//logdlg->Show();

	m_lpPckCenter->PrintLogI(THIS_NAME \
		THIS_VERSION \
		" is started.");

}


void TInstDlg::RefreshProgress()
{
	TCHAR		szString[MAX_PATH];
	INT			iNewPos;

	//if(0 == lpPckParams->cVarParams.dwUIProgressUpper)lpPckParams->cVarParams.dwUIProgressUpper = 1;
	iNewPos = (INT)((lpPckParams->cVarParams.dwUIProgress << 10) /
		lpPckParams->cVarParams.dwUIProgressUpper);

	SendDlgItemMessage(IDC_PROGRESS, PBM_SETPOS, (WPARAM)iNewPos, (LPARAM)0);

	if(lpPckParams->cVarParams.dwUIProgress == lpPckParams->cVarParams.dwUIProgressUpper)
		_stprintf_s(szString, szTimerProcessedFormatString, lpPckParams->cVarParams.dwUIProgress, lpPckParams->cVarParams.dwUIProgressUpper);
	else
		_stprintf_s(szString, szTimerProcessingFormatString, lpPckParams->cVarParams.dwUIProgress, lpPckParams->cVarParams.dwUIProgressUpper, lpPckParams->cVarParams.dwUIProgress * 100.0 / lpPckParams->cVarParams.dwUIProgressUpper, (lpPckParams->cVarParams.dwMTMemoryUsed >> 10) * 100.0 / (lpPckParams->dwMTMaxMemory >> 10));

	SetStatusBarText(3, szString);

}