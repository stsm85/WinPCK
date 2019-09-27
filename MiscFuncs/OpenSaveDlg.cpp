//////////////////////////////////////////////////////////////////////
// OpenSaveDlg.cpp: WinPCK 辅助函数部分
// 显示文件打开、保存对话框
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2018.5.29
//////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include "OpenSaveDlg.h"
#include <tchar.h>
#include <shlobj.h>
#include "COpenFileListener.h"

#define DEFAULT_FILTER	"所有文件\0*.*\0\0"
#define __L(quote) L#quote


BOOL OpenFilesVistaUp(HWND hwnd, TCHAR lpszPathName[MAX_PATH])
{
	IFileOpenDialog *pfd;
	FILEOPENDIALOGOPTIONS dwOptions;
	DWORD dwCookie = 0;

	CoInitialize(NULL);

	// CoCreate the dialog object.
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pfd));

	if(SUCCEEDED(hr)) {

		hr = pfd->GetOptions(&dwOptions);

		if(SUCCEEDED(hr)) {
			hr = pfd->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);

			if(SUCCEEDED(hr)) {
				COpenFileListener *ofl = new COpenFileListener(lpszPathName);
				hr = pfd->Advise(ofl, &dwCookie);

				if(SUCCEEDED(hr)) {
					hr = pfd->Show(hwnd);

					if(SUCCEEDED(hr)) {

					}

					pfd->Unadvise(dwCookie);
				}
			}
		}

		pfd->Release();
	}

	CoUninitialize();

	return SUCCEEDED(hr);
}


/******************************************************
只打开一个文件
******************************************************/

BOOL OpenSingleFile(HWND hWnd, TCHAR * lpszFileName, LPCTSTR lpstrFilter, DWORD nFilterIndex)
{
	OPENFILENAME ofn;
	TCHAR szStrPrintf[MAX_PATH];
	*szStrPrintf = 0;

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	if('\\' == *(lpszFileName + lstrlen(lpszFileName) - 1)) {
		ofn.lpstrInitialDir = lpszFileName;
		ofn.lpstrFile = szStrPrintf;
	} else {
		ofn.lpstrFile = lpszFileName;
	}

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = (NULL == lpstrFilter) ? TEXT(DEFAULT_FILTER) : lpstrFilter;
	ofn.nFilterIndex = nFilterIndex;
	//ofn.lpstrFile         = lpszFileName;
	//ofn.lpstrInitialDir   = lpszFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ENABLESIZING/* | OFN_ALLOWMULTISELECT*/;

	if(!GetOpenFileName(&ofn)) {
		return FALSE;
	}

	if(NULL != ofn.lpstrInitialDir)
		_tcscpy_s(lpszFileName, MAX_PATH, szStrPrintf);

	return TRUE;

}


BOOL OpenFiles(HWND hWnd, vector<wstring> &lpszFilePathArray)
{

	OPENFILENAME ofn;
	TCHAR * szBuffer, *szBufferPart;
	size_t stStringLength;

	szBuffer = (TCHAR *)malloc(MAX_BUFFER_SIZE_OFN * sizeof(TCHAR));
	if(szBuffer == NULL) {
		//m_PckLog.PrintLogEL(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
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
			MessageBox(hWnd, TEXT("选择的文件太多. 缓冲区无法装下所有文件的文件名。"),
				TEXT("缓冲区不够"), MB_OK);
		}
		free(szBuffer);
		return FALSE;
	}

	lpszFilePathArray.clear();

	// if first part of szBuffer is a directory the user selected multiple files
	// otherwise szBuffer is filename + path
	if(GetFileAttributes(szBuffer) & FILE_ATTRIBUTE_DIRECTORY) {
		// szBuffer 中第一个部分是目录 
		// 其他部分是 FileTitle

		szBufferPart = szBuffer + ofn.nFileOffset;

		//根目录下时目录名中会带'\'
		if(4 == ofn.nFileOffset) {
			ofn.nFileOffset--;
			szBuffer[2] = 0;
		}

		while(0 != *szBufferPart) {

			stStringLength = _tcsnlen(szBufferPart, MAX_PATH);
			szBufferPart += stStringLength + 1;

			TCHAR szFullPath[MAX_PATH] = { 0 };
			_stprintf_s(szFullPath, TEXT("%s\\%s"), szBuffer, szBufferPart);

			lpszFilePathArray.push_back(szFullPath);
		}

	} else { // 中选中了一个文件

		lpszFilePathArray.push_back(szBuffer);

	}

	// 删除缓冲区
	free(szBuffer);

	return TRUE;
}

int SaveFile(HWND hWnd, char * lpszFileName, LPCSTR lpszDefaultExt , LPCSTR lpstrFilter, DWORD nFilterIndex)
{
	/*
	nFilterIndex 的base index是1，所以输入时要+1，输出要-1
	*/

	OPENFILENAMEA ofn;

	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = (NULL == lpstrFilter) ? DEFAULT_FILTER : lpstrFilter;
	ofn.lpstrFile = lpszFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING;
	ofn.lpstrDefExt = lpszDefaultExt;
	ofn.nFilterIndex = nFilterIndex + 1;

	//计算lpstrDefExt的字符值


	if(!GetSaveFileNameA(&ofn)) {
		return FALSE;
	}
	return ofn.nFilterIndex - 1;

}

int SaveFile(HWND hWnd, wchar_t * lpszFileName, LPCWSTR lpszDefaultExt, LPCWSTR lpstrFilter, DWORD nFilterIndex)
{
	/*
	nFilterIndex 的base index是1，所以输入时要+1，输出要-1
	*/
	OPENFILENAMEW ofn;

	ZeroMemory(&ofn, sizeof(OPENFILENAMEW));
	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = (NULL == lpstrFilter) ? __L(DEFAULT_FILTER) : lpstrFilter;
	ofn.lpstrFile = lpszFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING;
	ofn.lpstrDefExt = lpszDefaultExt;
	ofn.nFilterIndex = nFilterIndex + 1;

	if(!GetSaveFileNameW(&ofn)) {
		return FALSE;
	}
	return ofn.nFilterIndex - 1;

}
#if 0
BOOL BrowseForFolderByPath(HWND hWnd, TCHAR * lpszPathName)
{

	BROWSEINFO					cBI;
	ITEMIDLIST					idl;
	LPITEMIDLIST				pidl = &idl;

	cBI.hwndOwner = hWnd;
	cBI.pidlRoot = 0;
	cBI.lpszTitle = TEXT("请选择目录");
	cBI.lpfn = BFFCALLBACK(&BFFCallBack);

	cBI.lParam = (LPARAM)lpszPathName;

	cBI.pszDisplayName = lpszPathName;

	cBI.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
	//OleInitialize(0);
	pidl = SHBrowseForFolder(&cBI);
	//OleUninitialize();
	if(NULL != pidl) {
		SHGetPathFromIDList(pidl, lpszPathName);
		CoTaskMemFree((VOID*)pidl);
		return TRUE;
	}
	return FALSE;
}

int CALLBACK BFFCallBack(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	//TCHAR szPath[MAX_PATH];
	//TCHAR szExePath[MAX_PATH];

	//ITEMIDLIST					idl;
	//LPITEMIDLIST				pidl = &idl;
	//BOOL bTemp;

	switch(uMsg) {
	case BFFM_INITIALIZED:
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		break;
	}

	return 0;
}
#endif