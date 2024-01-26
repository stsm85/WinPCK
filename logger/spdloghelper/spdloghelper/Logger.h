#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/callback_sink.h>

#include <spdlog/fmt/xchar.h>
#include <locale>

#define ENABLE_WINEDIT_LOGGER 1

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

namespace spdlogger {

#define STRING_ENABLE_IF  std::enable_if_t<std::is_same<T, std::string_view>::value || \
	std::is_same<T, std::string>::value || \
	std::is_same<T, CppFMT::string_view>::value, \
	int> = 0

#define WSTRING_ENABLE_IF  std::enable_if_t<std::is_same<T, std::wstring_view>::value || \
	std::is_same<T, std::wstring>::value || \
	std::is_same<T, CppFMT::wstring_view>::value, \
	int> = 0

	std::string WtoA(const wchar_t* from, size_t size);
	std::wstring AtoW(const char* from, size_t size);
	std::string WtoU8(const wchar_t* from, size_t size);
	std::wstring U8toW(const char* from, size_t size);

	template<typename T, WSTRING_ENABLE_IF>
	std::string WtoA(const T& from)
	{
		return std::move(WtoA(from.data(), from.size()));
	}

	template<typename T, STRING_ENABLE_IF>
	std::wstring AtoW(const T& from)
	{
		return std::move(AtoW(from.data(), from.size()));
	}

	template<typename T, WSTRING_ENABLE_IF>
	std::string WtoU8(const T& from)
	{
		return std::move(WtoU8(from.data(), from.size()));
	}

	template<typename T, STRING_ENABLE_IF>
	std::wstring U8toW(const T& from)
	{
		return std::move(U8toW(from.data(), from.size()));
	}

	template <size_t _Size>
	std::string WtoA(const wchar_t(&str)[_Size])
	{
		return WtoA(str, _Size - 1);
	}

	template <size_t _Size>
	std::wstring AtoW(const char(&str)[_Size])
	{
		return AtoW(str, _Size - 1);
	}
	template <size_t _Size>
	std::string WtoU8(const wchar_t(&str)[_Size])
	{
		return WtoU8(str, _Size - 1);
	}

	template <size_t _Size>
	std::wstring U8toW(const char(&str)[_Size])
	{
		return U8toW(str, _Size - 1);
	}

	template<typename T, typename Tr = std::remove_pointer_t<T>, std::enable_if_t<(std::is_same<T, wchar_t*>::value || std::is_same<T, const wchar_t*>::value) && std::is_pointer<T>::value, int> = 0>
	std::string WtoA(T from)
	{
		const auto size = std::char_traits<Tr>::length(from);
		return std::move(WtoA(from, size));
	}

	template<typename T, typename Tr = std::remove_pointer_t<T>, std::enable_if_t<(std::is_same<T, char*>::value || std::is_same<T, const char*>::value) && std::is_pointer<T>::value, int> = 0>
	std::wstring AtoW(T from)
	{
		const auto size = std::char_traits<Tr>::length(from);
		return std::move(AtoW(from, size));
	}

	template<typename T, typename Tr = std::remove_pointer_t<T>, std::enable_if_t<(std::is_same<T, wchar_t*>::value || std::is_same<T, const wchar_t*>::value) && std::is_pointer<T>::value, int> = 0>
	std::string WtoU8(T from)
	{
		const auto size = std::char_traits<Tr>::length(from);
		return std::move(WtoU8(from, size));
	}

	template<typename T, typename Tr = std::remove_pointer_t<T>, std::enable_if_t<(std::is_same<T, char*>::value || std::is_same<T, const char*>::value) && std::is_pointer<T>::value, int> = 0>
	std::wstring U8toW(T from)
	{
		const auto size = std::char_traits<Tr>::length(from);
		return std::move(U8toW(from, size));
	}

	std::string AtoU8(const std::string_view& src);
	std::string U8toA(const std::string_view& src);

	std::string AtoU8(const char* src, size_t size);
	std::string U8toA(const char* src, size_t size);

#undef STRING_ENABLE_IF
#undef WSTRING_ENABLE_IF
}

class CLogger
{
public:
	enum class LOGGER_TYPE
	{
		NONE,
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

public:

#ifdef SPDLOG_WCHAR_TO_UTF8_SUPPORT

	template<size_t _Size, typename... Args>
	void log(spdlog::level::level_enum&& lvl, const char(&fmt)[_Size], Args&&...args)
	{
		auto fmtstr = CppFMT::vformat(CppFMT::string_view(fmt, _Size - 1), CppFMT::make_format_args(std::forward<Args>(args)...));
		this->m_defaultlogger->log(lvl, spdlogger::AtoU8(fmtstr.data(), fmtstr.size()));
	}

	template<typename... Args>
	void log(spdlog::level::level_enum&& lvl, const CppFMT::string_view& _text, Args&&...args)
	{
		auto fmtstr = CppFMT::vformat(_text, CppFMT::make_format_args(std::forward<Args>(args)...));
		this->m_defaultlogger->log(lvl, spdlogger::AtoU8(fmtstr.data(), fmtstr.size()));
	}

	template<size_t _Size, typename... Args>
	void log(spdlog::level::level_enum&& lvl, const wchar_t(&fmt)[_Size], Args&&...args)
	{
		auto fmtstr = CppFMT::vformat(CppFMT::wstring_view(fmt, _Size - 1), CppFMT::make_wformat_args(std::forward<Args>(args)...));
		//auto fmtstr = CppFMT::vformat(std::forward<const wchar_t[_Size]>(fmt), CppFMT::make_wformat_args(std::forward<Args>(args)...));
		this->m_defaultlogger->log(lvl, spdlogger::WtoU8(fmtstr.data(), fmtstr.size()));
	}

	template<typename... Args>
	void log(spdlog::level::level_enum&& lvl, const CppFMT::wstring_view& _text, Args&&...args)
	{
		auto fmtstr = CppFMT::vformat(_text, CppFMT::make_wformat_args(std::forward<Args>(args)...));
		this->m_defaultlogger->log(lvl, spdlogger::WtoU8(fmtstr.data(), fmtstr.size()));
	}

#else
	template<size_t _Size, typename... Args>
	void log(spdlog::level::level_enum&& lvl, const char(&fmt)[_Size], Args&&...args)
	{
		this->m_defaultlogger->log(lvl, std::forward<const char[_Size]>(fmt), std::forward<Args>(args)...);
	}

	template<typename... Args>
	void log(spdlog::level::level_enum&& lvl, const CppFMT::string_view& _text, Args&&...args)
	{
		this->m_defaultlogger->log(lvl, _text, std::forward<Args>(args)...);
	}

	template<size_t _Size, typename... Args>
	void log(spdlog::level::level_enum&& lvl, const wchar_t(&fmt)[_Size], Args&&...args)
	{
		auto fmtstr = CppFMT::vformat(CppFMT::wstring_view(fmt, _Size - 1), CppFMT::make_wformat_args(std::forward<Args>(args)...));
		//auto fmtstr = CppFMT::vformat(std::forward<const wchar_t[_Size]>(fmt), CppFMT::make_wformat_args(std::forward<Args>(args)...));
		this->m_defaultlogger->log(lvl, spdlogger::WtoA(fmtstr.data(), fmtstr.size()));
	}

	template<typename... Args>
	void log(spdlog::level::level_enum&& lvl, const CppFMT::wstring_view& _text, Args&&...args)
	{
		//need edit
		auto fmtstr = CppFMT::vformat(_text, CppFMT::make_wformat_args(std::forward<Args>(args)...));
		this->m_defaultlogger->log(lvl, spdlogger::WtoA(fmtstr.data(), fmtstr.size()));
	}

#endif

	template<typename... Args>
	void off(Args&&...args)
	{
		this->log(spdlog::level::off, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void critical(Args&&...args)
	{
		this->log(spdlog::level::critical, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void error(Args&&...args)
	{
		this->log(spdlog::level::err, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void warn(Args&&...args)
	{
		this->log(spdlog::level::warn, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void info(Args&&...args)
	{
		this->log(spdlog::level::info, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void debug(Args&&...args)
	{
		this->log(spdlog::level::debug, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void trace(Args&&...args)
	{
		this->log(spdlog::level::trace, std::forward<Args>(args)...);
	}


private:
	auto log_file_name(const std::string& logger_name);

public:
	void append_winedit_sink(HWND richedit, int max_line, const std::string& logformat = DEFAULT_LOG_FORMAT);
	void append_winedit_color_sink(HWND richedit, int max_line, const std::string& logformat = DEFAULT_LOG_FORMAT);
	void append_callback_sink(const spdlog::custom_log_callback& callback, const std::string& logformat = DEFAULT_LOG_FORMAT);

private:
	template<class T>
	auto register_sink(const std::string& logger_name, T spdlog_sink);

	template<class T, typename... Args>
	auto spdlog_sink(const std::string& logformat, Args&&...args);

	auto null_sink(const std::string& logformat);
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

