//////////////////////////////////////////////////////////////////////
// CharsCodeConv.h
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////

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

#define CHARTYPE2_IS_SAME	std::is_same<T, wchar_t>::value || \
							std::is_same<T, char>::value

#define CHARTYPE3_IS_SAME	std::is_same<T, wchar_t>::value || \
							std::is_same<T, char8_t>::value || \
							std::is_same<T, char>::value

#define CHARTYPE2_ENABLE_IF  std::enable_if_t<CHARTYPE2_IS_SAME, int> = 0
#define CHARTYPE3_ENABLE_IF  std::enable_if_t<CHARTYPE3_IS_SAME, int> = 0

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

	template <size_t _Size, typename T, CHARTYPE3_ENABLE_IF>
	StringCodeConvT(const T(&str)[_Size])
	{
		this->from(str, _Size - 1);
	}

	template <typename T, CHARTYPE3_ENABLE_IF>
	StringCodeConvT(const std::basic_string<T>& str)
	{
		this->from(str);
	}

	template <typename T, CHARTYPE3_ENABLE_IF>
	StringCodeConvT(const std::basic_string_view<T>& str)
	{
		this->from(str);
	}

	template <size_t _Size, typename T, CHARTYPE3_ENABLE_IF>
	StringCodeConvT&& from(const T(&str)[_Size])
	{
		return this->from(str, _Size - 1);
	}

	template <typename T, CHARTYPE3_ENABLE_IF>
	StringCodeConvT&& from(const T* str, const size_t _Size)
	{
		this->m_from_type = this->chartype<T>();

		if constexpr (std::is_same<T, char>::value)
			this->ansi_string_v = std::move(std::string_view((const char*)str, _Size));
		else if constexpr (std::is_same<T, wchar_t>::value)
			this->wchar_string_v = std::move(std::wstring_view((const wchar_t*)str, _Size));
		else if constexpr (std::is_same<T, char8_t>::value)
			this->utf8_string_v = std::move(std::string_view((const char*)str, _Size));

		return std::move(*this);
	}

	template <typename T, CHARTYPE3_ENABLE_IF>
	StringCodeConvT&& from(const std::basic_string<T>& str)
	{
		return this->from(str.data(), str.size());
	}

	template <typename T, CHARTYPE3_ENABLE_IF>
	StringCodeConvT&& from(const std::basic_string_view<T>& str)
	{
		return this->from(str.data(), str.size());
	}

	template <typename T, CHARTYPE3_ENABLE_IF>
	StringCodeConvT&& copyfrom(const T* str, const size_t _Size)
	{
		this->m_from_type = this->chartype<T>();

		if constexpr (std::is_same<T, char>::value) {
			this->ansi_string.assign((const char*)str, _Size);
			this->ansi_string_v = this->ansi_string;
		}
		else if constexpr (std::is_same<T, wchar_t>::value) {
			this->wchar_string.assign((const wchar_t*)str, _Size);
			this->wchar_string_v = this->wchar_string;
		}
		else if constexpr (std::is_same<T, char8_t>::value) {
			this->utf8_string.assign((const char*)str, _Size);
			this->utf8_string_v = this->utf8_string;
		}
		return std::move(*this);
	}

	template <typename T, CHARTYPE3_ENABLE_IF>
	StringCodeConvT&& copyfrom(const std::basic_string_view<T>& str)
	{
		return this->copyfrom(str.data(), str.size());
	}

	StringCodeConvT&& copyfrom_utf8(const std::string_view& str);

	template <size_t _Size>
	StringCodeConvT&& from_utf8(const char(&str)[_Size])
	{
		return this->from((const char8_t*)str, _Size - 1);
	}

	StringCodeConvT&& from_utf8(const char* str, size_t _Size);
	StringCodeConvT&& from_utf8(const std::string_view& str);

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

	template <typename T, CHARTYPE3_ENABLE_IF>
	constexpr CHAR_TYPE chartype() const
	{
		if constexpr (std::is_same<T, char>::value)
			return CHAR_TYPE::ANSI;
		else if constexpr (std::is_same<T, wchar_t>::value)
			return CHAR_TYPE::UTF16LE;
		else if constexpr (std::is_same<T, char8_t>::value)
			return CHAR_TYPE::UTF8;
	}

public:
	int AtoW(const char* in, const size_t insize, wchar_t* out, const size_t outsize);
	int WtoA(const wchar_t* in, const size_t insize, char* out, const size_t outsize);

	int U8toW(const char* in, const size_t insize, wchar_t* out, const size_t outsize);
	int WtoU8(const wchar_t* in, const size_t insize, char* out, const size_t outsize);

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