//////////////////////////////////////////////////////////////////////
// BytesConv.cpp
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////

#include "BytesConv.h"


std::string byte_format(uint64_t bytes)
{
	constexpr char sizetext[][4] = { " KB", " MB", " GB", " TB", " PB", " EB", " ZB", " YB" };
	if (1024 > bytes)
	{
		return std::format("{} B", bytes);
	}
	else {
		for (auto text : sizetext) {

			auto byte_k = bytes >> 10;
			if(1024 > byte_k)
				return std::format("{:.2f}{}", (double)bytes / 1024.0, text);
			bytes = byte_k;
		}
		
	}
	return "ERR";
}

std::wstring wbyte_format(uint64_t bytes)
{
	constexpr wchar_t sizetext[][4] = { L" KB", L" MB", L" GB", L" TB", L" PB", L" EB", L" ZB", L" YB" };
	if (1024 > bytes)
	{
		return std::format(L"{} B", bytes);
	}
	else {
		for (auto text : sizetext) {

			auto byte_k = bytes >> 10;
			if (1024 > byte_k)
				return std::format(L"{:.2f}{}", (double)bytes / 1024.0, text);
			bytes = byte_k;
		}

	}
	return L"ERR";
}
#if 0
auto format_byte_size(uint64_t bytesize) -> std::wstring
{
	constexpr uint64_t one_kb = 1ui64 << 10;
	constexpr uint64_t one_mb = 1ui64 << 20;
	constexpr uint64_t one_gb = 1ui64 << 30;
	constexpr uint64_t one_tb = 1ui64 << 40;
	constexpr uint64_t one_pb = 1ui64 << 50;
	constexpr uint64_t one_eb = 1ui64 << 60;
	//constexpr uint64_t one_zb = 1ui64 << 70;
	//constexpr uint64_t one_yb = 1ui64 << 80;

	constexpr wchar_t bytes_name[][3] = {
		L"KB",
		L"MB",
		L"GB",
		L"TB",
		L"PB",
		L"EB"
	};

	if (bytesize <= one_kb)
	{
		return std::format(L"{} {}", bytesize, L"字节");
	}
	else {

		for (auto& xb : bytes_name)
		{
			if (bytesize < one_mb)
			{
				return std::format(L"{:.2f} {}", bytesize / (double)one_kb, xb);
			}
			else {
				bytesize >>= 10;
			}
		}

		auto loc = std::source_location::current();
		throw std::runtime_error(std::format("error in function: {}, file: {}, line: {}", loc.function_name(), loc.file_name(), loc.line()));
	}

}
#endif