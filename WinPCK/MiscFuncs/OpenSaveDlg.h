#pragma once
BOOL OpenSingleFile(HWND hWnd, TCHAR * lpszFileName, LPCTSTR lpstrFilter = NULL, DWORD nFilterIndex = 1);
DWORD SaveFile(HWND hWnd, TCHAR * lpszFileName, LPCTSTR lpstrFilter = NULL, DWORD nFilterIndex = 1);
BOOL BrowseForFolderByPath(HWND hWnd, TCHAR * lpszPathName);
static int CALLBACK BFFCallBack(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
