#pragma once
//////////////////////////////////////////////////////////////////////
// MemBuffer.h
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////

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
