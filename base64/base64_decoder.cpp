
#include "base64.h"

#if BASE64_ZUP
const uint32_t BASE64_DECODE[] = {
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //0
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //1
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62,  0, 63,  0,  0, //2
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0, //3
	 0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, //4
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,  0,  0,  0,  0, //5
	 0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, //6
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,  0,  0,  0,  0,  0}; //7

#else

const uint32_t BASE64_DECODE[] = {	 
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //0
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //1
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62,  0,  0,  0, 63, //2
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0, //3
	 0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, //4
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,  0,  0,  0,  0, //5
	 0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, //6
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,  0,  0,  0,  0,  0}; //7

#endif


//编码函数(原数据地址，原数据字节大小，编码输出地址)，使用char作为类型
BEXTERN void BEXPORT base64_decode(const void* pdata, const uint32_t data_size, void* out_pcode)
{
	const uint8_t* input = (const uint8_t*)pdata;
	const uint8_t* input_end = &input[data_size] - 4;
	uint8_t*       output = (uint8_t*)out_pcode;

	for (; input < input_end; input += 4, output += 3)
	{
		uint32_t decode0 = BASE64_DECODE[input[0]];
		uint32_t decode1 = BASE64_DECODE[input[1]];
		uint32_t decode2 = BASE64_DECODE[input[2]];
		uint32_t decode3 = BASE64_DECODE[input[3]];

		output[0] = decode0 << 2 | decode1 >> 4;
		output[1] = decode1 << 4 | decode2 >> 2;
		output[2] = decode2 << 6 | decode3;
	}


	uint32_t decode0 = BASE64_DECODE[input[0]];
	uint32_t decode1 = BASE64_DECODE[input[1]];

	output[0] = decode0 << 2 | decode1 >> 4;

	if (input[2] == '=') {

		output[1] = decode1 << 4;
		output[2] = 0;
	}
	else if (input[3] == '=') {
		uint32_t decode2 = BASE64_DECODE[input[2]];

		output[1] = decode1 << 4 | decode2 >> 2;
		output[2] = decode2 << 6;
		output[3] = 0;
	}
	else {
		uint32_t decode2 = BASE64_DECODE[input[2]];
		uint32_t decode3 = BASE64_DECODE[input[3]];

		output[1] = decode1 << 4 | decode2 >> 2;
		output[2] = decode2 << 6 | decode3;
		output[3] = 0;
	}

}

