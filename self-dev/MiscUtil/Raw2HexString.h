#pragma once
//////////////////////////////////////////////////////////////////////
// Raw2HexString.h
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////


#include <stdint.h>

typedef int                 BOOL;

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

class CRaw2HexString
{
	//函数
public:
	//iShowStartAddr 改变显示的地址的数值的起始值
	CRaw2HexString(uint8_t* lpbuf, size_t rawlength, __int64 iShowStartAddr = 0);
	//CRaw2HexString(LPBYTE lpbuf, size_t rawlength, char *_dst, size_t _dstsize, __int64 iShowStartAddr = 0);
	//CRaw2HexString(LPBYTE lpbuf, size_t rawlength, wchar_t *_dst, size_t _dstsize, __int64 iShowStartAddr = 0);
	virtual ~CRaw2HexString();

	const char *GetHexString();
private:

	char *	m_buffer;
	BOOL	m_ok;
};