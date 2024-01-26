#pragma once
//////////////////////////////////////////////////////////////////////
// AccessCheck.h
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/// <summary>
/// 检查文件夹是否有写、读等权限
/// </summary>
/// <param name="strPath"></param>
/// <param name="dwGenericAccessMask">GENERIC_READ, GENERIC_WRITE, GENERIC_EXECUTE, GENERIC_ALL</param>
/// <returns></returns>
template<typename T>
BOOL CanAccessFile(const T* strPath, DWORD dwGenericAccessMask);

