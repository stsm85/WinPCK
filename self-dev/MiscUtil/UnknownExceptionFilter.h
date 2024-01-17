#pragma once

/// <summary>
/// ³ÌÐò±ÀÀ£¹´×Ó
/// </summary>
/// <param name="clear_works"></param>
/// <returns></returns>

BOOL InstallExceptionFilter(void (*clear_works)() = nullptr);
LONG WINAPI ExpFilter(EXCEPTION_POINTERS* pExp, DWORD dwExpCode);
const std::string& ExpFilterMsg();
