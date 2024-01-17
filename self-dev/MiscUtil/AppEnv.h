#pragma once
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
