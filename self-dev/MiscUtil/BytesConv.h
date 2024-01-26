#pragma once
//////////////////////////////////////////////////////////////////////
// BytesConv.h
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <string>
#include <format>

/// <summary>
/// 字节数转换为 KB MB GB TB....
/// </summary>
/// <param name="bytes"></param>
/// <returns></returns>

std::string byte_format(uint64_t bytes);
std::wstring wbyte_format(uint64_t bytes);
