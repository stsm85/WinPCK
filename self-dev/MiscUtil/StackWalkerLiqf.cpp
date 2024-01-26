//////////////////////////////////////////////////////////////////////
// StackWalkerLiqf.cpp
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////


#include "StackWalkerLiqf.h"

StackWalkerLiqf::StackWalkerLiqf(const wchar_t* file)
{
	this->hFile = ::CreateFileW(file, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0);
	::SetFilePointer(hFile, 0, 0, FILE_END);
	::GetLocalTime(&tm);

	auto info = std::format(
		"------- 堆栈信息 -----\r\n"
		"生成时间        : {:04d}/{:02d}/{:02d} {:02d}:{:02d}:{:02d}\r\n"
		"------- 文件及调用堆栈 -----\r\n",
		tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);

	DWORD len = info.size();
	::WriteFile(hFile, info.c_str(), info.size(), &len, 0);

}

StackWalkerLiqf::~StackWalkerLiqf()
{
	auto info = std::string("------------------------\r\n\r\n");
	DWORD len = info.size();
	::WriteFile(hFile, info.c_str(), info.size(), &len, 0);
	::CloseHandle(hFile);
}

void StackWalkerLiqf::OnOutput(LPCSTR szText)
{
	DWORD len = strlen(szText);
	::WriteFile(hFile, szText, len, &len, 0);
}

void StackWalkerMem::OnOutput(LPCSTR szText)
{
	this->buffer.append(szText);
}