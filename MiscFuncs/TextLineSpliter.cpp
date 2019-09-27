#include "TextLineSpliter.h"
#include <assert.h>

//#include "CharsCodeConv.h"

#define MAX_LINE_LENGTH (MAX_PATH + 10)

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
static EolType getEOLFormatForm(const T* const data, size_t length, EolType defvalue = EolType::osdefault)
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


static const int CR = 0x0D;
static const int LF = 0x0A;

template<class T, class R, class RI>
void CTextUnits<T, R, RI>::ProcessLine(T& newline, std::vector<T>& v, int flag)
{
	if (flag & LINE_TRIM_LEFT) {

		T::size_type pos = newline.find_first_not_of(' ');
		if (T::npos != pos)
		{
			newline.erase(0, pos);
		}
	}

	if (flag & LINE_TRIM_RIGHT) {
		T::size_type pos = newline.find_last_not_of(' ');
		if (T::npos != pos)
		{
			newline.erase(pos + 1);
		}
	}

	if (flag & LINE_EMPTY_DELETE) {
		if (0 == newline.size())
			return;
	}

	v.emplace_back(newline);
}

template<class T, class R, class RI>
void CTextUnits<T, R, RI>::Split(const T& s, std::vector<T>& v, const T& c, int flag)
{
	T::size_type pos1, pos2;
	size_t len = s.length();
	pos2 = s.find(c);
	pos1 = 0;
	while (T::npos != pos2)
	{
		ProcessLine(s.substr(pos1, pos2 - pos1), v, flag);

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);

	}
	if (pos1 != len)
		ProcessLine(s.substr(pos1), v, flag);
}

template<class T, class R, class RI>
std::vector<T> CTextUnits<T,R,RI>::SplitRegEx(const T input, const T c, int flag = 0)
{
	R re(c);
	RI p(input.begin(), input.end(), re, -1);
	RI end;
	std::vector<T> vec;
	while (p != end)
		vec.emplace_back(*p++);

	return vec;
}

template<class T, class R, class RI>
void CTextUnits<T, R, RI>::GetEolChars(const std::string& _str, std::string& c)
{
	EolType eolType = getEOLFormatForm(_str.c_str(), _str.size());
	switch (eolType)
	{
	case EolType::windows: c = "\r\n"; break;
	case EolType::macos:   c = "\r"; break;
	case EolType::unix:    c = "\n"; break;
	default: c = "\r\n"; break;

	}
}

template<class T, class R, class RI>
void CTextUnits<T, R, RI>::GetEolChars(const std::wstring& _str, std::wstring& c)
{
	EolType eolType = getEOLFormatForm(_str.c_str(), _str.size());
	switch (eolType)
	{
	case EolType::windows: c = L"\r\n"; break;
	case EolType::macos:   c = L"\r"; break;
	case EolType::unix:    c = L"\n"; break;
	default: c = L"\r\n"; break;

	}
}

template<class T, class R, class RI>
void CTextUnits<T, R, RI>::SplitLine(const T _src, std::vector<T>&splitedLine, int flag)
{
	T eolChars;
	GetEolChars(_src, eolChars);
	Split(_src, splitedLine, eolChars, flag);
}

template class CTextUnits<std::string, std::regex, std::sregex_token_iterator>;
template class CTextUnits<std::wstring, std::wregex, std::wsregex_token_iterator>;
