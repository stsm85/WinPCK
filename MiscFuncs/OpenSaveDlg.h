#pragma once
#include <vector>
using namespace std;

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

#define	MAX_BUFFER_SIZE_OFN			0xFFFFF

BOOL OpenFilesVistaUp(HWND hwnd, TCHAR lpszPathName[MAX_PATH]);
BOOL OpenSingleFile(HWND hWnd, TCHAR * lpszFileName, LPCTSTR lpstrFilter = NULL, DWORD nFilterIndex = 1);
BOOL OpenFiles(HWND hWnd, vector <wstring> &lpszFilePathArray);
int SaveFile(HWND hWnd, char * lpszFileName, LPCSTR lpszDefaultExt = NULL, LPCSTR lpstrFilter = NULL, DWORD nFilterIndex = 0);
int SaveFile(HWND hWnd, wchar_t * lpszFileName, LPCWSTR lpszDefaultExt = NULL, LPCWSTR lpstrFilter = NULL, DWORD nFilterIndex = 0);

#if 0
BOOL BrowseForFolderByPath(HWND hWnd, TCHAR * lpszPathName);
static int CALLBACK BFFCallBack(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
#endif
