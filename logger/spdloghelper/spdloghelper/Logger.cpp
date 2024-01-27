//////////////////////////////////////////////////////////////////////
// Logger.cpp
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////

#include "pch_spdlogh.h"

#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/null_sink.h>

#if ENABLE_WINEDIT_LOGGER
#include <spdlog/sinks/winedit_sinks.h>
#endif

#include <iostream>


namespace spdlogger {

#ifndef CP_ACP
#define CP_ACP 0
#endif

#ifndef CP_UTF8
#define CP_UTF8 65001
#endif

	template<typename Char, typename... Args>
	auto facet_in_out(auto& f, Args&&...args)
	{
		if constexpr (std::is_same<Char, char>::value)
		{
			return f.in(std::forward<Args>(args)...);
		}
		else
		{
			return f.out(std::forward<Args>(args)...);
		}
	}


	template<uint32_t codepage>
	constexpr auto codepage_str()
	{
		switch (codepage)
		{
		case CP_UTF8:
			return ".utf8";
		case CP_ACP:
		default:
			return "";
		}
	}

	template<typename T1, typename T2>
	size_t WtoAtoW(auto& f, const T1* const _src, size_t _src_size, T2* const _dst, size_t _dst_size)
	{
		std::mbstate_t mb{}; // initial shift state
		size_t to_end_size = _dst_size == 0 ? _src_size * f.max_length() + 1 : _dst_size;
		const T1* from = _src;
		const T1* from_end = &_src[_src_size];
		const T1* from_next;
		T2* to = _dst;
		T2* to_end = &_dst[to_end_size];
		T2* to_next;

		while (std::codecvt_base::error == facet_in_out<T1>(f, mb, from, from_end, from_next,
			to, to_end, to_next) && from_end != from_next)
		{
			from = from_next + 1;
			to = to_next + 1;
		}
		*to_next = 0;

		// error checking skipped for brevity
		return to_next - _dst;
	}

#if 0
	size_t WtoA(const wchar_t* const _src, size_t _src_size, char* const _dst, size_t _dst_size)
	{
		auto loc = std::locale("");
		auto& f = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
		return WtoA(f, _src, _src_size, _dst, _dst_size);
	}
#endif

	template<int codepage, typename T1, typename T2, typename STRING_T2 = std::basic_string<T2>>
	STRING_T2 WtoAtoW(const T1* from, size_t from_size)
	{
		auto loc = std::locale(codepage_str<codepage>());
		auto& f = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
		auto size = from_size * f.max_length();

		STRING_T2 to(size, '\0');
		auto ret = WtoAtoW<T1,T2>(f, from, from_size, to.data(), to.size());
		to.resize(ret);
		return std::move(to);
	}

	std::string WtoA(const wchar_t* from, size_t size)
	{
		return std::move(WtoAtoW<CP_ACP, wchar_t, char>(from, size));
	}

	std::wstring AtoW(const char* from, size_t size)
	{
		return std::move(WtoAtoW<CP_ACP, char, wchar_t>(from, size));
	}

	std::string WtoU8(const wchar_t* from, size_t size)
	{
		return std::move(WtoAtoW<CP_UTF8, wchar_t, char>(from, size));
	}

	std::wstring U8toW(const char* from, size_t size)
	{
		return std::move(WtoAtoW<CP_UTF8, char, wchar_t>(from, size));
	}

	std::string AtoU8(const std::string_view& src)
	{
		return std::move(WtoU8(AtoW(src)));
	}

	std::string U8toA(const std::string_view& src)
	{
		return std::move(WtoA(U8toW(src)));
	}

	std::string AtoU8(const char* src, size_t size)
	{
		return std::move(WtoU8(AtoW(src, size)));
	}

	std::string U8toA(const char* src, size_t size)
	{
		return std::move(WtoA(U8toW(src, size)));
	}

}

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

auto CLogger::null_sink(const std::string& logformat)
{
	this->m_logfile = "<NULL>";
	return this->spdlog_sink<spdlog::sinks::null_sink_st>("null");
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
		case LOGGER_TYPE::NONE:
			logger = this->register_sink(logger_name, this->null_sink(logformat));
			break;
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

void CLogger::append_callback_sink(const spdlog::custom_log_callback& callback, const std::string& logformat)
{
	auto spd_sink = std::make_shared<spdlog::sinks::callback_sink_mt>(callback);
	//spd_sink->set_pattern(logformat);
	this->m_defaultlogger->sinks().push_back(spd_sink);
}

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
