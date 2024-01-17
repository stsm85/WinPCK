#include "MemBufferWriter.h"
#include "StdFileFuncs.h"
#include <stdexcept>

void CMemBufferWriter::reset(int capacity)
{
	this->m_buffer.clear();
	if (0 != capacity)
		this->m_buffer.reserve(capacity);
}

void CMemBufferWriter::WriteFile(std::wstring file) const
{
	StdFile::write(file.c_str(), this->m_buffer);
}

void CMemBufferWriter::WriteSqliteInt(uint64_t val)
{
	uint64_t uval = val;
	uint8_t byteval = 0;

	while (uval >= 0x80) {

		byteval = (uval & 0x7f) | 0x80;
		this->m_buffer.append(1, (char)byteval);
		uval >>= 7;
	}
	this->m_buffer.append(1, (char)uval);
}

void CMemBufferWriter::WriteSqliteString(const std::string& in)
{
	this->WriteSqliteInt(in.size());
	this->m_buffer.append(in);
}

std::string& CMemBufferWriter::data()
{
	return this->m_buffer;
}

void CMemBufferWriter::WriteByte(uint8_t val)
{
	this->m_buffer.push_back(val);
}

void CMemBufferWriter::WriteUint16(uint16_t val)
{
	this->m_buffer.append((char*)&val, sizeof(uint16_t));
}

void CMemBufferWriter::WriteUint32(uint32_t val)
{
	this->m_buffer.append((char*)&val, sizeof(uint32_t));
}

void CMemBufferWriter::WriteString(const std::string& in)
{
	this->m_buffer.append(in);
}

void CMemBufferWriter::WriteData(int size, void* in)
{
	this->m_buffer.append((char*)in, size);
}

void CMemBufferWriter::WriteData(int start, int size, void* in)
{
	auto size_after_write = start + size;
	if (size_after_write > this->m_buffer.size())
	{
		this->m_buffer.resize(size_after_write);
	}
	memcpy(this->m_buffer.data() + start, in, size);
}

void CMemBufferWriter::FillZeros(int start, int size)
{
	auto size_after_write = start + size;
	if (size_after_write > this->m_buffer.size())
	{
		this->m_buffer.resize(size_after_write);
	}
	memset(this->m_buffer.data() + start, 0, size);
}

void CMemBufferWriter::AppendZeros(int size)
{
	this->m_buffer.append(size, 0);
}
