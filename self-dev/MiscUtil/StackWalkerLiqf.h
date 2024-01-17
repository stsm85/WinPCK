#pragma once

#include <string>
#include <format>

#include "StackWalker.h"

/// <summary>
/// 程序崩溃后，用于生成调用堆栈列表等信息
/// </summary>

class StackWalkerLiqf : public StackWalker
{
public:
	StackWalkerLiqf(const wchar_t* file);
	virtual ~StackWalkerLiqf();

private:
	virtual void OnOutput(LPCSTR szText);

	HANDLE		hFile;
	SYSTEMTIME	tm;
};

class StackWalkerMem : public StackWalker
{
public:
	StackWalkerMem() {
		this->buffer.reserve(32*1024);
	};
	virtual ~StackWalkerMem() = default;

	std::string& text()
	{
		return this->buffer;
	}

private:
	virtual void OnOutput(LPCSTR szText);

	std::string buffer;

};
