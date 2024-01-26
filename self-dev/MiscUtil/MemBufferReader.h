#pragma once
//////////////////////////////////////////////////////////////////////
// MemBufferReader.h
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////

#include "MemBuffer.h"

class CMemBufferReader :
    public CMemBuffer
{
public:
	CMemBufferReader() = default;
	virtual ~CMemBufferReader() = default;

	void reset();

	void ReadFile(const std::wstring& file);
	void Attach(const uint8_t* buffer, size_t size);
	void Attach(const std::string& buffer);

	//读取错误时返回-1
	uint64_t ReadSqliteInt();

	uint8_t ReadByte();
	uint16_t ReadUint16();
	uint32_t ReadUint32();
	uint32_t ReadString(int size, std::string& out);
	uint32_t ReadData(int size, void* out);
	uint32_t ReadData(int start, int size, void* out) const;
	void skip(int size);

	const char* data_pointer() const
	{
		return this->m_pointer.char_ptr;
	}

	const char* data_pointer(int pos) const
	{
		return this->m_buffer.data() + pos;
	}

private:
	typedef union _CombinedPointer {

		const char* char_ptr;
		const uint8_t* byte_ptr;
	}CombinedPointer;

	CombinedPointer m_pointer;
	
};

