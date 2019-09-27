#pragma once
#include <string>
#include <vector>
#include <regex>

#define LINE_TRIM_LEFT		1
#define LINE_TRIM_RIGHT		2
#define LINE_EMPTY_DELETE	4

template <class T, class R, class RI>
class CTextUnits
{
public:
	CTextUnits() {};
	~CTextUnits() {};

	static void SplitLine(const T _src, std::vector<T>& splitedLine, int flag);
	static void Split(const T& s, std::vector<T>& v, const T& c, int flag = 0);
	static std::vector<T> SplitRegEx(const T s, const T c, int flag = 0);

private:

	static void GetEolChars(const std::string& _str, std::string& c);
	static void GetEolChars(const std::wstring& _str, std::wstring& c);
	static void ProcessLine(T& newline, std::vector<T>& v, int flag);

};


template class CTextUnits<std::string, std::regex, std::sregex_token_iterator>;
template class CTextUnits<std::wstring, std::wregex, std::wsregex_token_iterator>;

typedef CTextUnits<std::string, std::regex, std::sregex_token_iterator> CTextUnitsA;
typedef CTextUnits<std::wstring, std::wregex, std::wsregex_token_iterator> CTextUnitsW;
