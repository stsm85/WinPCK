#include <pch.h>
#include "PckClassCodepage.h"

#pragma region 文件名转码

//ansi -> uc cp936
size_t	CPckClassCodepage::PckFilenameCode2UCS(const char* _in, size_t _insize, wchar_t* _out, size_t _outsize)
{
	return StringCodeConvChs().AtoW(_in, _insize, _out, _outsize);
}
//uc -> ansi cp936
size_t	CPckClassCodepage::PckFilenameCode2Ansi(const wchar_t* _in, size_t _insize, char* _out, size_t _outsize)
{
	return StringCodeConvChs().WtoA(_in, _insize, _out, _outsize);
}