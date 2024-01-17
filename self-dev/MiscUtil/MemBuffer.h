#pragma once
#include <string>

/// <summary>
/// �ڴ��Ķ�ȡ��д��
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