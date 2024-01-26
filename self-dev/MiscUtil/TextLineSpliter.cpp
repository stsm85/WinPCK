//////////////////////////////////////////////////////////////////////
// TextLineSpliter.cpp
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////

#include "TextLineSpliter.h"
#include <assert.h>

#define MAX_LINE_LENGTH (MAX_PATH + 10)

constexpr char CR = 0x0D;
constexpr char LF = 0x0A;

//code from Notepad++ project
//Copyright (C)2003 Don HO <don.h@free.fr>
enum class EolType : std::uint8_t
{
	windows,
	macos,
	unix,

	// special values
	unknown, // can not be the first value for legacy code
	osdefault = windows,
};

template<class T>
static EolType getEOLFormatForm(const T* data, size_t length, EolType defvalue = EolType::osdefault)
{
	//assert(length == 0 or data != nullptr && "invalid buffer for getEOLFormatForm()");

	for (size_t i = 0; i != length; ++i)
	{
		if (data[i] == CR)
		{
			if (i + 1 < length && data[i + 1] == LF)
				return EolType::windows;

			return EolType::macos;
		}

		if (data[i] == LF)
			return EolType::unix;
	}

	return defvalue; // fallback unknown
}

template<class T, class Ts, class Tv>
Tv CTextUnits<T, Ts, Tv>::Trim(const T* s, size_t _Size)
{
	Tv str_v(s, _Size);
	TrimLeftInteral(str_v);
	TrimRightInteral(str_v);
	return std::move(str_v);
}

template<class T, class Ts, class Tv>
Tv CTextUnits<T, Ts, Tv>::Trim(const Ts& s)
{
	return Trim(s.data(), s.size());
}

template<class T, class Ts, class Tv>
Tv CTextUnits<T, Ts, Tv>::TrimLeft(const T* s, size_t _Size)
{
	Tv str_v(s, _Size);
	TrimLeftInteral(str_v);
	return std::move(str_v);
}

template<class T, class Ts, class Tv>
Tv CTextUnits<T, Ts, Tv>::TrimLeft(const Ts& s)
{
	return TrimLeft(s.data(), s.size());
}

template<class T, class Ts, class Tv>
Tv CTextUnits<T, Ts, Tv>::TrimRight(const T* s, size_t _Size)
{
	Tv str_v(s, _Size);
	TrimRightInteral(str_v);
	return std::move(str_v);
}

template<class T, class Ts, class Tv>
Tv CTextUnits<T, Ts, Tv>::TrimRight(const Ts& s)
{
	return TrimRight(s.data(), s.size());
}

template<class T, class Ts, class Tv>
void CTextUnits<T, Ts, Tv>::TrimLeftInteral(Tv& s)
{
	while (0 < s.size() && 256 > (uint16_t)s.front() && std::isspace((uint8_t)s.front()))
	{
		s.remove_prefix(1);
	}
}

template<class T, class Ts, class Tv>
void CTextUnits<T, Ts, Tv>::TrimRightInteral(Tv& s)
{

	while (0 < s.size() && 256 > (uint16_t)s.back() && std::isspace((uint8_t)s.back()))
	{
		s.remove_suffix(1);
	}
}


template<class T, class Ts, class Tv>
inline void CTextUnits<T, Ts, Tv>::ProcessLine(Tv newline, std::vector<Tv>& v, SPLIT_FLAG flag)
{
	if (SPLIT_FLAG::NONE != flag) {
		if (SPLIT_FLAG::TRIM_LEFT == (flag & SPLIT_FLAG::TRIM_LEFT)) {
			TrimLeftInteral(newline);
		}

		if (SPLIT_FLAG::TRIM_RIGHT == (flag & SPLIT_FLAG::TRIM_RIGHT)) {
			TrimRightInteral(newline);
		}

		if (SPLIT_FLAG::DELETE_NULL_LINE == (flag & SPLIT_FLAG::DELETE_NULL_LINE)) {
			if (0 == newline.size())
				return;
		}
	}

	v.push_back(std::move(newline));
}

template<class T, class Ts, class Tv>
std::vector<Tv> CTextUnits<T, Ts, Tv>::Split(const Tv& s, const Ts c, SPLIT_FLAG flag)
{
	size_t pos1 = 0;
	size_t len = s.size();
	auto pos2 = s.find(c);

	std::vector<Tv> v;

	while (std::basic_string<T>::npos != pos2)
	{
		ProcessLine(s.substr(pos1, pos2 - pos1), v, flag);

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);

	}
	if (pos1 != len)
		ProcessLine(s.substr(pos1), v, flag);

	return std::move(v);
}


template<class T, class Ts, class Tv>
std::vector<Tv> CTextUnits<T, Ts, Tv>::Split(const Tv& s, const Ts c)
{
	return std::move(CTextUnits<T, Ts, Tv>::Split(s, c, SPLIT_FLAG::NONE));
}


template<class T, class Ts, class Tv>
std::vector<Ts> CTextUnits<T, Ts, Tv>::SplitRegEx(const Ts& input, const Ts c, int flag)
{
	std::basic_regex<T> re(c);
	std::regex_token_iterator<Ts::const_iterator> p(input.begin(), input.end(), re, -1);
	std::regex_token_iterator<Ts::const_iterator> end;
	std::vector<Ts> vec;
	while (p != end)
		vec.emplace_back(*p++);

	return vec;
}

template<class T, class Ts, class Tv>
void CTextUnits<T, Ts, Tv>::GetEolChars(const Tv& _str, Ts& c)
{
	EolType eolType = getEOLFormatForm(_str.data(), _str.size());
	switch (eolType)
	{
	case EolType::windows: c = { CR, LF }; break;
	case EolType::macos:   c = { CR }; break;
	case EolType::unix:    c = { LF }; break;
	default: c = { CR, LF }; break;
	}
}

template<class T, class Ts, class Tv>
std::vector<Tv> CTextUnits<T, Ts, Tv>::SplitLine(const T* _src, size_t size, SPLIT_FLAG flag)
{
	Tv _src_v(_src, size);
	return std::move(SplitLine(_src_v, flag));
}

template<class T, class Ts, class Tv>
std::vector<Tv> CTextUnits<T, Ts, Tv>::SplitLine(const Ts& _src, SPLIT_FLAG flag)
{
	Tv _src_v = _src;
	return std::move(SplitLine(_src_v, flag));
}

template<class T, class Ts, class Tv>
std::vector<Tv> CTextUnits<T, Ts, Tv>::SplitLine(const Tv& _src, SPLIT_FLAG flag)
{
	Ts eolChars;
	GetEolChars(_src, eolChars);
	return std::move(Split(_src, eolChars, flag));
}

template<class T, class Ts, class Tv>
Ts CTextUnits<T, Ts, Tv>::Replace(const Ts& str, const Ts& src, const Ts& dest)
{
	Ts ret;

	size_t pos_begin = 0;
	auto pos = str.find(src);
	size_t len = src.length();
	while (pos != Ts::npos)
	{
		ret.append(str.data() + pos_begin, pos - pos_begin);
		ret += dest;
		pos_begin = pos + len;
		pos = str.find(src, pos_begin);
	}
	if (pos_begin < str.length())
	{
		ret.append(str.begin() + pos_begin, str.end());
	}
	return ret;
}

template<class T, class Ts, class Tv>
Ts CTextUnits<T, Ts, Tv>::Replace(const Ts& str, const T src, const T dest)
{
	Ts ret(str);

	size_t pos_begin = 0;
	auto pos = ret.find(src);
	while (pos != Ts::npos)
	{
		ret[pos] = dest;
		pos_begin = pos + 1;
		pos = str.find(src, pos_begin);
	}

	return ret;
}

template class CTextUnits<char>;
template class CTextUnits<wchar_t>;
