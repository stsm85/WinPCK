#include "PckClassCodepage.h"

#pragma region 文件名转码
#include "CharsCodeConv.h"
//ansi -> uc cp936
size_t  CPckClassCodepage::PckFilenameCode2UCS(const char *_in, wchar_t *_out, size_t _outsize)
{
	CAnsi2Ucs cAnsi2Ucs(CP936);
	if (NULL != _out) {
		cAnsi2Ucs.GetString(_in, _out, _outsize);
		return cAnsi2Ucs.GetConvertedStrLen();
	}
	else {
		return cAnsi2Ucs.GetStrlen(_in);
	}
}
//uc -> ansi cp936
size_t  CPckClassCodepage::PckFilenameCode2Ansi(const wchar_t *_in, char *_out, size_t _outsize)
{
	CUcs2Ansi cUcs2Ansi(CP936);
	if (NULL != _out) {
		cUcs2Ansi.GetString(_in, _out, _outsize);
		return cUcs2Ansi.GetConvertedStrLen();
	}
	else {
		return cUcs2Ansi.GetStrlen(_in);
	}
}
size_t  CPckClassCodepage::NativeFilenameCode2UCS(const char *_in, wchar_t *_out, size_t _outsize)
{
	CAnsi2Ucs cAnsi2Ucs(CP_ACP);
	if (NULL != _out) {
		cAnsi2Ucs.GetString(_in, _out, _outsize);
		return cAnsi2Ucs.GetConvertedStrLen();
	}
	else {
		return cAnsi2Ucs.GetStrlen(_in);
	}
}

size_t  CPckClassCodepage::NativeFilenameCode2Ansi(const wchar_t *_in, char *_out, size_t _outsize)
{
	CUcs2Ansi cUcs2Ansi(CP_ACP);
	if (NULL != _out) {
		cUcs2Ansi.GetString(_in, _out, _outsize);
		return cUcs2Ansi.GetConvertedStrLen();
	}
	else {
		return cUcs2Ansi.GetStrlen(_in);
	}
}
#pragma endregion