#pragma once
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

template <typename _Test, class _Tchar, class _Twchar>
using char_enable_if_t = typename char_enable_if<_Test, _Tchar, _Twchar>::type;




