//////////////////////////////////////////////////////////////////////
// CharsCodeConv.cpp: WinPCK 辅助函数部分
// 将文字在Ansi Unicode UTF-8之间转换
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2018.5.29
//////////////////////////////////////////////////////////////////////

#include "CharsCodeConv.h"

int AtoW(const char *src, WCHAR *dst, int bufsize, int max_len, int cp)
{
	//返回的值是转换后的字符串使用strlen或wcslen的值+1
	return	::MultiByteToWideChar(cp, 0, src, max_len, dst, bufsize);
}
int WtoA(const WCHAR *src, char *dst, int bufsize, int max_len, int cp)
{
	return	::WideCharToMultiByte(cp, 0, src, max_len, dst, bufsize, "_", 0);
}

int U8toW(const char *src, WCHAR *dst, int bufsize, int max_len)
{
	//返回的值是转换后的字符串使用strlen或wcslen的值+1
	return	::MultiByteToWideChar(CP_UTF8, 0, src, max_len, dst, bufsize);
}
int WtoU8(const WCHAR *src, char *dst, int bufsize, int max_len)
{
	return	::WideCharToMultiByte(CP_UTF8, 0, src, max_len, dst, bufsize, NULL, 0);
}

#define DATATYPE_UTF8_DETECT_RTN {if(0 == *s) return TEXT_TYPE_RAW;else	{isNotUTF8 = TRUE; break;}}	

static int DataType(const char *_s, size_t size)
{
	const u_char *s = (const u_char *)_s;
	BOOL  isNotUTF8 = FALSE;
	 
	while (*s) {
		if (*s <= 0x7f) {
		}
		else if (*s <= 0xdf) {
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN
		}
		else if (*s <= 0xef) {
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
		}
		else if (*s <= 0xf7) {
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
		}
		else if (*s <= 0xfb) {
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
		}
		else if (*s <= 0xfd) {
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
		}
		++s;
	}

	while (*s) {
		++s;
	}

	//int a = (char*)s - _s;

	if (size > ((char*)s - _s))
		return TEXT_TYPE_RAW;
	else
		return isNotUTF8 ? TEXT_TYPE_ANSI : TEXT_TYPE_UTF8;

}

#undef DATATYPE_UTF8_DETECT_RTN

int	TextDataType(const char* &_s, size_t size)
{
	int textType = TEXT_TYPE_ANSI;
	if (0 != size) {

		*(WORD*)(_s + size) = 0;

		if (0xfeff == *(WORD*)_s) {

			textType = TEXT_TYPE_UCS2;
			_s += 2;
		}
		else if ((0xbbef == *(WORD*)_s) && (0xbf == *(_s + 3))) {

			textType = TEXT_TYPE_UTF8;
			_s += 3;
		}
		else {
			textType = DataType((char*)_s, size);
		}
	}
	return textType;
}
#pragma region CTextConv2UCS2

CTextConv2UCS2::CTextConv2UCS2():
	unicode_str(nullptr),
	cA2U(CP_ACP)
{
}

CTextConv2UCS2::~CTextConv2UCS2()
{
	if (nullptr != unicode_str)
		free(unicode_str);
}

const wchar_t* CTextConv2UCS2::GetUnicodeString(const char* _str, int _strlen)
{

	unicode_str = (char*)malloc(_strlen + 2);

	if (nullptr == unicode_str)
		return nullptr;

	*(unicode_str + _strlen) = 0;
	memcpy(unicode_str, _str, _strlen);

	const char* _realstrbuf = unicode_str;

	int textType = TextDataType(_realstrbuf, _strlen);

	switch (textType)
	{
	case TEXT_TYPE_ANSI:

		return cA2U.GetString(_realstrbuf);
		break;

	case TEXT_TYPE_UCS2:
		
		return (wchar_t*)_realstrbuf;
		break;

	case TEXT_TYPE_UTF8:
		return c82U.GetString(_realstrbuf);
		break;

	default:

		break;

	}

	return nullptr;
}

#pragma endregion

#pragma region CCharsCodeConv

CCharsCodeConv::CCharsCodeConv() : m_ok(FALSE), m_buffer(nullptr)
{}

CCharsCodeConv::~CCharsCodeConv()
{
	if(NULL != m_buffer)free(m_buffer);
}

const wchar_t * CAnsi2Ucs::GetString(const char *_src, wchar_t *_dst, int _dstsize)
{
	if(NULL != _dst) {
		//assert(0 != _dstsize);
		m_ConvertedStrLen = AtoW(_src, _dst, _dstsize, -1, codepage);
	} else {
		int		len;
		if((len = AtoW(_src, NULL, 0, -1, codepage)) > 0) {
			if(NULL != m_buffer)free(m_buffer);
			m_buffer = malloc(len * sizeof(wchar_t));
			m_ConvertedStrLen = AtoW(_src, (wchar_t*)m_buffer, len, -1, codepage);
		}
	}
	return	(wchar_t*)m_buffer;
}

int	CAnsi2Ucs::GetStrlen(const char *_src, wchar_t *_dst, int _dstsize)
{
	if(NULL != _dst) {
		//assert(0 != _dstsize);
		return (AtoW(_src, _dst, _dstsize, -1, codepage) - 1);
	} else {
		return (AtoW(_src, NULL, 0, -1, codepage) - 1);
	}

}

const char * CUcs2Ansi::GetString(const wchar_t *_src, char *_dst, int _dstsize)//_dstsize是指数组元素个数
{

	if(NULL != _dst) {

		//assert(0 != _dstsize);
		m_ConvertedStrLen = WtoA(_src, _dst, _dstsize, -1, codepage);

	} else {

		int		len;
		if((len = WtoA(_src, NULL, 0, -1, codepage)) > 0) {
			if(NULL != m_buffer)free(m_buffer);
			m_buffer = malloc(len * sizeof(char));
			m_ConvertedStrLen = WtoA(_src, (char*)m_buffer, len, -1, codepage);
		}
	}
	return	(char*)m_buffer;

}

int	CUcs2Ansi::GetStrlen(const wchar_t *_src, char *_dst, int _dstsize)
{
	if(NULL != _dst) {
		return (WtoA(_src, _dst, _dstsize, -1, codepage) - 1);
	} else {
		return (WtoA(_src, NULL, 0, -1, codepage) - 1);
	}
}

const char * CUcs2U8::GetString(const wchar_t *_src, char *_dst, int _dstsize)//_dstsize是指数组元素个数
{

	if(NULL != _dst) {
		//assert(0 != _dstsize);
		m_ConvertedStrLen = WtoU8(_src, _dst, _dstsize);
	} else {

		int		len;
		if((len = WtoU8(_src, NULL, 0)) > 0) {
			if(NULL != m_buffer)free(m_buffer);
			m_buffer = malloc(len * sizeof(char));
			m_ConvertedStrLen = WtoU8(_src, (char*)m_buffer, len);
		}
	}
	return	(char*)m_buffer;
}

int	CUcs2U8::GetStrlen(const wchar_t *_src, char *_dst, int _dstsize)
{
	if(NULL != _dst) {
		return (WtoU8(_src, _dst, _dstsize) - 1);
	} else {
		return (WtoU8(_src, NULL, 0) - 1);
	}
}

const wchar_t * CU82Ucs::GetString(const char *_src, wchar_t *_dst, int _dstsize)
{
	if(NULL != _dst) {
		//assert(0 != _dstsize);
		m_ConvertedStrLen = U8toW(_src, _dst, _dstsize);
	} else {
		int		len;
		if((len = U8toW(_src, NULL, 0)) > 0) {
			if(NULL != m_buffer)free(m_buffer);
			m_buffer = malloc(len * sizeof(wchar_t));
			m_ConvertedStrLen = U8toW(_src, (wchar_t*)m_buffer, len);
		}
	}
	return	(wchar_t*)m_buffer;
}

int	CU82Ucs::GetStrlen(const char *_src, wchar_t *_dst, int _dstsize)
{
	if(NULL != _dst) {
		//assert(0 != _dstsize);
		return (U8toW(_src, _dst, _dstsize) - 1);
	} else {
		return (U8toW(_src, NULL, 0) - 1);
	}

}

#pragma endregion