#pragma once

#include <codecvt>

#define TEXT_TYPE_ANSI	0
#define TEXT_TYPE_UCS2	1
#define TEXT_TYPE_UTF8	2
#define TEXT_TYPE_RAW	3

#ifdef _DEBUG
int AtoW(const char *src, wchar_t *dst, int bufsize, int max_len, int cp);
int WtoA(const wchar_t *src, char *dst, int bufsize, int max_len, int cp);
int U8toW(const char *src, wchar_t *dst, int bufsize, int max_len = -1);
int WtoU8(const wchar_t *src, char *dst, int bufsize, int max_len = -1);
#else
int AtoW(const char *src, wchar_t *dst, int bufsize, int max_len, int cp);
int WtoA(const wchar_t *src, char *dst, int bufsize, int max_len, int cp);
int U8toW(const char *src, wchar_t *dst, int bufsize, int max_len = -1);
int WtoU8(const wchar_t *src, char *dst, int bufsize, int max_len = -1);
#endif


//std::wstring AtoW(const std::string& src, int cp);
//std::string WtoA(const std::wstring& src, int cp);
//std::wstring U8toW(const std::string& src);
//std::string WtoU8(const std::wstring& src);

/*
返回值：TEXT_TYPE_ANSI-TEXT_TYPE_RAW
_s：如果不是以0xfeff或0xbfbbef开头，不修改，否则+2或+3后返回
*/
int	TextDataType(const char* &_s, size_t size);

class CCharsCodeConv
{
public:
	CCharsCodeConv();
	~CCharsCodeConv();
	//返回的长度包含了\0
	virtual int		GetConvertedStrLen() { return m_ConvertedStrLen - 1; }

protected:

	void *	m_buffer;
	int		m_ConvertedStrLen;
	int		m_ok;

};


class CAnsi2Ucs : public CCharsCodeConv
{
public:
	CAnsi2Ucs(int cp) : codepage(cp) {}
	virtual ~CAnsi2Ucs() {};

	const wchar_t *GetString(const char *_src, wchar_t *_dst = nullptr, int _dstsize = 0);	//ANSI和UNICODE互转
	int		GetStrlen(const char *_src, wchar_t *_dst = nullptr, int _dstsize = 0);	//ANSI字符在对应的UNICODE的wcslen下的返回值

private:
	int		codepage;
};

class CUcs2Ansi : public CCharsCodeConv
{
public:
	CUcs2Ansi(int cp) : codepage(cp) { }
	virtual ~CUcs2Ansi() {};

	const char *GetString(const wchar_t *_src, char *_dst = nullptr, int _dstsize = 0);
	int		GetStrlen(const wchar_t *_src, char *_dst = nullptr, int _dstsize = 0);	//UNICODE字符在对应的ANSI的strlen下的返回值

private:
	int		codepage;
};

class CU82Ucs : public CCharsCodeConv
{
public:
	CU82Ucs(){}
	virtual ~CU82Ucs() {};

	const wchar_t *GetString(const char *_src, wchar_t *_dst = nullptr, int _dstsize = 0);	//ANSI和UNICODE互转
	int		GetStrlen(const char *_src, wchar_t *_dst = nullptr, int _dstsize = 0);	//ANSI字符在对应的UNICODE的wcslen下的返回值
};

class CUcs2U8 : public CCharsCodeConv
{
public:
	CUcs2U8(){}
	virtual ~CUcs2U8() {};

	const char *GetString(const wchar_t *_src, char *_dst = nullptr, int _dstsize = 0);
	int		GetStrlen(const wchar_t *_src, char *_dst = nullptr, int _dstsize = 0);	//UNICODE字符在对应的ANSI的strlen下的返回值
};

class CTextConv2UCS2
{
public:
	CTextConv2UCS2();
	~CTextConv2UCS2();

	const wchar_t* GetUnicodeString(const char* _str, int _strlen);

private:
	char*		unicode_str;

	CAnsi2Ucs	cA2U;
	CU82Ucs		c82U;
};