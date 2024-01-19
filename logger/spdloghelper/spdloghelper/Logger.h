#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/xchar.h>

#include <CharsCodeConv.h>

constexpr auto DEFAULT_LOG_FORMAT = "[%Y-%m-%d %H:%M:%S] %L: %v";
constexpr auto DEFAULT_LOGGER_NAME = "main";

class CLogger;
class CEdit;
class CRichEdit;

namespace CppFMT = fmt::v9;

//helper functions
std::string get_date();
std::string get_datetime();
std::string get_datetime2();

class CLogger
{
public:
	enum class LOGGER_TYPE
	{
		CONSOLE_ONLY,
		FILE_ONLY,
		CONSOLE_FILE,
		MEM_ONLY,
		MEM_FILE,
		EDIT_ONLY,
		EDIT_FILE
	};
private:
	CLogger(LOGGER_TYPE type);
	CLogger(const CLogger&) = delete;
	const CLogger& operator=(const CLogger&) = delete;
	virtual ~CLogger();


public:

	static CLogger& GetInstance(LOGGER_TYPE initfunc) {
		static CLogger onlyInstance(initfunc);
		return onlyInstance;
	}

	const auto& logfile() {
		return this->m_logfile;
	}

#ifdef SPDLOG_WCHAR_TO_UTF8_SUPPORT

	template<size_t _Size, typename... Args>
	void log(spdlog::level::level_enum&& lvl, const char(&fmt)[_Size], Args&&...args)
	{
		auto fmtstr = CppFMT::vformat(CppFMT::string_view(fmt, _Size), CppFMT::make_format_args(std::forward<Args>(args)...));
		this->m_defaultlogger->log(lvl, StringCodeConv(fmtstr).to_utf8());
		//this->m_defaultlogger->log(lvl, std::forward<const char[_Size]>(fmt), std::forward<Args>(args)...);
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

	template<size_t _Size, typename... Args>
	void warn(const char(&fmt)[_Size], Args&&...args)
	{
		this->log(spdlog::level::warn, std::forward<const char[_Size]>(fmt), std::forward<Args>(args)...);
	}
	template<size_t _Size, typename... Args>
	void info(const char(&fmt)[_Size], Args&&...args)
	{
		this->log(spdlog::level::info, std::forward<const char[_Size]>(fmt), std::forward<Args>(args)...);
	}

	template<size_t _Size, typename... Args>
	void debug(const char(&fmt)[_Size], Args&&...args)
	{
		this->log(spdlog::level::debug, std::forward<const char[_Size]>(fmt), std::forward<Args>(args)...);
	}

	template<size_t _Size, typename... Args>
	void trace(const char(&fmt)[_Size], Args&&...args)
	{
		this->log(spdlog::level::trace, std::forward<const char[_Size]>(fmt), std::forward<Args>(args)...);
	}

	template<typename... Args>
	void log(spdlog::level::level_enum&& lvl, const std::string& _text, Args&&...args)
	{
		auto fmtstr = CppFMT::vformat(_text, CppFMT::make_format_args(std::forward<Args>(args)...));
		this->m_defaultlogger->log(lvl, StringCodeConv(fmtstr).to_utf8());
		//this->m_defaultlogger->log(lvl, _text, std::forward<Args>(args)...);
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

	template<typename... Args>
	void warn(const std::string& _text, Args&&...args)
	{
		this->log(spdlog::level::warn, _text, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void info(const std::string& _text, Args&&...args)
	{
		this->log(spdlog::level::info, _text, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void debug(const std::string& _text, Args&&...args)
	{
		this->log(spdlog::level::debug, _text, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void trace(const std::string& _text, Args&&...args)
	{
		this->log(spdlog::level::trace, _text, std::forward<Args>(args)...);
	}

	template<size_t _Size, typename... Args>
	void log(spdlog::level::level_enum&& lvl, const wchar_t(&fmt)[_Size], Args&&...args)
	{
		auto fmtstr = CppFMT::vformat(CppFMT::wstring_view(fmt, _Size), CppFMT::make_wformat_args(std::forward<Args>(args)...));
		this->m_defaultlogger->log(lvl, StringCodeConv(fmtstr).to_utf8());
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

	template<size_t _Size, typename... Args>
	void warn(const wchar_t(&fmt)[_Size], Args&&...args)
	{
		this->log(spdlog::level::warn, std::forward<const wchar_t[_Size]>(fmt), std::forward<Args>(args)...);
	}
	template<size_t _Size, typename... Args>
	void info(const wchar_t(&fmt)[_Size], Args&&...args)
	{
		this->log(spdlog::level::info, std::forward<const wchar_t[_Size]>(fmt), std::forward<Args>(args)...);
	}

	template<size_t _Size, typename... Args>
	void debug(const wchar_t(&fmt)[_Size], Args&&...args)
	{
		this->log(spdlog::level::debug, std::forward<const wchar_t[_Size]>(fmt), std::forward<Args>(args)...);
	}

	template<size_t _Size, typename... Args>
	void trace(const wchar_t(&fmt)[_Size], Args&&...args)
	{
		this->log(spdlog::level::trace, std::forward<const wchar_t[_Size]>(fmt), std::forward<Args>(args)...);
	}

	template<typename... Args>
	void log(spdlog::level::level_enum&& lvl, const std::wstring& _text, Args&&...args)
	{
		auto fmtstr = CppFMT::vformat(_text, CppFMT::make_wformat_args(std::forward<Args>(args)...));
		this->m_defaultlogger->log(lvl, StringCodeConv(fmtstr).to_utf8());
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

	template<typename... Args>
	void warn(const std::wstring& _text, Args&&...args)
	{
		this->log(spdlog::level::warn, _text, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void info(const std::wstring& _text, Args&&...args)
	{
		this->log(spdlog::level::info, _text, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void debug(const std::wstring& _text, Args&&...args)
	{
		this->log(spdlog::level::debug, _text, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void trace(const std::wstring& _text, Args&&...args)
	{
		this->log(spdlog::level::trace, _text, std::forward<Args>(args)...);
	}
#else
	template<size_t _Size, typename... Args>
	void log(spdlog::level::level_enum&& lvl, const char(&fmt)[_Size], Args&&...args)
	{
		this->m_defaultlogger->log(lvl, std::forward<const char[_Size]>(fmt), std::forward<Args>(args)...);
	}

	template<size_t _Size, typename... Args>
	void critical(const char(&fmt)[_Size], Args&&...args)
	{
		this->m_defaultlogger->critical(std::forward<const char[_Size]>(fmt), std::forward<Args>(args)...);
	}

	template<size_t _Size, typename... Args>
	void error(const char(&fmt)[_Size], Args&&...args)
	{
		this->m_defaultlogger->error(std::forward<const char[_Size]>(fmt), std::forward<Args>(args)...);
	}

	template<size_t _Size, typename... Args>
	void warn(const char(&fmt)[_Size], Args&&...args)
	{
		this->m_defaultlogger->warn(std::forward<const char[_Size]>(fmt), std::forward<Args>(args)...);
	}
	template<size_t _Size, typename... Args>
	void info(const char(&fmt)[_Size], Args&&...args)
	{
		this->m_defaultlogger->info(std::forward<const char[_Size]>(fmt), std::forward<Args>(args)...);
	}

	template<size_t _Size, typename... Args>
	void debug(const char(&fmt)[_Size], Args&&...args)
	{
		this->m_defaultlogger->debug(std::forward<const char[_Size]>(fmt), std::forward<Args>(args)...);
	}

	template<size_t _Size, typename... Args>
	void trace(const char(&fmt)[_Size], Args&&...args)
	{
		this->m_defaultlogger->trace(std::forward<const char[_Size]>(fmt), std::forward<Args>(args)...);
	}

	template<typename... Args>
	void log(spdlog::level::level_enum&& lvl, const std::string& _text, Args&&...args)
	{
		this->m_defaultlogger->log(lvl, _text, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void critical(const std::string& _text, Args&&...args)
	{
		this->m_defaultlogger->critical(_text, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void error(const std::string& _text, Args&&...args)
	{
		this->m_defaultlogger->error(_text, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void warn(const std::string& _text, Args&&...args)
	{
		this->m_defaultlogger->warn(_text, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void info(const std::string& _text, Args&&...args)
	{
		this->m_defaultlogger->info(_text, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void debug(const std::string& _text, Args&&...args)
	{
		this->m_defaultlogger->debug(_text, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void trace(const std::string& _text, Args&&...args)
	{
		this->m_defaultlogger->trace(_text, std::forward<Args>(args)...);
	}

	template<size_t _Size, typename... Args>
	void log(spdlog::level::level_enum&& lvl, const wchar_t(&fmt)[_Size], Args&&...args)
	{
		auto fmtstr = CppFMT::vformat(CppFMT::wstring_view(fmt, _Size), CppFMT::make_wformat_args(std::forward<Args>(args)...));
		this->m_defaultlogger->log(lvl, StringCodeConv(fmtstr).to_ansi());
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

	template<size_t _Size, typename... Args>
	void warn(const wchar_t(&fmt)[_Size], Args&&...args)
	{
		this->log(spdlog::level::warn, std::forward<const wchar_t[_Size]>(fmt), std::forward<Args>(args)...);
	}
	template<size_t _Size, typename... Args>
	void info(const wchar_t(&fmt)[_Size], Args&&...args)
	{
		this->log(spdlog::level::info, std::forward<const wchar_t[_Size]>(fmt), std::forward<Args>(args)...);
	}

	template<size_t _Size, typename... Args>
	void debug(const wchar_t(&fmt)[_Size], Args&&...args)
	{
		this->log(spdlog::level::debug, std::forward<const wchar_t[_Size]>(fmt), std::forward<Args>(args)...);
	}

	template<size_t _Size, typename... Args>
	void trace(const wchar_t(&fmt)[_Size], Args&&...args)
	{
		this->log(spdlog::level::trace, std::forward<const wchar_t[_Size]>(fmt), std::forward<Args>(args)...);
	}

	template<typename... Args>
	void log(spdlog::level::level_enum&& lvl, const std::wstring& _text, Args&&...args)
	{
		//need edit
		auto fmtstr = CppFMT::vformat(_text, CppFMT::make_wformat_args(std::forward<Args>(args)...));
		this->m_defaultlogger->log(lvl, StringCodeConv(fmtstr).to_ansi());
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

	template<typename... Args>
	void warn(const std::wstring& _text, Args&&...args)
	{
		this->log(spdlog::level::warn, _text, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void info(const std::wstring& _text, Args&&...args)
	{
		this->log(spdlog::level::info, _text, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void debug(const std::wstring& _text, Args&&...args)
	{
		this->log(spdlog::level::debug, _text, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void trace(const std::wstring& _text, Args&&...args)
	{
		this->log(spdlog::level::trace, _text, std::forward<Args>(args)...);
	}
#endif

private:
	auto log_file_name(const std::string& logger_name);

public:
	void append_winedit_sink(HWND richedit, int max_line, const std::string& logformat = DEFAULT_LOG_FORMAT);
	void append_winedit_color_sink(HWND richedit, int max_line, const std::string& logformat = DEFAULT_LOG_FORMAT);

private:
	template<class T>
	auto register_sink(const std::string& logger_name, T spdlog_sink);

	template<class T, typename... Args>
	auto spdlog_sink(const std::string& logformat, Args&&...args);

	auto console_sink(const std::string& logformat);
	auto file_sink(const std::string& logformat, const std::string& logger_name);
	auto memory_sink(const std::string& logformat);

public:
	void setup_default_logger(LOGGER_TYPE type, const std::string& logformat = DEFAULT_LOG_FORMAT, const std::string& logger_name = DEFAULT_LOGGER_NAME);
	void show_log_msg(const std::string& appname);

	std::string read_mem_log();
	std::string read_mem_log(int size);
	int read_mem_log(char* buffer, int size);

	void set_default_logger(auto logger)
	{
		this->m_defaultlogger = logger;
	};

	auto get_logger(const std::string& name) const {
		try {
			return spdlog::get(name);
		}
		catch (std::exception) {
			return spdlog::default_logger();
		}
	};

	auto operator[](const std::string& name) const {
		try {
			return spdlog::get(name);
		}
		catch (std::exception) {
			return spdlog::default_logger();
		}
	}

	auto operator->() const noexcept {
		return this->m_defaultlogger.get();
	}

#if ENABLE_MULTILOGGER
	class _logger {
	private:
		std::shared_ptr<spdlog::logger> m_consolelogger = spdlog::default_logger();
		std::shared_ptr<spdlog::logger> m_filelogger = spdlog::default_logger();
		std::shared_ptr<spdlog::logger> m_combinedlogger = spdlog::default_logger();
		std::shared_ptr<spdlog::logger> m_memlogger = spdlog::default_logger();
		//std::shared_ptr<spdlog::logger> m_userlogger1 = spdlog::default_logger();
		//std::shared_ptr<spdlog::logger> m_memfilelogger = spdlog::default_logger();

		//std::stringstream m_memfile_stream;
		//std::iostream m_log_stream;

	public:
		auto console() {
			return this->m_consolelogger.get();
		};

		auto file() {
			return this->m_filelogger.get();
		};

		auto combined() {
			return this->m_combinedlogger.get();
		};

		auto mem() {
			return this->m_memlogger.get();
		};

		//auto userlog1() {
		//	return this->m_userlogger1.get();
		//};

		friend class CLogger;
	} logger;
#endif
private:

	std::stringstream m_mem_stream;

	std::string m_logfile;
	std::shared_ptr<spdlog::logger> m_defaultlogger = spdlog::default_logger();

};

//extern CLogger& Logger;
