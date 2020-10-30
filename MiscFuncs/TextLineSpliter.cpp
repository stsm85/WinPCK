#include "TextLineSpliter.h"
#include <assert.h>

#define MAX_LINE_LENGTH (MAX_PATH + 10)

static const int CR = 0x0D;
static const int LF = 0x0A;

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

template<class T>
std::basic_string<T> CTextUnits<T>::Trim(std::basic_string<T>& s)
{
	TrimLeft(s);
	TrimRight(s);
	return s;
}

template<class T>
std::basic_string<T> CTextUnits<T>::TrimLeft(std::basic_string<T>& s)
{
	auto pos = s.find_first_not_of({ ' ','\t' });
	if (std::basic_string<T>::npos != pos)
	{
		s.erase(0, pos);
	}
	return s;
}

template<class T>
std::basic_string<T> CTextUnits<T>::TrimRight(std::basic_string<T>& s)
{
	auto pos = s.find_last_not_of({ ' ','\t' });
	if (std::basic_string<T>::npos != pos)
	{
		s.erase(pos + 1);
	}
	return s;
}

template<class T>
void CTextUnits<T>::ProcessLine(std::basic_string<T> newline, std::vector<std::basic_string<T>>& v, int flag)
{
	if (flag & LINE_TRIM_LEFT) {

		TrimLeft(newline);
		//auto pos = newline.find_first_not_of(' ');
		//if (std::basic_string<T>::npos != pos)
		//{
		//	newline.erase(0, pos);
		//}
	}

	if (flag & LINE_TRIM_RIGHT) {

		TrimRight(newline);
		//auto pos = newline.find_last_not_of(' ');
		//if (std::basic_string<T>::npos != pos)
		//{
		//	newline.erase(pos + 1);
		//}
	}

	if (flag & LINE_EMPTY_DELETE) {
		if (0 == newline.size())
			return;
	}

	v.emplace_back(newline);
}

template<class T>
void CTextUnits<T>::Split(const std::basic_string<T>& s, std::vector<std::basic_string<T>>& v, const std::basic_string<T> c, int flag)
{
	size_t pos1;
	size_t len = s.length();
	auto pos2 = s.find(c);
	pos1 = 0;
	while (std::basic_string<T>::npos != pos2)
	{
		ProcessLine(s.substr(pos1, pos2 - pos1), v, flag);

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);

	}
	if (pos1 != len)
		ProcessLine(s.substr(pos1), v, flag);
}

template<class T>
void CTextUnits<T>::Split(const std::basic_string<T>& s, std::vector<std::basic_string<T>>& v, const std::basic_string<T> c)
{
	size_t pos1, pos2;
	size_t len = s.length();
	pos2 = s.find(c);
	pos1 = 0;
	while (std::basic_string<T>::npos != pos2)
	{
		v.emplace_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);

	}
	if (pos1 != len)
		v.emplace_back(s.substr(pos1));
}

template<class T>
std::vector<std::basic_string<T>> CTextUnits<T>::SplitRegEx(const std::basic_string<T> input, const std::basic_string<T> c, int flag)
{
	std::basic_regex<T> re(c);
	std::regex_token_iterator<std::basic_string<T>::const_iterator> p(input.begin(), input.end(), re, -1);
	std::regex_token_iterator<std::basic_string<T>::const_iterator> end;
	std::vector<std::basic_string<T>> vec;
	while (p != end)
		vec.emplace_back(*p++);

	return vec;
}

template<class T>
void CTextUnits<T>::GetEolChars(const std::basic_string<T>& _str, std::basic_string<T>& c)
{
	EolType eolType = getEOLFormatForm(_str.c_str(), _str.size());
	switch (eolType)
	{
	case EolType::windows: c = {CR, LF}; break;
	case EolType::macos:   c = {CR}; break;
	case EolType::unix:    c = {LF}; break;
	default: c = { CR, LF }; break;
	}
}

template<class T>
void CTextUnits<T>::SplitLine(const std::basic_string<T> _src, std::vector<std::basic_string<T>>&splitedLine, int flag)
{
	std::basic_string<T> eolChars;
	GetEolChars(_src, eolChars);
	Split(_src, splitedLine, eolChars, flag);
}

template<class T>
std::basic_string<T> CTextUnits<T>::Replace(const std::basic_string<T>& str, const std::basic_string<T>& src, const std::basic_string<T>& dest)
{
	std::basic_string<T> ret;

	size_t pos_begin = 0;
	auto pos = str.find(src);
	size_t len = src.length();
	while (pos != std::basic_string<T>::npos)
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

template class CTextUnits<char>;
template class CTextUnits<wchar_t>;
