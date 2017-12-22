
/* ========================================================================
	Project  Name			: Win32 Lightweight  Class Library Test
	Module Name				: Registry Class
	Create					: 1996-06-01(Sat)
	Update					: 2010-05-09(Sun)
	Copyright				: H.Shirouzu
	Reference				: 
	======================================================================== */
#include <stdio.h>
#include "tlib.h"
#ifdef _USE_T_REG_
static char *tregist_id =
"@(#)Copyright (C) 1996-2010 H.Shirouzu		tregist.cpp	Ver0.97";

#include "tregist.h"

TRegistry::TRegistry(LPCTSTR company, LPTSTR appName)
{
	openCnt = 0;
	ChangeApp(company, appName);
}

TRegistry::TRegistry(HKEY top_key)
{
	topKey = top_key;
	openCnt = 0;
}

TRegistry::~TRegistry(void)
{
	while (openCnt > 0) {
		CloseKey();
	}
}



BOOL TRegistry::ChangeApp(LPCTSTR company, LPTSTR appName)
{
	while (openCnt > 0) {
		CloseKey();
	}

	topKey = HKEY_CURRENT_USER;

	TCHAR	wbuf[100];
#ifdef UNICODE

	swprintf(wbuf, L"software\\%s" , company);

	if (appName != NULL && (*appName)) {
		swprintf(wbuf + wcslen(wbuf), L"\\%s", appName);
	}

#else

	sprintf(wbuf, "software\\%s", company);

	if (appName != NULL && (*appName)) {
		sprintf(wbuf + strlen(wbuf), "\\%s", appName);
	}

#endif

	return	CreateKey(wbuf);
}

void TRegistry::ChangeTopKey(HKEY top_key)
{
	while (openCnt > 0)
		CloseKey();

	topKey = top_key;
}

BOOL TRegistry::OpenKey(LPCTSTR subKey, BOOL createFlg)
{
	HKEY	parentKey = (openCnt == 0 ? topKey : hKey[openCnt -1]);

	if (openCnt >= MAX_KEYARRAY) {
		return	FALSE;
	}
	DWORD	tmp;
	LONG	status;

	if (createFlg) {
		status = ::RegCreateKeyEx(parentKey, subKey, 0, NULL, REG_OPTION_NON_VOLATILE,
					KEY_ALL_ACCESS, NULL, &hKey[openCnt], &tmp);
	}
	else {
		if ((status = ::RegOpenKeyEx(parentKey, subKey, 0, KEY_ALL_ACCESS, &hKey[openCnt]))
				!= ERROR_SUCCESS)
			status = ::RegOpenKeyEx(parentKey, subKey, 0, KEY_READ, &hKey[openCnt]);
	}

	if (status == ERROR_SUCCESS)
		return	openCnt++, TRUE;
	else
		return	FALSE;
}

BOOL TRegistry::CloseKey(void)
{
	if (openCnt <= 0) {
		return	FALSE;
	}
	::RegCloseKey(hKey[--openCnt]);

	return	TRUE;
}

BOOL TRegistry::GetInt(LPCTSTR subKey, int *val)
{
	return	GetLong(subKey, (long *)val);
}

BOOL TRegistry::SetInt(LPCTSTR subKey, int val)
{
	return	SetLong(subKey, (long)val);
}

BOOL TRegistry::GetLong(LPCTSTR subKey, long *val)
{
	DWORD	type = REG_DWORD, dw_size = sizeof(long);

	if (::RegQueryValueEx(hKey[openCnt -1], subKey, 0, &type, (BYTE *)val, &dw_size)
			== ERROR_SUCCESS) {
		return	TRUE;
	}
// Ì‚ÌŒÝŠ·«—p
	TCHAR	wbuf[100];
	long	size_byte = sizeof(wbuf);

	if (::RegQueryValue(hKey[openCnt -1], subKey, wbuf, &size_byte) != ERROR_SUCCESS)
		return	FALSE;
#ifdef UNICODE
	*val = wcstol(wbuf, 0, 10);
#else
	*val = strtol(wbuf, 0, 10);
#endif
	return	TRUE;
}

BOOL TRegistry::SetLong(LPCTSTR subKey, long val)
{
	return	::RegSetValueEx(hKey[openCnt -1], subKey, 0, REG_DWORD,
			(const BYTE *)&val, sizeof(val)) == ERROR_SUCCESS;
}

BOOL TRegistry::GetStr(LPCTSTR subKey, LPTSTR str, int size)
{
	DWORD	type = REG_SZ;

	if (::RegQueryValueEx(hKey[openCnt -1], subKey, 0, &type, (BYTE *)str, (DWORD *)&size) != ERROR_SUCCESS
	&&  ::RegQueryValue(hKey[openCnt -1], subKey, str, (LPLONG)&size) != ERROR_SUCCESS)
		return	FALSE;

	return	TRUE;
}


BOOL TRegistry::SetStr(LPCTSTR subKey, LPCTSTR str)
{
#ifdef UNICODE
	return	::RegSetValueEx(hKey[openCnt -1], subKey, 0, REG_SZ, (const BYTE *)str, (DWORD)(wcslen(str) +1) * sizeof(TCHAR)) == ERROR_SUCCESS;////
#else
	return	::RegSetValueEx(hKey[openCnt -1], subKey, 0, REG_SZ, (const BYTE *)str, (DWORD)strlen(str) +1) == ERROR_SUCCESS;
#endif
}

BOOL TRegistry::GetByte(LPCTSTR subKey, BYTE *data, int *size)
{
	DWORD	type = REG_BINARY;

	return	::RegQueryValueEx(hKey[openCnt -1], subKey, 0, &type,
			(BYTE *)data, (LPDWORD)size) == ERROR_SUCCESS;
}


BOOL TRegistry::SetByte(LPCTSTR subKey, const BYTE *data, int size)
{
	return	::RegSetValueEx(hKey[openCnt -1], subKey, 0, REG_BINARY, data, size)
			== ERROR_SUCCESS;
}

BOOL TRegistry::DeleteKey(LPCTSTR subKey)
{
	return	::RegDeleteKey(hKey[openCnt -1], subKey) == ERROR_SUCCESS;
}

BOOL TRegistry::DeleteValue(LPCTSTR subValue)
{
	return	::RegDeleteValue(hKey[openCnt -1], subValue) == ERROR_SUCCESS;
}

BOOL TRegistry::EnumKey(DWORD cnt, LPTSTR buf, int size)
{
	return	::RegEnumKeyEx(hKey[openCnt -1], cnt, buf, (DWORD *)&size, 0, 0, 0, 0)
			== ERROR_SUCCESS;
}

BOOL TRegistry::EnumValue(DWORD cnt, LPTSTR buf, int size, DWORD *type)
{
	return	::RegEnumValue(hKey[openCnt -1], cnt, buf, (DWORD *)&size, 0, type, 0, 0)
			== ERROR_SUCCESS;
}

BOOL TRegistry::DeleteChildTree(LPCTSTR subKey)
{
	TCHAR	wbuf[256];
	BOOL	ret = TRUE;

	if (subKey && OpenKey(subKey) == FALSE) {
		return	FALSE;
	}

	while (EnumKey(0, wbuf, sizeof(wbuf) / sizeof(TCHAR)))
	{
		if ((ret = DeleteChildTree(wbuf)) == FALSE)
			break;
	}
	if (subKey != NULL)
	{
		CloseKey();
		ret = DeleteKey(subKey) ? ret : FALSE;
	}
	else {
		while (EnumValue(0, wbuf, sizeof(wbuf) / sizeof(TCHAR)))
		{
			if (DeleteValue(wbuf) == FALSE)
			{
				ret = FALSE;
				break;
			}
		}
	}
	return	ret;
}


#endif