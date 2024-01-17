#include "pch_spdlogh.h"

std::string get_date()
{
	//std::chrono::zoned_time zt{ "Asia/Shanghai", std::chrono::system_clock::now() };
	std::chrono::zoned_seconds zt{ "Asia/Shanghai", std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()) };
	return std::format("{:%F}", zt);
}

std::string get_datetime()
{
	//std::chrono::zoned_time zt{ "Asia/Shanghai", std::chrono::system_clock::now() };
	std::chrono::zoned_seconds zt{ "Asia/Shanghai", std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()) };
	return std::format("{:%F %H:%M:%S}", zt);
}

std::string get_datetime2()
{
	std::chrono::zoned_seconds zt{ "Asia/Shanghai", std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()) };
	//std::chrono::zoned_time zt{ "Asia/Shanghai", std::chrono::system_clock::now() };
	return std::format("{:%Y%m%d_%H%M%S}", zt);
}

