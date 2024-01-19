#pragma once

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

