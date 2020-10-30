#pragma once
#include <string>
#include <vector>
#include <regex>

#define LINE_TRIM_LEFT		1
#define LINE_TRIM_RIGHT		2
#define LINE_EMPTY_DELETE	4

//template <class T>
//using Tx = std::basic_string<T, char_traits<T>, allocator<T>>;
//, template<typename VELEM, typename = std::char_traits<VELEM>, typename = std::allocator<VELEM>> class TSTRING = std::basic_string>

template <class T>
class CTextUnits
{
public:
	CTextUnits() {};
	~CTextUnits() {};

	static void SplitLine(const std::basic_string<T> _src, std::vector<std::basic_string<T>>& splitedLine, int process_flag = (LINE_TRIM_LEFT | LINE_TRIM_RIGHT | LINE_EMPTY_DELETE));

	static void Split(const std::basic_string<T>& s, std::vector<std::basic_string<T>>& v, const std::basic_string<T> c, int process_flag);
	static void Split(const std::basic_string<T>& s, std::vector<std::basic_string<T>>& v, const std::basic_string<T> c);

	static std::basic_string<T> Trim(std::basic_string<T>& s);
	static std::basic_string<T> TrimLeft(std::basic_string<T>& s);
	static std::basic_string<T> TrimRight(std::basic_string<T>& s);

	static std::vector<std::basic_string<T>> SplitRegEx(const std::basic_string<T> s, const std::basic_string<T> c, int process_flag = 0);
	static std::basic_string<T> Replace(const std::basic_string<T>& str, const std::basic_string<T>& src, const std::basic_string<T>& dest);

private:

	static void GetEolChars(const std::basic_string<T>& _str, std::basic_string<T>& c);
	static void ProcessLine(std::basic_string<T> newline, std::vector<std::basic_string<T>>& v, int process_flag);

};

typedef CTextUnits<char> CTextUnitsA;
typedef CTextUnits<wchar_t> CTextUnitsW;
