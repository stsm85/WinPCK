#include <Windows.h>
#include "OpenSaveDlg.h"
#include <tchar.h>
#include <shlobj.h>
#include "globals.h"

/******************************************************
只打开一个文件
******************************************************/

BOOL OpenSingleFile(HWND hWnd, TCHAR * lpszFileName)
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
	ofn.lpstrFilter = TEXT_FILE_FILTER;
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


DWORD SaveFile(HWND hWnd, TCHAR * lpszFileName, LPCTSTR lpstrFilter, DWORD nFilterIndex)
{
	OPENFILENAME ofn;
	//TCHAR szStrPrintf[260];

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	//ofn.lpstrFilter       = TEXT_FILE_FILTER;
	ofn.lpstrFilter = lpstrFilter;
	ofn.lpstrFile = lpszFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING;
	ofn.lpstrDefExt = TEXT("pck");
	ofn.nFilterIndex = nFilterIndex;

	if(!GetSaveFileName(&ofn)) {
		return FALSE;
	}
	return ofn.nFilterIndex;

}

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