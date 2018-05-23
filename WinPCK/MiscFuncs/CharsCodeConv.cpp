#include <Windows.h>
#include "CharsCodeConv.h"

inline int AtoW(const char *src, WCHAR *dst, int bufsize, int max_len = -1)
{
	//返回的值是转换后的字符串使用strlen或wcslen的值+1
	return	::MultiByteToWideChar(CP_ACP, 0, src, max_len, dst, bufsize);
}
inline int WtoA(const WCHAR *src, char *dst, int bufsize, int max_len = -1)
{
	return	::WideCharToMultiByte(CP_ACP, 0, src, max_len, dst, bufsize, "_", 0);
}

inline int U8toW(const char *src, WCHAR *dst, int bufsize, int max_len = -1)
{
	//返回的值是转换后的字符串使用strlen或wcslen的值+1
	return	::MultiByteToWideChar(CP_UTF8, 0, src, max_len, dst, bufsize);
}
inline int WtoU8(const WCHAR *src, char *dst, int bufsize, int max_len = -1)
{
	return	::WideCharToMultiByte(CP_UTF8, 0, src, max_len, dst, bufsize, "_", 0);
}

CCharsCodeConv::CCharsCodeConv() : m_ok(FALSE), m_buffer(NULL)
{}

CCharsCodeConv::~CCharsCodeConv()
{
	if(NULL != m_buffer)free(m_buffer);
}

const wchar_t * CAnsi2Ucs::GetString(const char *_src, wchar_t *_dst, int _dstsize)
{
	if(NULL != _dst) {
		//assert(0 != _dstsize);
		AtoW(_src, _dst, _dstsize);
	} else {
		int		len;
		if((len = AtoW(_src, NULL, 0)) > 0) {
			if(NULL != m_buffer)free(m_buffer);
			m_buffer = malloc(len * sizeof(wchar_t));
			AtoW(_src, (wchar_t*)m_buffer, len);
		}
	}
	return	(wchar_t*)m_buffer;
}

int	CAnsi2Ucs::GetStrlen(const char *_src, wchar_t *_dst, int _dstsize)
{
	if(NULL != _dst) {
		//assert(0 != _dstsize);
		return (AtoW(_src, _dst, _dstsize) - 1);
	} else {
		return  (AtoW(_src, NULL, 0) - 1);
	}

}

const char * CUcs2Ansi::GetString(const wchar_t *_src, char *_dst, int _dstsize)//_dstsize是指数组元素个数
{

	if(NULL != _dst) {

		//assert(0 != _dstsize);
		WtoA(_src, _dst, _dstsize);

	} else {

		int		len;
		if((len = WtoA(_src, NULL, 0)) > 0) {
			if(NULL != m_buffer)free(m_buffer);
			m_buffer = malloc(len * sizeof(char));
			WtoA(_src, (char*)m_buffer, len);
		}
	}
	return	(char*)m_buffer;

}

int	CUcs2Ansi::GetStrlen(const wchar_t *_src, char *_dst, int _dstsize)
{
	if(NULL != _dst) {
		return (WtoA(_src, _dst, _dstsize) - 1);
	} else {
		return (WtoA(_src, NULL, 0) - 1);
	}
}

const char * CUcs2U8::GetString(const wchar_t *_src, char *_dst, int _dstsize)//_dstsize是指数组元素个数
{

	if(NULL != _dst) {
		//assert(0 != _dstsize);
		WtoU8(_src, _dst, _dstsize);
	} else {

		int		len;
		if((len = WtoU8(_src, NULL, 0)) > 0) {
			if(NULL != m_buffer)free(m_buffer);
			m_buffer = malloc(len * sizeof(char));
			WtoU8(_src, (char*)m_buffer, len);
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
		U8toW(_src, _dst, _dstsize);
	} else {
		int		len;
		if((len = U8toW(_src, NULL, 0)) > 0) {
			if(NULL != m_buffer)free(m_buffer);
			m_buffer = malloc(len * sizeof(wchar_t));
			U8toW(_src, (wchar_t*)m_buffer, len);
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
		return  (U8toW(_src, NULL, 0) - 1);
	}

}