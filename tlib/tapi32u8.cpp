
/* ========================================================================
	Project  Name			: Win32 Lightweight  Class Library Test
	Module Name				: Application Frame Class
	Create					: 1996-06-01(Sat)
	Update					: 2010-05-09(Sun)
	Copyright				: H.Shirouzu
	Reference				: 
	======================================================================== */
#include "tlib.h"
#ifdef _USE_T_U8FUNC_
static char *tap32u8_id =
"@(#)Copyright (C) 1996-2010 H.Shirouzu		tap32u8.cpp	Ver0.99";

#include "tapi32u8.h"

#include <stdio.h>
#include <mbstring.h>
#include <stdlib.h>
#include <stddef.h>

/*=========================================================================
	UCS2(W) - UTF-8(U8) - ANSI(A) ‘ŠŒÝ•ÏŠ·
=========================================================================*/
WCHAR *U8toW(const char *src, BOOL noStatic) {
	static	WCHAR	*_wbuf = NULL;

	WCHAR	*wtmp = NULL;
	WCHAR	*&wbuf = noStatic ? wtmp : _wbuf;

	if (wbuf) {
		delete [] wbuf;
		wbuf = NULL;
	}

	int		len;
	if ((len = U8toW(src, NULL, 0)) > 0) {
		wbuf = new WCHAR [len];
		U8toW(src, wbuf, len);
	}
	return	wbuf;
}

char *WtoU8(const WCHAR *src, BOOL noStatic) {
	static	char	*_buf = NULL;

	char	*tmp = NULL;
	char	*&buf = noStatic ? tmp : _buf;

	if (buf) {
		delete [] buf;
		buf = NULL;
	}

	int		len;
	if ((len = WtoU8(src, NULL, 0)) > 0) {
		buf = new char [len];
		WtoU8(src, buf, len);
	}
	return	buf;
}

char *WtoA(const WCHAR *src, BOOL noStatic) {
	static	char	*_buf = NULL;

	char	*tmp = NULL;
	char	*&buf = noStatic ? tmp : _buf;

	if (buf) {
		delete [] buf;
		buf = NULL;
	}

	int		len;
	if ((len = WtoA(src, NULL, 0)) > 0) {
		buf = new char [len];
		WtoA(src, buf, len);
	}
	return	buf;
}

char *AtoU8(const char *src, BOOL noStatic) {
	static	char	*_buf = NULL;

	char	*tmp = NULL;
	char	*&buf = noStatic ? tmp : _buf;

	if (buf) {
		delete [] buf;
		buf = NULL;
	}

	WCHAR	*wsrc = AtoW(src, TRUE);
	if (wsrc) {
		buf = WtoU8(wsrc, TRUE);
	}
	delete [] wsrc;
	return	buf;
}

char *U8toA(const char *src, BOOL noStatic) {
	static	char	*_buf = NULL;

	char	*tmp = NULL;
	char	*&buf = noStatic ? tmp : _buf;

	if (buf) {
		delete [] buf;
		buf = NULL;
	}

	WCHAR	*wsrc = U8toW(src, TRUE);
	if (wsrc) {
		buf = WtoA(wsrc, TRUE);
	}
	delete [] wsrc;
	return	buf;
}


BOOL IsUTF8(const char *_s)
{
	const u_char *s = (const u_char *)_s;

	while (*s) {
		if (*s <= 0x7f) {
		}
		else if (*s <= 0xdf) {
			if ((*++s & 0xc0) != 0x80) return FALSE;
		}
		else if (*s <= 0xef) {
			if ((*++s & 0xc0) != 0x80) return FALSE;
			if ((*++s & 0xc0) != 0x80) return FALSE;
		}
		else if (*s <= 0xf7) {
			if ((*++s & 0xc0) != 0x80) return FALSE;
			if ((*++s & 0xc0) != 0x80) return FALSE;
			if ((*++s & 0xc0) != 0x80) return FALSE;
		}
		else if (*s <= 0xfb) {
			if ((*++s & 0xc0) != 0x80) return FALSE;
			if ((*++s & 0xc0) != 0x80) return FALSE;
			if ((*++s & 0xc0) != 0x80) return FALSE;
			if ((*++s & 0xc0) != 0x80) return FALSE;
		}
		else if (*s <= 0xfd) {
			if ((*++s & 0xc0) != 0x80) return FALSE;
			if ((*++s & 0xc0) != 0x80) return FALSE;
			if ((*++s & 0xc0) != 0x80) return FALSE;
			if ((*++s & 0xc0) != 0x80) return FALSE;
			if ((*++s & 0xc0) != 0x80) return FALSE;
		}
		s++;
	}
	return	TRUE;
}

#endif