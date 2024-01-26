#pragma once
//////////////////////////////////////////////////////////////////////
// AppEnv.h
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////

#include <filesystem>
namespace fs = std::filesystem;

/// <summary>
/// 系统各环境变量路径
/// </summary>
/// <returns></returns>

#ifndef APPNAME
#define APPNAME "LiqfSoft"
#endif

const fs::path& AppDirectry();
const char* AppDirectryA();
const wchar_t* AppDirectryW();

const fs::path& AppDataPath(const std::string& appname = APPNAME);
const char* AppDataPathA(const std::string& appname = APPNAME);
const wchar_t* AppDataPathW(const std::string& appname = APPNAME);

const fs::path& LocalAppDataPath(const std::string& appname = APPNAME);
const char* LocalAppDataPathA(const std::string& appname = APPNAME);
const wchar_t* LocalAppDataPathW(const std::string& appname = APPNAME);

const fs::path& AccessableDataPath(uint32_t dwGenericAccessMask, const std::string& appname = APPNAME);

const fs::path& WriteableDataPath();
const char* WriteableDataPathA();
const wchar_t* WriteableDataPathW();
