#include "pch_spdlogh.h"

#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/ostream_sink.h>
#if ENABLE_WINEDIT_LOGGER
#include <spdlog/sinks/winedit_sinks.h>
#endif

#include <iostream>

CLogger::CLogger(LOGGER_TYPE type) {

	try {
#ifdef WIN32
		std::string stem;
		try {
			wchar_t exepath[MAX_PATH];
			::GetModuleFileNameW(NULL, exepath, MAX_PATH);
			fs::path exefile(exepath);

			stem = exefile.stem().string();

			//// \/:*?"<>|
			//std::transform(stem.begin(), stem.end(), stem.begin(), [](char ch) -> char {

			//	if ('?' == ch) [[unlikely]] {
			//		return '_';
			//		}
			//	else {
			//		return ch;
			//	}
			//	});
		}
		catch (...)
		{
#ifdef APPNAME
			stem.assign(APPNAME);
#else
			stem.assign(DEFAULT_LOGGER_NAME);
#endif
		}

		this->setup_default_logger(type, DEFAULT_LOG_FORMAT, stem);
#else
#ifdef APPNAME
		this->setup_default_logger(type, DEFAULT_LOG_FORMAT, APPNAME);
#else
		this->setup_default_logger(type, DEFAULT_LOG_FORMAT, DEFAULT_LOGGER_NAME);
#endif
#endif

		
	}
	catch (std::bad_function_call ex)
	{
		std::cout << "Log user function call failed: " << ex.what() << std::endl;
	}
}


CLogger::~CLogger() {

	spdlog::shutdown();
}

auto CLogger::log_file_name(const std::string& logger_name)
{
	//pLogger->set_default_logger(pLogger->logger.mem());
	//change_workdir();

	//auto log_file = fs::path(getenv("APPDATA")) / "LiqfSoft";
	//fs::create_directory(log_file);
	//log_file /= APPNAME ".log";

	auto log_file = fs::current_path() / "logs";
	fs::create_directory(log_file);
	log_file /= (logger_name + ".log");
	return log_file.string();
}

template<class T>
auto CLogger::register_sink(const std::string& logger_name, T spdlog_sink)
{
	auto spd_logger = std::make_shared<spdlog::logger>(logger_name, spdlog_sink);
	spd_logger->flush_on(spdlog::level::warn);
	spdlog::register_logger(spd_logger);
	return spd_logger;
}

template<>
auto CLogger::register_sink(const std::string& logger_name, std::vector<spdlog::sink_ptr> sinks)
{
	auto spd_logger = std::make_shared<spdlog::logger>(logger_name, sinks.begin(), sinks.end());
	spd_logger->flush_on(spdlog::level::warn);
	spdlog::register_logger(spd_logger);
	
	return spd_logger;
}

template<class T, typename... Args>
auto CLogger::spdlog_sink(const std::string& logformat, Args&&...args)
{
	auto spd_sink = std::make_shared<T>(std::forward<Args>(args)...);
	spd_sink->set_pattern(logformat);
	return spd_sink;
}

auto CLogger::console_sink(const std::string& logformat)
{
	this->m_logfile = "<CONSOLE>";
	return this->spdlog_sink<spdlog::sinks::stdout_color_sink_mt>(logformat);
}

auto CLogger::file_sink(const std::string& logformat, const std::string& logger_name)
{
	constexpr bool truncate = false;
	auto logfile = log_file_name(logger_name);
	this->m_logfile = logfile;

	return this->spdlog_sink<spdlog::sinks::daily_file_sink_mt>(logformat, logfile, 0, 0, truncate);
}

auto CLogger::memory_sink(const std::string& logformat)
{
	return this->spdlog_sink<spdlog::sinks::ostream_sink_mt>(logformat, this->m_mem_stream);
}


void CLogger::setup_default_logger(LOGGER_TYPE type, const std::string& logformat, const std::string& logger_name)
{
	try {
		std::shared_ptr<spdlog::logger> logger;

		switch (type)
		{
		case LOGGER_TYPE::CONSOLE_ONLY:

			logger = this->register_sink(logger_name, this->console_sink(logformat));
			break;

		case LOGGER_TYPE::CONSOLE_FILE:
			{
				std::vector<spdlog::sink_ptr> sinks{
					this->console_sink(logformat),
					this->file_sink(logformat, logger_name)
				};
				logger = this->register_sink(logger_name, sinks);
			}
			break;
		
		case LOGGER_TYPE::MEM_ONLY:

			logger = this->register_sink(logger_name, this->memory_sink(logformat));
			break;

		case LOGGER_TYPE::MEM_FILE:
			{
				std::vector<spdlog::sink_ptr> sinks{
				this->memory_sink(logformat),
				this->file_sink(logformat, logger_name)
				};
				logger = this->register_sink(logger_name, sinks);
			}
			break;

		case LOGGER_TYPE::FILE_ONLY:

			logger = this->register_sink(logger_name, this->file_sink(logformat, logger_name));
			break;
#if 0
		case LOGGER_TYPE::EDIT_ONLY:

			logger = this->register_sink(logger_name, this->winedit_sink(logformat, logger_name));
			break;

		case LOGGER_TYPE::EDIT_FILE:

			logger = this->register_sink(logger_name, sinks);
			break;
#endif
		default:
			return /*spdlog::default_logger()*/;
		}

		this->m_defaultlogger = logger;
		spdlog::set_default_logger(logger);
		logger->set_level(spdlog::level::info);

		return /*logger*/;
	}
	catch (const spdlog::spdlog_ex& ex)
	{
		std::cout << "Log initialization failed: " << ex.what() << std::endl;
		return /*spdlog::default_logger()*/;
	}
}

#if ENABLE_WINEDIT_LOGGER

void CLogger::append_winedit_sink(HWND richedit, int max_line, const std::string& logformat)
{
	auto spd_sink = std::make_shared<spdlog::sinks::winedit_sink_mt>(richedit, max_line);
	spd_sink->set_pattern(logformat);
	this->m_defaultlogger->sinks().push_back(spd_sink);
}

void CLogger::append_winedit_color_sink(HWND richedit, int max_line, const std::string& logformat)
{
	auto spd_sink = std::make_shared<spdlog::sinks::winedit_color_sink_mt>(richedit, max_line);
	spd_sink->set_pattern(logformat);
	this->m_defaultlogger->sinks().push_back(spd_sink);
}
#endif

void CLogger::show_log_msg(const std::string& appname)
{
	this->m_defaultlogger->info("{} is started.", appname);
	this->m_defaultlogger->info("logger file path is: {}", this->m_logfile);
}

std::string CLogger::read_mem_log()
{
	auto pos = this->m_mem_stream.tellg();
	this->m_mem_stream.seekg(0, std::ios::end);
	auto nLengthSize = this->m_mem_stream.tellg() - pos;

	if (0 == nLengthSize)
		return "";

	std::stringstream str;
	std::swap(this->m_mem_stream, str);
	return str.str();
}

std::string CLogger::read_mem_log(int size)
{
	std::string str;
	str.resize(size);
	auto bytesread = this->read_mem_log(str.data(), size);
	if (0 == bytesread)
		return "";
	str.resize(bytesread);
	return str;
}

int CLogger::read_mem_log(char* buffer, int size)
{

	auto pos = this->m_mem_stream.tellg();

	this->m_mem_stream.seekg(0, std::ios::end);
	auto nLengthSize = this->m_mem_stream.tellg() - pos;
	this->m_mem_stream.seekg(pos, std::ios::beg);

	if (0 == nLengthSize)
		return 0;

	if (size >= nLengthSize)
	{
		this->m_mem_stream.read(buffer, nLengthSize);
		this->m_mem_stream.str("");

		return nLengthSize;
	}
	else {
		this->m_mem_stream.read(buffer, size);
		return size;
	}
}
