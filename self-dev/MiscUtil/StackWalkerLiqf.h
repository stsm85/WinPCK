#pragma once
//////////////////////////////////////////////////////////////////////
// StackWalkerLiqf.h
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////


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
