#pragma once
//////////////////////////////////////////////////////////////////////
// TextEncoding.h
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////

namespace TextEncoding
{
	enum class EncodingType
	{
		ANSI,
		UCS2,
		UTF8,
		RAW
	};

	EncodingType	TextDataType(const char*& _s, size_t size);
}

