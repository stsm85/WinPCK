#pragma once
#include <vector>
using namespace std;

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

#define	MAX_BUFFER_SIZE_OFN			0xFFFFF


BOOL OpenSingleFile(HWND hWnd, TCHAR * lpszFileName, LPCTSTR lpstrFilter = NULL, DWORD nFilterIndex = 1);
BOOL OpenFiles(HWND hWnd, vector <tstring> &lpszFilePathArray);
DWORD SaveFile(HWND hWnd, char * lpszFileName, LPCSTR lpszDefaultExt = NULL, LPCSTR lpstrFilter = NULL, DWORD nFilterIndex = 1);
DWORD SaveFile(HWND hWnd, wchar_t * lpszFileName, LPCWSTR lpszDefaultExt = NULL, LPCWSTR lpstrFilter = NULL, DWORD nFilterIndex = 1);
BOOL BrowseForFolderByPath(HWND hWnd, TCHAR * lpszPathName);
static int CALLBACK BFFCallBack(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

