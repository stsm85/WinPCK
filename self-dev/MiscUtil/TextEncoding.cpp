//////////////////////////////////////////////////////////////////////
// TextEncoding.cpp
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////

#include "TextEncoding.h"

#include <stdint.h>

namespace TextEncoding
{
#define DATATYPE_UTF8_DETECT_RTN {if(0 == *s) return EncodingType::RAW;else	{isNotUTF8 = true; break;}}	

	EncodingType DataType(const char* _s, size_t size)
	{
		const uint8_t* s = (const uint8_t*)_s;
		bool  isNotUTF8 = false;

		while (*s) {
			if (*s <= 0x7f) {
			}
			else if (*s <= 0xdf) {
				if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN
			}
			else if (*s <= 0xef) {
				if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
				if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			}
			else if (*s <= 0xf7) {
				if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
				if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
				if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			}
			else if (*s <= 0xfb) {
				if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
				if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
				if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
				if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			}
			else if (*s <= 0xfd) {
				if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
				if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
				if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
				if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
				if ((*++s & 0xc0) != 0x80) DATATYPE_UTF8_DETECT_RTN;
			}
			++s;
		}

		while (*s) {
			++s;
		}

		if (size > ((char*)s - _s))
			return EncodingType::RAW;
		else
			return isNotUTF8 ? EncodingType::ANSI : EncodingType::UTF8;
	}

	EncodingType TextDataType(const char*& _s, size_t size)
	{
		auto textType = EncodingType::ANSI;
		if (0 != size) {

			*(uint16_t*)(_s + size) = 0;

			if (0xfeff == *(uint16_t*)_s) {

				textType = EncodingType::UCS2;
				_s += 2;
			}
			else if ((0xbbef == *(uint16_t*)_s) && (0xbf == *(_s + 3))) {

				textType = EncodingType::UTF8;
				_s += 3;
			}
			else {
				textType = DataType((char*)_s, size);
			}
		}
		return textType;
	}
}