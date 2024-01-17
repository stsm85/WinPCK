#pragma once
#include <string>

/// <summary>
/// 内存块的读取、写入
/// </summary>

class CMemBuffer
{
public:
	CMemBuffer() = default;
	virtual ~CMemBuffer() = default;

	auto size()
	{
		return this->m_buffer.size();
	}

	uint32_t pos() const
	{
		return m_offset;
	}

protected:
	std::string m_buffer;

	size_t buffersize = 0;
	uint32_t m_offset = 0;

};
