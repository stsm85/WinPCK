#pragma once

#if 0
#include <spdlog/spdlog.h>
#ifdef SPDLOG_WCHAR_TO_UTF8_SUPPORT
#include <spdlog/fmt/xchar.h>
#endif
#include <CharsCodeConv.h>
#endif

#if ENABLE_DETAILLOG
#include <source_location>

template<>
struct CppFMT::formatter<std::source_location, char> {
	auto parse(CppFMT::format_parse_context& parseContext) {
		auto symbolsEnd = std::ranges::find(parseContext, '}');
		//auto symbols = std::string_view(parseContext.begin(), symbolsEnd);
		//std::cout << "parse(" << symbols << ")" << std::endl;
		return symbolsEnd;
	}

	auto format(const std::source_location& location, CppFMT::format_context& formatContext) {
		return CppFMT::format_to(formatContext.out(), "({}({},{}):{})", location.file_name(), location.line(), location.column(), location.function_name());
	}
};

template<>
struct CppFMT::formatter<std::source_location, wchar_t> {
	auto parse(CppFMT::wformat_parse_context& parseContext) {
		auto symbolsEnd = std::ranges::find(parseContext, '}');
		//auto symbols = std::string_view(parseContext.begin(), symbolsEnd);
		//std::cout << "parse(" << symbols << ")" << std::endl;
		return symbolsEnd;
	}

	auto format(const std::source_location& location, CppFMT::wformat_context& formatContext) {
		const auto fmt_str = CppFMT::format("({}({},{}):{})", location.file_name(), location.line(), location.column(), location.function_name());
		return CppFMT::format_to(formatContext.out(), L"{}", spdlogger::AtoW(fmt_str));
	}
};

class DetailLog
{
public:
	DetailLog(const std::source_location& _location = std::source_location::current()) :
		location(_location) {};
	~DetailLog() {};

	template<typename... Args>
	void log(spdlog::level::level_enum&& lvl, CppFMT::string_view fmt, Args&&...args) const
	{
		auto str1 = CppFMT::format("{} {}", fmt, this->location);
		auto str2 = CppFMT::vformat(str1, CppFMT::make_format_args(std::forward<Args>(args)...));
		Logger->log(lvl, str2);
	}

	template<size_t _Size, typename... Args>
	void critical(const char(&fmt)[_Size], Args&&...args)
	{
		this->log(spdlog::level::critical, std::forward<const char[_Size]>(fmt), std::forward<Args>(args)...);
	}

	template<size_t _Size, typename... Args>
	void error(const char(&fmt)[_Size], Args&&...args)
	{
		this->log(spdlog::level::err, std::forward<const char[_Size]>(fmt), std::forward<Args>(args)...);
	}

	template<typename... Args>
	void log(spdlog::level::level_enum&& lvl, const std::string& _text, Args&&...args)
	{
		auto str1 = CppFMT::format("{} {}", _text, this->location);
		auto str2 = CppFMT::vformat(str1, CppFMT::make_format_args(std::forward<Args>(args)...));
		Logger->log(lvl, str2);
	}
	template<typename... Args>
	void critical(const std::string& _text, Args&&...args)
	{
		this->log(spdlog::level::critical, _text, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void error(const std::string& _text, Args&&...args)
	{
		this->log(spdlog::level::err, _text, std::forward<Args>(args)...);
	}
	template<size_t _Size, typename... Args>
	void log(spdlog::level::level_enum&& lvl, const wchar_t(&fmt)[_Size], Args&&...args)
	{
		auto str1 = CppFMT::format(L"{} {}", std::forward<const wchar_t[_Size]>(fmt), this->location);
		auto str2 = CppFMT::vformat(CppFMT::wstring_view(str1), CppFMT::make_wformat_args(std::forward<Args>(args)...));
		Logger->log(lvl, str2);
	}

	template<size_t _Size, typename... Args>
	void critical(const wchar_t(&fmt)[_Size], Args&&...args)
	{
		this->log(spdlog::level::critical, std::forward<const wchar_t[_Size]>(fmt), std::forward<Args>(args)...);
	}

	template<size_t _Size, typename... Args>
	void error(const wchar_t(&fmt)[_Size], Args&&...args)
	{
		this->log(spdlog::level::err, std::forward<const wchar_t[_Size]>(fmt), std::forward<Args>(args)...);
	}

	template<typename... Args>
	void log(spdlog::level::level_enum&& lvl, const std::wstring& _text, Args&&...args)
	{
		auto str1 = CppFMT::format(L"{} {}", _text, this->location);
		auto str2 = CppFMT::vformat(CppFMT::wstring_view(str1), CppFMT::make_wformat_args(std::forward<Args>(args)...));
		Logger->log(lvl, str2);
	}

	template<typename... Args>
	void critical(const std::wstring& _text, Args&&...args)
	{
		this->log(spdlog::level::critical, _text, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void error(const std::wstring& _text, Args&&...args)
	{
		this->log(spdlog::level::err, _text, std::forward<Args>(args)...);
	}

private:
	const std::source_location& location;
};

#endif