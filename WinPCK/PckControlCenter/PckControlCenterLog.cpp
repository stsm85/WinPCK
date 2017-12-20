
//////////////////////////////////////////////////////////////////////
// PckControlCenterLog.cpp: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 头文件,界面与PCK类的数据交互，控制中心
//
// 此程序由 李秋枫/stsm/liqf 编写，pck结构参考若水的pck结构.txt，并
// 参考了其易语言代码中并于读pck文件列表的部分
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2015.5.21
//////////////////////////////////////////////////////////////////////

#include "PckControlCenter.h"
#include <commctrl.h>
#include "resource.h"

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4244 )

char *GetErrorMsg( CONST DWORD dwError)
{
	static char szMessage[1024] = "错误原因：";
	//strcpy(szMessage, "错误原因：");

	char *lpszMessage = szMessage + 10;
	// retrieve a message from the system message table
	switch(dwError)
	{
	case 2:
		strcpy(lpszMessage, "系统找不到指定的文件。");
		break;
	case 3:
		strcpy(lpszMessage, "系统找不到指定的路径。");
		break;
	case 4:
		strcpy(lpszMessage, "系统无法打开文件。");
		break;
	case 5:
		strcpy(lpszMessage, "拒绝访问。");
		break;
	case 32:
		strcpy(lpszMessage, "另一个程序正在使用此文件，进程无法访问。");
		break;
	default:
		if (!FormatMessageA( 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			lpszMessage,
			1014,
			NULL ))
		{
			strcpy(lpszMessage, "未知错误");
		}
	}
	SetLastError(0);
	return szMessage;
}


inline char *UCS2toA(const WCHAR *src, int max_len=-1)
{
	static char dst[8192];
	::WideCharToMultiByte(CP_ACP, 0, src, max_len, dst, 8192, "_", 0);
	return dst;
}

void CPckControlCenter::SetLogListWnd(HWND _hWndList)
{
	m_hWndLogListWnd = _hWndList;
}


void CPckControlCenter::_InsertLogIntoList(const int _loglevel, const char *_logtext)
{

	LVITEMA	item;
	SYSTEMTIME systime;

	if (LOG_IMAGE_NOTICE == _loglevel) {
		::SendDlgItemMessageA(m_hWndMain, IDC_STATUS, SB_SETTEXTA, 4, (LPARAM)_logtext);
		::SendDlgItemMessageA(m_hWndMain, IDC_STATUS, SB_SETTIPTEXTA, 4, (LPARAM)_logtext);
	}

	char szPrintf[4096];

	GetLocalTime (&systime);

	sprintf_s(szPrintf, "%02d:%02d:%02d %s", systime.wHour, systime.wMinute, systime.wSecond, _logtext);


	ZeroMemory(&item, sizeof(LVITEMA));

	item.iItem = INT_MAX;			//从0开始
	item.iImage = _loglevel;
	//item.iSubItem = 0;
	item.mask = LVIF_TEXT | LVIF_IMAGE;
	item.pszText = szPrintf;

	if(-1 != (m_LogListCount = ::SendMessageA(m_hWndLogListWnd, LVM_INSERTITEMA, 0, (LPARAM)&item))){
		ListView_EnsureVisible(m_hWndLogListWnd, m_LogListCount, 0);
		//++m_LogListCount;
	}

}

void CPckControlCenter::PrintLogI(const char *_logtext){
	_InsertLogIntoList(LOG_IMAGE_INFO, _logtext);
}

void CPckControlCenter::PrintLogW(const char *_logtext){
	_InsertLogIntoList(LOG_IMAGE_WARNING, _logtext);
}

void CPckControlCenter::PrintLogE(const char *_logtext){
	_InsertLogIntoList(LOG_IMAGE_ERROR, _logtext);
	ShowWindow(GetParent(m_hWndLogListWnd), SW_SHOW);
}

void CPckControlCenter::PrintLogD(const char *_logtext){
	_InsertLogIntoList(LOG_IMAGE_DEBUG, _logtext);
}

void CPckControlCenter::PrintLogN(const char *_logtext) {
	_InsertLogIntoList(LOG_IMAGE_NOTICE, _logtext);
}

void CPckControlCenter::_InsertLogIntoList(const int _loglevel, const wchar_t *_logtext)
{

	LVITEMW	item;
	SYSTEMTIME systime;

	if (LOG_IMAGE_NOTICE == _loglevel) {
		::SendDlgItemMessageW(m_hWndMain, IDC_STATUS, SB_SETTEXTW, 4, (LPARAM)_logtext);
		::SendDlgItemMessageW(m_hWndMain, IDC_STATUS, SB_SETTIPTEXTW, 4, (LPARAM)_logtext);
	}

	wchar_t szPrintf[4096];

	GetLocalTime (&systime);

	swprintf_s(szPrintf, L"%02d:%02d:%02d %s", systime.wHour, systime.wMinute, systime.wSecond, _logtext);


	ZeroMemory(&item, sizeof(LVITEMW));

	item.iItem = INT_MAX;			//从0开始
	item.iImage = _loglevel;
	//item.iSubItem = 0;
	item.mask = LVIF_TEXT | LVIF_IMAGE;
	item.pszText = szPrintf;

	if(-1 != (m_LogListCount = ::SendMessageW(m_hWndLogListWnd, LVM_INSERTITEMW, 0, (LPARAM)&item))){
		ListView_EnsureVisible(m_hWndLogListWnd, m_LogListCount, 0);
		//++m_LogListCount;
	}
	
}

void CPckControlCenter::PrintLogI(const wchar_t *_logtext){
	_InsertLogIntoList(LOG_IMAGE_INFO, _logtext);
}

void CPckControlCenter::PrintLogW(const wchar_t *_logtext){
	_InsertLogIntoList(LOG_IMAGE_WARNING, _logtext);
}

void CPckControlCenter::PrintLogE(const wchar_t *_logtext){
	_InsertLogIntoList(LOG_IMAGE_ERROR, _logtext);
	ShowWindow(GetParent(m_hWndLogListWnd), SW_SHOW);
}

void CPckControlCenter::PrintLogD(const wchar_t *_logtext){
	_InsertLogIntoList(LOG_IMAGE_DEBUG, _logtext);
}

void CPckControlCenter::PrintLogN(const wchar_t *_logtext){
	_InsertLogIntoList(LOG_IMAGE_NOTICE, _logtext);
}

void CPckControlCenter::PrintLogE(const char *_maintext, const char *_file, const char *_func, const long _line)
{
	char szPrintf[8192];
	sprintf_s(szPrintf, "%s (%s 发生错误在 %s 行数:%d)", _maintext, _file, _func, _line);
	PrintLogE(szPrintf);
	if (0 == m_dwLastError) {
		m_dwLastError = GetLastError();
		if(0 != m_dwLastError)
			PrintLogE(GetErrorMsg(GetLastError()));
	} else{
		PrintLogE(GetErrorMsg(m_dwLastError));
		m_dwLastError = 0;
	}
}

void CPckControlCenter::PrintLogE(const wchar_t *_maintext, const char *_file, const char *_func, const long _line)
{
	m_dwLastError = GetLastError();
	PrintLogE(UCS2toA(_maintext), _file, _func, _line);
}

void CPckControlCenter::PrintLogE(const char *_fmt, const char *_maintext, const char *_file, const char *_func, const long _line)
{
	char szPrintf[8192];
	sprintf_s(szPrintf, _fmt, _maintext);
	PrintLogE(szPrintf, _file, _func, _line);

}
void CPckControlCenter::PrintLogE(const char *_fmt, const wchar_t *_maintext, const char *_file, const char *_func, const long _line)
{
	m_dwLastError = GetLastError();
	PrintLogE(_fmt, UCS2toA(_maintext), _file, _func, _line);
}


void CPckControlCenter::PrintLog(const char chLevel, const char *_maintext)
{
	//char szPrintf[8192];
	//StringCchPrintfA(szPrintf, 8192, "%s", _maintext);
	switch(chLevel)
	{
	case LOG_FLAG_ERROR:
		PrintLogE(_maintext);
		break;
	case LOG_FLAG_WARNING:
		PrintLogW(_maintext);
		break;
	case LOG_FLAG_INFO:
		PrintLogI(_maintext);
		break;
	case LOG_FLAG_NOTICE:
		PrintLogN(_maintext);
		break;
#ifdef _DEBUG
	case LOG_FLAG_DEBUG:
		PrintLogD(_maintext);
		break;
#endif
	default:
		PrintLogI(_maintext);
	}
	//PrintLogE(szPrintf);
}

void CPckControlCenter::PrintLog(const char chLevel, const wchar_t *_maintext)
{
	PrintLog(chLevel, UCS2toA(_maintext));
}

void CPckControlCenter::PrintLog(const char chLevel, const char *_fmt, const char *_maintext)
{
	char szPrintf[8192];
	sprintf_s(szPrintf, _fmt, _maintext);
	PrintLog(chLevel, szPrintf);

}
void CPckControlCenter::PrintLog(const char chLevel, const char *_fmt, const wchar_t *_maintext)
{
	PrintLog(chLevel, _fmt, UCS2toA(_maintext));
}