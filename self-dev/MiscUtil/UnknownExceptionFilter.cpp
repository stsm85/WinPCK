
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <minidumpapiset.h>
#include <string>

#include "UnknownExceptionFilter.h"
#include "StackWalkerLiqf.h"
#include <stdio.h>
#include <format>

#include "AppEnv.h"

#ifndef APPNAME
#error "APPNAME must be defined"
#endif

#pragma comment(lib, "Dbghelp.lib") 

namespace liqf_exception {

	static auto dmpfile_path = WriteableDataPath();

	static auto stack_info = (dmpfile_path / (char*)u8"" APPNAME "_exception.log\x00\x00 By liqf/stsm/李秋枫 stsm85@126.com").wstring();
	static auto callstrace_info = (dmpfile_path / (char*)u8"" APPNAME "_callstrace.log\x00\x00 By liqf/stsm/李秋枫 stsm85@126.com").wstring();
	static auto minidmp_info = (dmpfile_path / APPNAME "_crashd.dmp").wstring();

	void (*do_some_before_exit)() = nullptr;
#define STACKDUMP_SIZE		256
#define MAX_STACKDUMP_SIZE	8192

	static std::string call_stack_info;

	void write_stack_info(auto info)
	{
		static char			buf[MAX_STACKDUMP_SIZE];
		static HANDLE		hFile;
		static SYSTEMTIME	tm;
		static DWORD		len, i, j;
		static char* stack, * esp;
		static CONTEXT* context;

		hFile = ::CreateFileW(stack_info.c_str(), GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			::SetFilePointer(hFile, 0, 0, FILE_END);
			::GetLocalTime(&tm);

			context = info->ContextRecord;

#pragma warning(push)
#pragma warning(disable: 4477)
#pragma warning(disable: 4313)
			len = sprintf(buf,
#ifdef _WIN64
				"------ %s -----\r\n"
				" Date        : %d/%02d/%02d %02d:%02d:%02d\r\n"
				" Code/Addr   : %p / %p\r\n"
				" AX/BX/CX/DX : %p / %p / %p / %p\r\n"
				" SI/DI/BP/SP : %p / %p / %p / %p\r\n"
				" 08/09/10/11 : %p / %p / %p / %p\r\n"
				" 12/13/14/15 : %p / %p / %p / %p\r\n"
				"------- 堆栈信息 -----\r\n"
				, APPNAME
				, tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond
				, info->ExceptionRecord->ExceptionCode, info->ExceptionRecord->ExceptionAddress
				, context->Rax, context->Rbx, context->Rcx, context->Rdx
				, context->Rsi, context->Rdi, context->Rbp, context->Rsp
				, context->R8, context->R9, context->R10, context->R11
				, context->R12, context->R13, context->R14, context->R15
#else
				"------ %s -----\r\n"
				" Date        : %d/%02d/%02d %02d:%02d:%02d\r\n"
				" Code/Addr   : %X / %p\r\n"
				" AX/BX/CX/DX : %08x / %08x / %08x / %08x\r\n"
				" SI/DI/BP/SP : %08x / %08x / %08x / %08x\r\n"
				"------- 堆栈信息 -----\r\n"
				, APPNAME
				, tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond
				, info->ExceptionRecord->ExceptionCode, info->ExceptionRecord->ExceptionAddress
				, context->Eax, context->Ebx, context->Ecx, context->Edx
				, context->Esi, context->Edi, context->Ebp, context->Esp
#endif
			);

			::WriteFile(hFile, buf, len, &len, 0);

#ifdef _WIN64
			esp = (char*)context->Rsp;
#else
			esp = (char*)context->Esp;
#endif

			for (i = 0; i < MAX_STACKDUMP_SIZE / STACKDUMP_SIZE; i++) {
				stack = esp + (i * STACKDUMP_SIZE);
				if (::IsBadReadPtr(stack, STACKDUMP_SIZE))
					break;
				len = 0;
				for (j = 0; j < STACKDUMP_SIZE / sizeof(DWORD_PTR); j++)
					len += sprintf(buf + len, "%p%s", ((DWORD_PTR*)stack)[j],
						((j + 1) % (32 / sizeof(DWORD_PTR))) ? " " : "\r\n");
				::WriteFile(hFile, buf, len, &len, 0);
			}
#pragma warning(pop)

			len = sprintf(buf, "------------------------\r\n\r\n");
			::WriteFile(hFile, buf, len, &len, 0);
			::CloseHandle(hFile);
		}
	}

	void write_callstack_info(auto context)
	{
		StackWalkerLiqf sw(callstrace_info.c_str());
		sw.ShowCallstack(GetCurrentThread(), context);
	}

	void write_minidump(auto pExInfo)
	{
		HANDLE hFile = ::CreateFileW(minidmp_info.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			MINIDUMP_EXCEPTION_INFORMATION einfo;
			einfo.ThreadId = ::GetCurrentThreadId();
			einfo.ExceptionPointers = pExInfo;
			einfo.ClientPointers = FALSE;

			::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpNormal, &einfo, NULL, NULL);
			::CloseHandle(hFile);
		}
	}

	LONG WINAPI Local_UnhandledExceptionFilter(struct _EXCEPTION_POINTERS* info)
	{

		//write_stack_info(info);
		write_callstack_info(info->ContextRecord);
		write_minidump(info);

		if (nullptr != do_some_before_exit)
			do_some_before_exit();

		::MessageBoxW(0, std::format(L"出现异常，程序即将退出。\r\n异常信息已被保存到:\r\n{}\r\n", dmpfile_path.wstring()).c_str(), L"" APPNAME, MB_OK);

		return	EXCEPTION_EXECUTE_HANDLER;
	}
}

BOOL InstallExceptionFilter(void (*clear_works)())
{
	//char	buf[MAX_PATH];

	//::GetModuleFileNameA(NULL, buf, sizeof(buf));
	//strcpy(strrchr(buf, '.'), (char*)u8"_exception.log\x00\x00 By liqf/stsm/李秋枫 stsm85@126.com");
	//liqf_exception::ExceptionLogFile = strdup(buf);

	//strcpy(strrchr(buf, '.'), (char*)u8".dmp");
	//liqf_exception::ExceptionDmpFile = strdup(buf);

	//liqf_exception::ExceptionTitle = strdup(title);
	//ExceptionLogInfo = "出现异常，程序即将退出。\r\n异常信息已被保存到:\r\n{}\r\n";

	liqf_exception::do_some_before_exit = clear_works;

	::SetUnhandledExceptionFilter(&liqf_exception::Local_UnhandledExceptionFilter);

	return	TRUE;
}

LONG WINAPI ExpFilter(EXCEPTION_POINTERS* pExp, DWORD dwExpCode)
{
	StackWalkerMem sw;
	sw.ShowCallstack(GetCurrentThread(), pExp->ContextRecord);

	liqf_exception::call_stack_info = std::move(sw.text());
	return EXCEPTION_EXECUTE_HANDLER;
}

const std::string& ExpFilterMsg()
{
	return liqf_exception::call_stack_info;
}
