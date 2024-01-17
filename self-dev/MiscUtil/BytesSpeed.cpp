#include "BytesSpeed.h"
#include <chrono>
#include <format>
#include "BytesConv.h"

#define SIZE_PER_SECTOR		(512)
#define SIZE_PER_64K		(64 * 1024)

CBytesSpeed::CBytesSpeed()
{
}

CBytesSpeed::~CBytesSpeed()
{
}

void CBytesSpeed::add(uint64_t pos)
{
	if(this->MAX_SLOT <= this->queue_list.size())
	{
		this->queue_list.pop_front();
	}
	INFO info{
		.timestemp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(),
		.current_pos = pos,
	};
	this->queue_list.push_back(info);
}

uint64_t CBytesSpeed::speed()
{
	if (2 > this->queue_list.size())
		return 0;

	auto time_du = this->queue_list.back().timestemp - this->queue_list.front().timestemp;
	auto pos_du = this->queue_list.back().current_pos - this->queue_list.front().current_pos;
	return pos_du * 1000 / time_du;
}


std::string CBytesSpeed::speed_str()
{
	return std::format("{}/s", byte_format(this->speed()));
}

std::string CBytesSpeed::time_remain()
{
	auto speed = CBytesSpeed::speed();
	if (0 == speed)
		return "剩余时间：--";

	auto dwSeconds = (this->bytes_all - this->queue_list.back().current_pos) / speed;
	div_t dTime = div(dwSeconds, 60);

	return std::format("剩余时间：{:02}:{:02}", dTime.quot, dTime.rem);
}

void CBytesSpeedSector::set_all_bytes(uint64_t file_size)
{
	this->bytes_all = (file_size / SIZE_PER_SECTOR) + ((file_size & SIZE_PER_SECTOR) ? 1 : 0);
}

uint64_t CBytesSpeedSector::speed()
{
	return CBytesSpeed::speed() * SIZE_PER_SECTOR;
}
