#pragma once
//////////////////////////////////////////////////////////////////////
// MemBufferWriter.h
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////


#include "MemBuffer.h"

class CMemBufferWriter :
    public CMemBuffer
{
public:
	CMemBufferWriter() = default;
	virtual ~CMemBufferWriter() = default;

	void reset(int capacity = 0);

	void WriteFile(std::wstring file) const;

	void WriteSqliteInt(uint64_t val);
	void WriteSqliteString(const std::string& in);

	std::string& data();

	void WriteByte(uint8_t val);
	void WriteUint16(uint16_t val);
	void WriteUint32(uint32_t val);
	void WriteString(const std::string& in);
	void WriteData(int size, void* in);
	void WriteData(int start, int size, void* in);
	void FillZeros(int start, int size);
	void AppendZeros(int size);

};

