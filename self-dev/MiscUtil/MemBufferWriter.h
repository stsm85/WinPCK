#pragma once
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

