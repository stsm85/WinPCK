#pragma once
#include "PckClassBaseFeatures.h"

#define CP936	936
#define PCK_INTERNAL_CODEPAGE	CP936

class CPckClassCodepage :
	protected virtual CPckClassBaseFeatures
{
public:
	CPckClassCodepage();
	~CPckClassCodepage();

protected:
	//文件名转码，返回值为转换后的字符串的长度(strlen,wcslen)
	//ansi -> uc cp936
	size_t	PckFilenameCode2UCS(const char *_in, wchar_t *_out, size_t _outsize);
	//uc -> ansi cp936
	size_t	PckFilenameCode2Ansi(const wchar_t *_in, char *_out, size_t _outsize);
	//ansi -> uc native
	size_t	NativeFilenameCode2UCS(const char *_in, wchar_t *_out, size_t _outsize);
	//uc -> ansi native
	size_t	NativeFilenameCode2Ansi(const wchar_t *_in, char *_out, size_t _outsize);

private:

};

