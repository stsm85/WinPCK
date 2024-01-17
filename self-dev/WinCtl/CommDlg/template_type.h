#pragma once
#include <string>

#pragma region char_enable_if_t

template <typename _Test, class _Tchar = void, class _Twchar = void>
struct char_enable_if {}; // no member "type" when !_Test

template <class _Tchar, class _Twchar>
struct char_enable_if<char, _Tchar, _Twchar> { 
    using type = _Tchar;
};

template <class _Tchar, class _Twchar>
struct char_enable_if<wchar_t, _Tchar, _Twchar> {
    using type = _Twchar;
};

template <typename _Test, class _Tchar = void, class _Twchar = void>
using char_enable_if_t = typename char_enable_if<_Test, _Tchar, _Twchar>::type;

#pragma endregion

#pragma region std_string_seletor
template <typename _Test, class _Tchar = std::string, class _Twchar = std::wstring>
struct std_string_seletor {}; // no member "type" when !_Test

template <class _Tchar, class _Twchar>
struct std_string_seletor<char, _Tchar, _Twchar> {
    using type = _Tchar;
};

template <class _Tchar, class _Twchar>
struct std_string_seletor<wchar_t, _Tchar, _Twchar> {
    using type = _Twchar;
};

template <class _Tchar, class _Twchar>
struct std_string_seletor<const char, _Tchar, _Twchar> {
    using type = _Tchar;
};

template <class _Tchar, class _Twchar>
struct std_string_seletor<const wchar_t, _Tchar, _Twchar> {
    using type = _Twchar;
};


template <typename _Test, class _Tchar = std::string, class _Twchar = std::wstring>
using std_string_seletor_t = typename std_string_seletor<_Test, _Tchar, _Twchar>::type;
#pragma endregion