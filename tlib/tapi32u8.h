/* @(#)Copyright (C) 1996-2010 H.Shirouzu		tapi32u8.h	Ver0.99 */
/* ========================================================================
	Project  Name			: Win32 Lightweight  Class Library Test
	Module Name				: Main Header
	Create					: 2005-04-10(Sun)
	Update					: 2010-05-09(Sun)
	Copyright				: H.Shirouzu
	Reference				: 
	======================================================================== */

#ifndef TAPI32U8_H
#define TAPI32U8_H

#ifdef _USE_T_U8FUNC_

// UTF8 string class
enum StrMode { BY_UTF8, BY_MBCS };

inline int WtoU8(const WCHAR *src, char *dst, int bufsize, int max_len=-1) {
	return	::WideCharToMultiByte(CP_UTF8, 0, src, max_len, dst, bufsize, 0, 0);
}
inline int U8toW(const char *src, WCHAR *dst, int bufsize, int max_len=-1) {
	return	::MultiByteToWideChar(CP_UTF8, 0, src, max_len, dst, bufsize);
}
inline int WtoS(LPCWSTR src, char *dst, int bufsize, StrMode mode, int max_len=-1) {
	return (mode == BY_UTF8) ? WtoU8(src, dst, bufsize, max_len)
							 : WtoA(src, dst, bufsize, max_len);
}

WCHAR *U8toW(const char *src, BOOL noStatic=FALSE);
char *WtoU8(const WCHAR *src, BOOL noStatic=FALSE);
char *WtoA(const WCHAR *src, BOOL noStatic=FALSE);
char *AtoU8(const char *src, BOOL noStatic=FALSE);
char *U8toA(const char *src, BOOL noStatic=FALSE);

// Win32(W) API UTF8 wrapper
BOOL GetMenuStringU8(HMENU hMenu, UINT uItem, char *buf, int bufsize, UINT flags);
DWORD GetModuleFileNameU8(HMODULE hModule, char *buf, DWORD bufsize);
UINT GetDriveTypeU8(const char *path);

class U8str {
	char	*s;
public:
	U8str(const WCHAR *_s=NULL) { s = _s ? WtoU8(_s, TRUE) : NULL; }
	U8str(const char *_s, StrMode mode=BY_UTF8) {
		s = _s ? mode == BY_UTF8 ? strdupNew(s) : AtoU8(_s, TRUE) : NULL;
	}
	U8str(int len) { if (len) { s = new char [len]; *s = 0; } else { s = NULL; } }
	~U8str() { delete [] s; }
	operator const char *() { return s; }
	char	*Buf() { return s; }
};

class Wstr {
	WCHAR	*s;
public:
	Wstr(const char *_s, StrMode mode=BY_UTF8) {
		s = _s ? mode == BY_UTF8 ? U8toW(_s, TRUE) : AtoW(_s, TRUE) : NULL;
	}
	Wstr(int len) { if (len) { s = new WCHAR [len]; *s = 0; } else { s = NULL; } }
	~Wstr() { delete [] s; }
	operator const WCHAR *() { return s; }
	operator const void *() { return s; }	// for V()
	WCHAR	*Buf() { return s; }
};

class MBCSstr {
	char	*s;
public:
	MBCSstr(const WCHAR *_s=NULL) { s = _s ? WtoA(_s, TRUE) : NULL; }
	MBCSstr(const char *_s, StrMode mode=BY_UTF8) {
		s = _s ? mode == BY_UTF8 ? U8toA(_s, TRUE) : strdupNew(s) : NULL;
	}
	MBCSstr(int len) { if (len) { s = new char [len]; *s = 0; } else { s = NULL; } }
	~MBCSstr() { delete [] s; }
	operator const char *() { return s; }
	char	*Buf() { return s; }
};

BOOL IsUTF8(const char *s);

#endif
#endif
