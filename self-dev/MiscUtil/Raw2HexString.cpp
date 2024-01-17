//////////////////////////////////////////////////////////////////////
// Raw2HexString.cpp: WinPCK 辅助函数部分
// 将二进制数据转换为二进制文本
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2018.5.29
//////////////////////////////////////////////////////////////////////

#include "Raw2HexString.h"
#include <memory>


#pragma warning ( disable : 4996 )

#define ADDRESS_FMTSTR		"%016llxh: "
#define ADDRESS_STRLEN		19
//strlen("+0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +a +b +c +d +e +f ")
#define	DATAHEX_STRLEN		19
#define	BYTEHEX_STRLEN		3
#define	BYTES_PER_LINE		16
#define	LINE_SPACECRLF_LEN	3
#define LINE_STRLEN			(ADDRESS_STRLEN + BYTEHEX_STRLEN * BYTES_PER_LINE + DATAHEX_STRLEN)
//#define LINE_STRLEN			76

CRaw2HexString::CRaw2HexString(uint8_t* lpbuf, size_t rawlength, __int64 iShowStartAddr)
{

	const char	*lpszByte2Raw[0x100] = {
		"00 ", "01 ", "02 ", "03 ", "04 ", "05 ", "06 ", "07 ", "08 ", "09 ", "0a ", "0b ", "0c ", "0d ", "0e ", "0f ",
		"10 ", "11 ", "12 ", "13 ", "14 ", "15 ", "16 ", "17 ", "18 ", "19 ", "1a ", "1b ", "1c ", "1d ", "1e ", "1f ",
		"20 ", "21 ", "22 ", "23 ", "24 ", "25 ", "26 ", "27 ", "28 ", "29 ", "2a ", "2b ", "2c ", "2d ", "2e ", "2f ",
		"30 ", "31 ", "32 ", "33 ", "34 ", "35 ", "36 ", "37 ", "38 ", "39 ", "3a ", "3b ", "3c ", "3d ", "3e ", "3f ",
		"40 ", "41 ", "42 ", "43 ", "44 ", "45 ", "46 ", "47 ", "48 ", "49 ", "4a ", "4b ", "4c ", "4d ", "4e ", "4f ",
		"50 ", "51 ", "52 ", "53 ", "54 ", "55 ", "56 ", "57 ", "58 ", "59 ", "5a ", "5b ", "5c ", "5d ", "5e ", "5f ",
		"60 ", "61 ", "62 ", "63 ", "64 ", "65 ", "66 ", "67 ", "68 ", "69 ", "6a ", "6b ", "6c ", "6d ", "6e ", "6f ",
		"70 ", "71 ", "72 ", "73 ", "74 ", "75 ", "76 ", "77 ", "78 ", "79 ", "7a ", "7b ", "7c ", "7d ", "7e ", "7f ",
		"80 ", "81 ", "82 ", "83 ", "84 ", "85 ", "86 ", "87 ", "88 ", "89 ", "8a ", "8b ", "8c ", "8d ", "8e ", "8f ",
		"90 ", "91 ", "92 ", "93 ", "94 ", "95 ", "96 ", "97 ", "98 ", "99 ", "9a ", "9b ", "9c ", "9d ", "9e ", "9f ",
		"a0 ", "a1 ", "a2 ", "a3 ", "a4 ", "a5 ", "a6 ", "a7 ", "a8 ", "a9 ", "aa ", "ab ", "ac ", "ad ", "ae ", "af ",
		"b0 ", "b1 ", "b2 ", "b3 ", "b4 ", "b5 ", "b6 ", "b7 ", "b8 ", "b9 ", "ba ", "bb ", "bc ", "bd ", "be ", "bf ",
		"c0 ", "c1 ", "c2 ", "c3 ", "c4 ", "c5 ", "c6 ", "c7 ", "c8 ", "c9 ", "ca ", "cb ", "cc ", "cd ", "ce ", "cf ",
		"d0 ", "d1 ", "d2 ", "d3 ", "d4 ", "d5 ", "d6 ", "d7 ", "d8 ", "d9 ", "da ", "db ", "dc ", "dd ", "de ", "df ",
		"e0 ", "e1 ", "e2 ", "e3 ", "e4 ", "e5 ", "e6 ", "e7 ", "e8 ", "e9 ", "ea ", "eb ", "ec ", "ed ", "ee ", "ef ",
		"f0 ", "f1 ", "f2 ", "f3 ", "f4 ", "f5 ", "f6 ", "f7 ", "f8 ", "f9 ", "fa ", "fb ", "fc ", "fd ", "fe ", "ff "
	};

	m_ok = FALSE;

	uint32_t dwDst = ((rawlength >> 4) + 2) * LINE_STRLEN;
	if(NULL == (m_buffer = (char*)malloc(dwDst))) {
		return;
	}


	__int64 i;
	char *lpbuffer = m_buffer;

	uint8_t raw16[32];
	memset(raw16, 0, 32);
	raw16[16] = ' ';
	raw16[17] = '\r';
	raw16[18] = '\n';

	int nFullLine = (rawlength & 0xfffffff0) + 0x10;

	memcpy(lpbuffer, "Address          : +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +a +b +c +d +e +f 0123456789abcdef \r\n", LINE_STRLEN);
	lpbuffer += LINE_STRLEN;

	for(i = BYTES_PER_LINE;i < nFullLine;i += BYTES_PER_LINE) {
		//复制文字显示区
		memcpy(raw16, &lpbuf[i - BYTES_PER_LINE], BYTES_PER_LINE);

		//打印地址
		sprintf(lpbuffer, ADDRESS_FMTSTR, i - BYTES_PER_LINE + iShowStartAddr);
		lpbuffer += ADDRESS_STRLEN;

		//打印hex区
		for(int j = 0;j < BYTES_PER_LINE;++j) {

			memcpy(lpbuffer, lpszByte2Raw[raw16[j]], BYTEHEX_STRLEN);
			lpbuffer += BYTEHEX_STRLEN;

		}

		//文字显示区 特殊字符过滤
		for(int j = 0;j < BYTES_PER_LINE;j++) {
			if(0x20 > raw16[j])
				raw16[j] = '.';
		}

		//把处理完的数据复制到目标缓冲区
		memcpy(lpbuffer, raw16, DATAHEX_STRLEN);
		lpbuffer += DATAHEX_STRLEN;

	}

	//处理余下的部分
	int nLeftBytes = rawlength & 0xf;

	if(0 != nLeftBytes) {
		memcpy(raw16, &lpbuf[i - BYTES_PER_LINE], nLeftBytes);

		sprintf(lpbuffer, ADDRESS_FMTSTR, i - BYTES_PER_LINE + iShowStartAddr);
		lpbuffer += ADDRESS_STRLEN;

		for(int j = 0;j < BYTES_PER_LINE;++j) {
			if(j < nLeftBytes)
				memcpy(lpbuffer, lpszByte2Raw[raw16[j]], BYTEHEX_STRLEN);
			else
				memcpy(lpbuffer, "   ", BYTEHEX_STRLEN);
			lpbuffer += BYTEHEX_STRLEN;
		}

		for(int j = 0;j < nLeftBytes;j++) {
			if(0x20 > raw16[j])
				raw16[j] = '.';
		}

		memcpy(lpbuffer, raw16, nLeftBytes);
		lpbuffer += nLeftBytes;
	}

	*lpbuffer = 0;

	m_ok = TRUE;
}

CRaw2HexString::~CRaw2HexString()
{
	if(NULL != m_buffer)
		free(m_buffer);
}

const char *	CRaw2HexString::GetHexString()
{
	return m_ok ? m_buffer : "error in Raw2Hex!";
}
