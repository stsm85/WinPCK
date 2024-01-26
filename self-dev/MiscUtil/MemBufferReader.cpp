//////////////////////////////////////////////////////////////////////
// MemBufferReader.cpp
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////


#include "MemBufferReader.h"
#include "StdFileFuncs.h"
#include <stdexcept>

void CMemBufferReader::reset()
{
	this->buffersize = this->m_buffer.size();
	this->m_offset = 0;
	this->m_pointer.char_ptr = this->m_buffer.data();
}

void CMemBufferReader::ReadFile(const std::wstring& file)
{
	this->m_buffer = StdFile::read_all_bytes(file.c_str());
	this->reset();
}

void CMemBufferReader::Attach(const std::string& buffer)
{
	this->m_buffer = buffer;
	this->reset();
}

void CMemBufferReader::Attach(const uint8_t* buffer, size_t size)
{
	this->m_buffer.assign((const char*)buffer, size);
	this->reset();
}

uint8_t CMemBufferReader::ReadByte()
{
	if (this->m_offset >= this->buffersize)
		throw std::overflow_error("read buffer overflow");

	uint8_t byteval = *this->m_pointer.byte_ptr;
	this->m_pointer.byte_ptr++;
	this->m_offset++;

	return byteval;
}


uint64_t CMemBufferReader::ReadSqliteInt() {

	try {
		auto byteval = this->ReadByte();
		if (0 != (byteval & 0x80)) {
			return (byteval & 0x7f) + this->ReadSqliteInt() * 0x80;
		}
		else {
			return byteval;
		}
	}
	catch (std::exception) {
		return -1;
	}
}

uint16_t CMemBufferReader::ReadUint16()
{
	if (this->m_offset > this->buffersize + sizeof(uint16_t))
		throw std::overflow_error("read buffer error");

	auto int16_val = *(uint16_t*)this->m_pointer.byte_ptr;
	this->m_pointer.byte_ptr += sizeof(uint16_t);
	this->m_offset += sizeof(uint16_t);
	return int16_val;
}

uint32_t CMemBufferReader::ReadUint32()
{
	int offset_after_read = this->m_offset + sizeof(uint32_t);
	if (offset_after_read > this->buffersize)
		throw std::overflow_error("read buffer overflow");

	uint32_t val = *(uint32_t*)this->m_pointer.byte_ptr;
	this->m_pointer.byte_ptr += sizeof(uint32_t);
	this->m_offset += sizeof(uint32_t);

	return val;

}

uint32_t CMemBufferReader::ReadString(int size, std::string& out)
{
	int offset_after_read = this->m_offset + size;
	if (offset_after_read > this->buffersize)
		throw std::overflow_error("read buffer overflow");

	out.assign(this->m_pointer.char_ptr, size);
	this->m_pointer.char_ptr += size;
	this->m_offset = offset_after_read;
	return size;
}

uint32_t CMemBufferReader::ReadData(int size, void* out)
{
	int offset_after_read = this->m_offset + size;
	if (offset_after_read > this->buffersize)
		throw std::overflow_error("read buffer overflow");

	memcpy(out, this->m_pointer.char_ptr, size);
	this->m_pointer.char_ptr += size;
	this->m_offset = offset_after_read;
	return size;
}


//指定读取位置，不影响指针
uint32_t CMemBufferReader::ReadData(int start, int size, void* out) const
{
	int offset_after_read = this->m_offset + size;
	if (offset_after_read > this->buffersize)
		throw std::overflow_error("read buffer overflow");

	memcpy(out, this->m_buffer.data() + start, size);
	return size;
}

void CMemBufferReader::skip(int size)
{
	int offset_after_read = this->m_offset + size;
	if (offset_after_read > this->buffersize)
		throw std::overflow_error("read buffer overflow");

	this->m_pointer.char_ptr += size;
	this->m_offset = offset_after_read;
}

