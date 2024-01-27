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


#define CHARTYPE2_IS_SAME	std::is_same<T, wchar_t>::value || \
							std::is_same<T, char>::value

#define CHARTYPE3_IS_SAME	std::is_same<T, wchar_t>::value || \
							std::is_same<T, char8_t>::value || \
							std::is_same<T, char>::value

#define CHARTYPE2_ENABLE_IF  std::enable_if_t<CHARTYPE2_IS_SAME, int> = 0
#define CHARTYPE3_ENABLE_IF  std::enable_if_t<CHARTYPE3_IS_SAME, int> = 0


