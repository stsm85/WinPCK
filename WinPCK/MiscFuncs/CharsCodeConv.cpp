#include <Windows.h>
#include "CharsCodeConv.h"

inline int AtoW(const char *src, WCHAR *dst, int bufsize, int max_len = -1)
{
	return	::MultiByteToWideChar(CP_ACP, 0, src, max_len, dst, bufsize);
}
inline int WtoA(const WCHAR *src, char *dst, int bufsize, int max_len = -1)
{
	return	::WideCharToMultiByte(CP_ACP, 0, src, max_len, dst, bufsize, "_", 0);
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
			m_buffer = malloc(len * sizeof(wchar_t));
			AtoW(_src, (wchar_t*)m_buffer, len);
		}
	}
	return	(wchar_t*)m_buffer;
}

int	CAnsi2Ucs::GetStrlen(const char *_src)
{
	return (AtoW(_src, NULL, 0) - 1);
}

const char * CUcs2Ansi::GetString(const wchar_t *_src, char *_dst, int _dstsize)//_dstsize是指数组元素个数
{

	if(NULL != _dst) {

		//assert(0 != _dstsize);
		WtoA(_src, _dst, _dstsize);

	} else {

		int		len;
		if((len = WtoA(_src, NULL, 0)) > 0) {
			m_buffer = malloc(len * sizeof(char));
			WtoA(_src, (char*)m_buffer, len);
		}
	}
	return	(char*)m_buffer;

}

int	CUcs2Ansi::GetStrlen(const wchar_t *_src)
{
	return (WtoA(_src, NULL, 0) - 1);
}