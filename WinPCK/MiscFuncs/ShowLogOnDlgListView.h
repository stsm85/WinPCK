#pragma once

//log日志相关功能 
void	SetLogListWnd(HWND _hWndList);
void	SetLogMainWnd(HWND _hWnd);

//log日志相关功能 
void	PreInsertLogToList(const int, const WCHAR *);
void	_InsertLogIntoList(const int, const wchar_t *);

