#pragma once
#include <string>
#include <vector>
#include <regex>

//#define LINE_TRIM_LEFT		1
//#define LINE_TRIM_RIGHT		2
//#define LINE_EMPTY_DELETE	4

//template <class T>
//using Tx = std::basic_string<T, char_traits<T>, allocator<T>>;
//, template<typename VELEM, typename = std::char_traits<VELEM>, typename = std::allocator<VELEM>> class TSTRING = std::basic_string>

/// <summary>
/// 文本按行分割
/// </summary>


enum class SPLIT_FLAG
{
	NONE = 0,
	TRIM_LEFT = 1,
	TRIM_RIGHT = (1 << 1),
	DELETE_NULL_LINE = (1 << 2)
};
_BITMASK_OPS(_EXPORT_STD, SPLIT_FLAG)

template<class T, class Ts = std::basic_string<T>, class Tv = std::basic_string_view<T>>
class CTextUnits
{
public:
	CTextUnits() {};
	~CTextUnits() {};
	
	template<size_t _Size>
	static std::vector<Tv> SplitLine(const T(&_src)[_Size], SPLIT_FLAG flag = (SPLIT_FLAG::TRIM_LEFT | SPLIT_FLAG::TRIM_RIGHT | SPLIT_FLAG::DELETE_NULL_LINE))
	{
		return std::move(SplitLine(_src, _Size - 1, flag));
	}

	static std::vector<Tv> SplitLine(const T* _src, size_t size, SPLIT_FLAG process_flag = (SPLIT_FLAG::TRIM_LEFT | SPLIT_FLAG::TRIM_RIGHT | SPLIT_FLAG::DELETE_NULL_LINE));
	static std::vector<Tv> SplitLine(const Ts& _src, SPLIT_FLAG process_flag = (SPLIT_FLAG::TRIM_LEFT | SPLIT_FLAG::TRIM_RIGHT | SPLIT_FLAG::DELETE_NULL_LINE));

private:
	static std::vector<Tv> SplitLine(const Tv& _src, SPLIT_FLAG process_flag);

public:
	static std::vector<Tv> Split(const Tv& s, const Ts c, SPLIT_FLAG process_flag);
	static std::vector<Tv> Split(const Tv& s, const Ts c);

	template<size_t _Size>
	static Tv Trim(const T(&s)[_Size])
	{
		return Trim(s, _Size - 1);
	}

	static Tv Trim(const T* s, size_t _Size);
	static Tv Trim(const Ts& s);


	template<size_t _Size>
	static Tv TrimLeft(const T(&s)[_Size])
	{
		return TrimLeft(s, _Size - 1);
	}
	static Tv TrimLeft(const T* s, size_t _Size);
	static Tv TrimLeft(const Ts& s);


	template<size_t _Size>
	static Tv TrimRight(const T(&s)[_Size])
	{
		return TrimRight(s, _Size - 1);
	}
	static Tv TrimRight(const T* s, size_t _Size);
	static Tv TrimRight(const Ts& s);

private:
	static void TrimLeftInteral(Tv& s);
	static void TrimRightInteral(Tv& s);
public:

	static std::vector<Ts> SplitRegEx(const Ts& s, const Ts c, int process_flag = 0);
	static Ts Replace(const Ts& str, const Ts& src, const Ts& dest);
	static Ts Replace(const Ts& str, const T src, const T dest);

private:

	static void GetEolChars(const Tv& _str, Ts& c);
	static inline void ProcessLine(Tv newline, std::vector<Tv>& v, SPLIT_FLAG process_flag);

};

typedef CTextUnits<char> CTextUnitsA;
typedef CTextUnits<wchar_t> CTextUnitsW;
