//////////////////////////////////////////////////////////////////////
// CharsCodeConv.cpp: WinPCK 辅助函数部分
// 将文字在Ansi Unicode UTF-8之间转换
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2022.4.3
//////////////////////////////////////////////////////////////////////
//#include "pch.h"
#include "CharsCodeConv.h"
#include <stdint.h>
#include <assert.h>

constexpr char CH_INVALID = '_';

template<uint32_t codepage>
StringCodeConvT<codepage>::StringCodeConvT(StringCodeConvT&& other) {
	this->m_from_type = other.m_from_type;
	this->ansi_string = std::move(other.ansi_string);
	this->utf8_string = std::move(other.utf8_string);
	this->wchar_string = std::move(other.wchar_string);

	this->ansi_string_v = std::move(other.ansi_string_v);
	this->utf8_string_v = std::move(other.utf8_string_v);
	this->wchar_string_v = std::move(other.wchar_string_v);
}

template<uint32_t codepage>
StringCodeConvT<codepage>&& StringCodeConvT<codepage>::copyfrom_utf8(const std::string_view& str)
{
	return this->copyfrom((const char8_t*)str.data(), str.size());
}

template<uint32_t codepage>
StringCodeConvT<codepage>&& StringCodeConvT<codepage>::from_utf8(const char* str, size_t _Size)
{
	return this->from((const char8_t*)str, _Size);
}

template<uint32_t codepage>
StringCodeConvT<codepage>&& StringCodeConvT<codepage>::from_utf8(const std::string_view& str)
{
	return this->from((const char8_t*)str.data(), str.size());
}

#if _USE_ICONV

template<uint32_t codepage>
constexpr auto StringCodeConvT<codepage>::codepage_str()
{
	switch (codepage)
	{
	case CP936:
		return "GBK";
	case CP_ACP:
	default:
		return "";
	}
}

template<uint32_t codepage>
void StringCodeConvT<codepage>::AtoW()
{
	/*
	//IGNORE
	//TRANSLIT
	*/
	auto cd = iconv_open("UTF-16LE", this->codepage_str());

	if (cd == 0)
		throw std::runtime_error("iconv_open error");

	this->wchar_string.resize(this->ansi_string_v.size());

	const char* in = this->ansi_string_v.data();
	char* out = (char*)this->wchar_string.data();

	size_t in_size = this->ansi_string_v.size();
	size_t out_size = this->wchar_string.size() * sizeof(wchar_t);

	while (0 != iconv(cd, &in, &in_size, &out, &out_size) && 0 != in_size)
	{
		//
		if (0 != (0x80 & *in)) [[likely]]
			{
				in += 2;
				in_size -= 2;
			}
		else {
			in++;
			in_size--;
		}
		*(uint16_t*)out = CH_INVALID;
		out += sizeof(wchar_t);
		out_size -= sizeof(wchar_t);
	}
	iconv_close(cd);

	auto real_size = this->wchar_string.size() - out_size / sizeof(wchar_t);
	this->wchar_string.resize(real_size);
}

template<uint32_t codepage>
void StringCodeConvT<codepage>::WtoA()
{
	/*
	//IGNORE
	//TRANSLIT
	*/
	auto cd = iconv_open(this->codepage_str()/*"GBK//TRANSLIT"*/, "UTF-16LE");

	if (cd == 0)
		throw std::runtime_error("iconv_open error");

	this->ansi_string.resize(this->wchar_string_v.size() * sizeof(wchar_t));

	const char* in = (const char*)this->wchar_string_v.data();
	char* out = (char*)this->ansi_string.data();

	size_t in_size = this->wchar_string_v.size() * sizeof(wchar_t);
	size_t out_size = this->ansi_string.size();

	while (0 != iconv(cd, &in, &in_size, &out, &out_size) && 0 != in_size)
	{
		in += sizeof(wchar_t);
		in_size -= sizeof(wchar_t);

		*out = CH_INVALID;
		out++;
		out_size--;
	}
	iconv_close(cd);

	auto real_size = this->ansi_string.size() - out_size;
	this->ansi_string.resize(real_size);
}

template<uint32_t codepage>
void StringCodeConvT<codepage>::U8toW()
{
	auto cd = iconv_open("UTF-16LE//IGNORE", "UTF-8");

	if (cd == 0)
		throw std::runtime_error("iconv_open error");

	this->wchar_string.resize(this->utf8_string_v.size());

	const char* in = (const char*)this->utf8_string_v.data();
	char* out = (char*)this->wchar_string.data();

	size_t in_size = this->utf8_string_v.size();
	size_t out_size = this->wchar_string.size() * sizeof(wchar_t);


	auto ret = iconv(cd, &in, &in_size, &out, &out_size);

	assert(0 != ret);

	iconv_close(cd);

	auto real_size = this->wchar_string.size() - out_size / sizeof(wchar_t);
	this->wchar_string.resize(real_size);
}

template<uint32_t codepage>
void StringCodeConvT<codepage>::WtoU8()
{

	auto cd = iconv_open("UTF-8", "UTF-16LE");

	if (cd == 0)
		throw std::runtime_error("iconv_open error");

	this->utf8_string.resize(this->wchar_string_v.size() * 3);

	const char* in = (const char*)this->wchar_string_v.data();
	char* out = (char*)this->utf8_string.data();

	size_t in_size = this->wchar_string_v.size() * sizeof(wchar_t);
	size_t out_size = this->utf8_string.size();


	auto ret = iconv(cd, &in, &in_size, &out, &out_size);

	assert(0 != ret);

	iconv_close(cd);

	auto real_size = this->utf8_string.size() - out_size;
	this->utf8_string.resize(real_size);
}

template<uint32_t codepage>
void StringCodeConvT<codepage>::U8toA()
{

	auto utf8_bytes = [](unsigned char s)
		{
			if (s <= 0x7f) {
				return 1;
			}
			else if (s <= 0xdf) {
				return 2;
			}
			else if (s <= 0xef) [[likely]] {
				return 3;
				}
			else if (s <= 0xf7) {
				return 4;
			}
			else if (s <= 0xfb) {
				return 5;
			}
			else if (s <= 0xfd) {
				return 6;
			}
			return 1;
		};

	auto cd = iconv_open(this->codepage_str()/*"//TRANSLIT"*/, "UTF-8");

	if (cd == 0)
		throw std::runtime_error("iconv_open error");

	this->ansi_string.resize(this->utf8_string_v.size());

	const char* in = (const char*)this->utf8_string_v.data();
	char* out = (char*)this->ansi_string.data();

	size_t in_size = this->utf8_string_v.size();
	size_t out_size = this->ansi_string.size();


	while (0 != iconv(cd, &in, &in_size, &out, &out_size) && 0 != in_size)
	{
		auto bytes = utf8_bytes(*(unsigned char*)in);
		in += bytes;
		in_size -= bytes;

		*out = CH_INVALID;
		out++;
		out_size--;
	}

	iconv_close(cd);

	auto real_size = this->ansi_string.size() - out_size;
	this->ansi_string.resize(real_size);
}

template<uint32_t codepage>
void StringCodeConvT<codepage>::AtoU8()
{

	auto cd = iconv_open("UTF-8", this->codepage_str());

	if (cd == 0)
		throw std::runtime_error("iconv_open error");

	this->utf8_string.resize(this->ansi_string_v.size() * 3);

	const char* in = (const char*)this->ansi_string_v.data();
	char* out = (char*)this->utf8_string.data();

	size_t in_size = this->ansi_string_v.size();
	size_t out_size = this->utf8_string.size();

	auto ret = iconv(cd, &in, &in_size, &out, &out_size);

	assert(0 != ret);
	iconv_close(cd);

	auto real_size = this->utf8_string.size() - out_size;
	this->utf8_string.resize(real_size);
}

#else //WINDOWS API

template<uint32_t codepage>
int StringCodeConvT<codepage>::AtoW(const char* in, const size_t insize, wchar_t* out, const size_t outsize)
{
	return ::MultiByteToWideChar(codepage, 0, in, insize, out, outsize);
}

template<uint32_t codepage>
int StringCodeConvT<codepage>::WtoA(const wchar_t* in, const size_t insize, char* out, const size_t outsize)
{
	return ::WideCharToMultiByte(codepage, 0, in, insize, out, outsize, "_", 0);
}

template<uint32_t codepage>
int StringCodeConvT<codepage>::U8toW(const char* in, const size_t insize, wchar_t* out, const size_t outsize)
{
	return ::MultiByteToWideChar(CP_UTF8, 0, in, insize, out, outsize);
}

template<uint32_t codepage>
int StringCodeConvT<codepage>::WtoU8(const wchar_t* in, const size_t insize, char* out, const size_t outsize)
{
	return ::WideCharToMultiByte(CP_UTF8, 0, in, insize, out, outsize, nullptr, 0);
}

template<uint32_t codepage>
void StringCodeConvT<codepage>::AtoW()
{
	this->wchar_string.resize(this->ansi_string_v.size());

	auto len = ::MultiByteToWideChar(codepage, 0, this->ansi_string_v.data(), this->ansi_string_v.size(), this->wchar_string.data(), this->wchar_string.size());
	this->wchar_string.resize(len);

	this->wchar_string_v = this->wchar_string;

	this->assert_error(len, this->ansi_string.size());
}

template<uint32_t codepage>
void StringCodeConvT<codepage>::WtoA()
{
	this->ansi_string.resize(this->wchar_string_v.size() * 2);
	auto len = ::WideCharToMultiByte(codepage, 0, this->wchar_string_v.data(), this->wchar_string_v.size(), this->ansi_string.data(), this->ansi_string.size(), "_", 0);
	this->ansi_string.resize(len);

	this->ansi_string_v = this->ansi_string;

	this->assert_error(len, this->wchar_string.size());
}

template<uint32_t codepage>
void StringCodeConvT<codepage>::U8toW()
{

	this->wchar_string.resize(this->utf8_string_v.size());

	auto len = ::MultiByteToWideChar(CP_UTF8, 0, this->utf8_string_v.data(), this->utf8_string_v.size(), this->wchar_string.data(), this->wchar_string.size());
	this->wchar_string.resize(len);

	this->wchar_string_v = this->wchar_string;

	this->assert_error(len, this->utf8_string.size());

}

template<uint32_t codepage>
void StringCodeConvT<codepage>::WtoU8()
{
	this->utf8_string.resize(this->wchar_string_v.size() * 3);
	auto len = ::WideCharToMultiByte(CP_UTF8, 0, this->wchar_string_v.data(), this->wchar_string_v.size(), this->utf8_string.data(), this->utf8_string.size(), nullptr, 0);
	this->utf8_string.resize(len);

	this->utf8_string_v = this->utf8_string;

	this->assert_error(len, this->wchar_string.size());
}

template<uint32_t codepage>
void StringCodeConvT<codepage>::U8toA()
{
	this->U8toW();
	this->WtoA();
}

template<uint32_t codepage>
void StringCodeConvT<codepage>::AtoU8()
{
	this->AtoW();
	this->WtoU8();
}

template<uint32_t codepage>
void StringCodeConvT<codepage>::assert_error(uint32_t dst, uint32_t src) const
{
	if (0 == dst && 0 != src)
	{
		std::string errmsg("[StringCodeConv] Unknown error.");
		auto e = ::GetLastError();
		if (0 != e)
		{
			LPVOID lpMsgBuf; // temporary message buffer
			std::string szMessage;

			// retrieve a message from the system message table
			if (::FormatMessageA(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				e,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPSTR)&lpMsgBuf,
				0,
				NULL))
			{
				errmsg.assign("[StringCodeConv] ");
				errmsg.append((char*)lpMsgBuf);
				LocalFree(lpMsgBuf);
			}
		}

		throw std::runtime_error(errmsg);
	}
}

#endif

template<uint32_t codepage>
std::string StringCodeConvT<codepage>::to_ansi()
{

	switch (this->m_from_type)
	{
	[[likely]] case CHAR_TYPE::UTF16LE:
		this->WtoA();
		break;
	case CHAR_TYPE::UTF8:
		this->U8toA();
		break;
	[[unlikely]] case CHAR_TYPE::NONE:
	[[unlikely]] case CHAR_TYPE::ANSI:
		throw std::runtime_error("convert route is invalid");
		break;
	}
	return std::move(this->ansi_string);
}

template<uint32_t codepage>
std::string StringCodeConvT<codepage>::to_utf8()
{
	switch (this->m_from_type)
	{
	[[likely]] case CHAR_TYPE::UTF16LE:
		this->WtoU8();
		break;
	case CHAR_TYPE::ANSI:
		this->AtoU8();
		break;
	[[unlikely]] case CHAR_TYPE::NONE:
	[[unlikely]] case CHAR_TYPE::UTF8:
		throw std::runtime_error("convert route is invalid");
		break;
	}

	return std::move(this->utf8_string);
}

template<uint32_t codepage>
std::wstring StringCodeConvT<codepage>::to_wchar()
{

	switch (this->m_from_type)
	{
	case CHAR_TYPE::UTF8:
		this->U8toW();
		break;
	[[likely]] case CHAR_TYPE::ANSI:
		this->AtoW();
		break;
	[[unlikely]] case CHAR_TYPE::NONE:
	[[unlikely]] case CHAR_TYPE::UTF16LE:
		throw std::runtime_error("convert route is invalid");
		break;
	}

	return std::move(this->wchar_string);
}

template class StringCodeConvT<CP_ACP>;
template class StringCodeConvT<CP936>;
