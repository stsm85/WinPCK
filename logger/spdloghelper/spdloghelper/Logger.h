#pragma once
#include <spdlog/spdlog.h>

#define ENABLE_WINEDIT_LOGGER 1
#define ENABLE_MULTILOGGER 0

constexpr auto DEFAULT_LOG_FORMAT = "[%Y-%m-%d %H:%M:%S] %L: %v";
constexpr auto DEFAULT_LOGGER_NAME = "main";

class CLogger;
class CEdit;
class CRichEdit;

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

	auto operator->() const noexcept{
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

extern CLogger& Logger;
