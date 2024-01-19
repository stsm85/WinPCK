#ifndef _CharsCodeConv_H_
#define _CharsCodeConv_H_

#define _USE_ICONV 0

#include <string>
#include <string_view>
#include <stdexcept>

#if _USE_ICONV
#include <iconv.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#ifndef CP_ACP
#define CP_ACP 0
#endif

#ifndef CP936
#define CP936 936
#endif

/// <summary>
/// 文字在GBK UTF8 和 UCS2编码之间转换
/// </summary>

template<uint32_t codepage>
class StringCodeConvT
{
public:
	StringCodeConvT() = default;
	StringCodeConvT(const StringCodeConvT& other) = delete;

	StringCodeConvT(StringCodeConvT&& other);

	~StringCodeConvT() = default;

	template <size_t _Size>
	StringCodeConvT(const char(&str)[_Size])
	{
		this->from_ansi(str, _Size - 1);
	}

	template <size_t _Size>
	StringCodeConvT(const char8_t(&str)[_Size])
	{
		this->from_utf8((const char*)str, _Size - 1);
	}

	template <size_t _Size>
	StringCodeConvT(const wchar_t(&str)[_Size])
	{
		this->from_wchar(str, _Size - 1);
	}

	StringCodeConvT(const std::string& str);
	StringCodeConvT(const std::u8string& str);
	StringCodeConvT(const std::wstring& str);

	StringCodeConvT&& copy_ansi(const std::string& str);
	StringCodeConvT&& copy_utf8(const std::string& str);
	StringCodeConvT&& copy_wchar(const std::wstring& str);

	template <size_t _Size>
	StringCodeConvT&& from_ansi(const char(&str)[_Size])
	{
		return this->from_ansi(str, _Size - 1);
	}

	template <size_t _Size>
	StringCodeConvT&& from_utf8(const char(&str)[_Size])
	{
		return this->from_utf8(str, _Size - 1);
	}

	template <size_t _Size>
	StringCodeConvT&& from_wchar(const wchar_t(&str)[_Size])
	{
		return this->from_wchar(str, _Size - 1);
	}

	StringCodeConvT&& from_ansi(const char* str, size_t _Size);
	StringCodeConvT&& from_utf8(const char* str, size_t _Size);
	StringCodeConvT&& from_wchar(const wchar_t* str, size_t _Size);

	StringCodeConvT&& from_ansi(const std::string& str);
	StringCodeConvT&& from_utf8(const std::string& str);
	StringCodeConvT&& from_wchar(const std::wstring& str);

	std::string to_ansi();
	std::string to_utf8();
	std::wstring to_wchar();

private:

	enum class CHAR_TYPE {
		NONE,
		ANSI,
		UTF8,
		UTF16LE
	};

public:
	int AtoW(const char* in, size_t insize, wchar_t* out, size_t outsize);
	int WtoA(const wchar_t* in, size_t insize, char* out, size_t outsize);

	int U8toW(const char* in, size_t insize, wchar_t* out, size_t outsize);
	int WtoU8(const wchar_t* in, size_t insize, char* out, size_t outsize);

private:
	void AtoW();
	void WtoA();
	void U8toW();
	void WtoU8();

	void U8toA();
	void AtoU8();

	std::string ansi_string;
	std::wstring wchar_string;
	std::string utf8_string;

	std::string_view ansi_string_v;
	std::wstring_view wchar_string_v;
	std::string_view utf8_string_v;

	CHAR_TYPE m_from_type = CHAR_TYPE::NONE;

#if _USE_ICONV
	constexpr auto codepage_str();
#else
	void assert_error(uint32_t dst, uint32_t src) const;
#endif

};

typedef StringCodeConvT<CP_ACP> StringCodeConv;
typedef StringCodeConvT<CP936> StringCodeConvChs;

#endif //_CharsCodeConv_H_