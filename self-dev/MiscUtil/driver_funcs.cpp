#include "driver_funcs.h"
#include <Windows.h>
#include <format>

std::vector<std::string> get_cdrom_logic_drivers()
{
	std::vector<std::string> list;
	auto all_disk = ::GetLogicalDrives();
	char disk_off = 'A';

	while (all_disk)

	{

		if ((all_disk & 0x1) == 1)

		{
			//sprintf_s(device_path, "\\\\.\\%s", disk_path);
			auto device_path = std::format(R"({}:)", disk_off);
			auto x = ::GetDriveTypeA(device_path.c_str());
			if (::GetDriveTypeA(device_path.c_str()) == DRIVE_CDROM)
			{
				list.push_back(device_path);
			}
		}

		all_disk >>= 1;
		disk_off++;
	}

	return list;
}