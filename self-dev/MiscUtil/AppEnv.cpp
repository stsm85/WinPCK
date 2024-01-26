//////////////////////////////////////////////////////////////////////
// AppEnv.cpp
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////

#include "AppEnv.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <array>

#ifndef APPNAME
#error "APPNAME must be defined"
#endif

#include "AccessCheck.h"

auto get_absolute_path()
{
	fs::path app_file;

	if(nullptr != __argv)
		app_file = std::move(fs::path(__argv[0]));
	else
		app_file = std::move(fs::path(__wargv[0]));


	if (!app_file.is_absolute()) {

		wchar_t exeFullPath[1024];
		::GetModuleFileNameW(NULL, exeFullPath, 1024);


		return fs::path(exeFullPath);
	}
	return app_file;
}

const fs::path& AppDirectry()
{
	static fs::path apppath;
	if (apppath.empty()) {
		apppath = get_absolute_path().parent_path();
	}
	return apppath;
}

const char* AppDirectryA()
{
	static std::string apppath;
	if (0 == apppath.size())
	{
		try {
			apppath.assign(AppDirectry().string());
		}
		catch (std::exception ex)
		{
			printf(ex.what());
		}
	}
	return apppath.c_str();
}

const wchar_t* AppDirectryW()
{
	static std::wstring apppath;
	if (0 == apppath.size())
	{
		try {
			apppath.assign(AppDirectry().wstring());
		}
		catch (std::exception ex)
		{
			printf(ex.what());
		}
	}
	return apppath.c_str();
}


const fs::path& AppDataPath(const std::string& appname)
{
	static fs::path app_path;
	if (app_path.empty()) {
		app_path = fs::path(getenv("APPDATA")) / appname;
	}
	return app_path;
}

const char* AppDataPathA(const std::string& appname)
{
	static std::string apppath;
	if (0 == apppath.size())
	{
		try {
			apppath.assign(AppDataPath(appname).string());
		}
		catch (std::exception ex)
		{
			printf(ex.what());
		}
	}
	return apppath.c_str();
}

const wchar_t* AppDataPathW(const std::string& appname)
{
	static std::wstring apppath;
	if (0 == apppath.size())
	{
		try {
			apppath.assign(AppDataPath(appname).wstring());
		}
		catch (std::exception ex)
		{
			printf(ex.what());
		}
	}
	return apppath.c_str();
}

const fs::path& LocalAppDataPath(const std::string& appname)
{
	static fs::path app_path;
	if (app_path.empty()) {
		app_path = fs::path(getenv("LOCALAPPDATA")) / appname;
	}
	return app_path;
}

const char* LocalAppDataPathA(const std::string& appname)
{
	static std::string apppath;
	if (0 == apppath.size())
	{
		try {
			apppath.assign(LocalAppDataPath(appname).string());
		}
		catch (std::exception ex)
		{
			printf(ex.what());
		}
	}
	return apppath.c_str();
}

const wchar_t* LocalAppDataPathW(const std::string& appname)
{
	static std::wstring apppath;
	if (0 == apppath.size())
	{
		try {
			apppath.assign(LocalAppDataPath(appname).wstring());
		}
		catch (std::exception ex)
		{
			printf(ex.what());
		}
	}
	return apppath.c_str();
}


const fs::path& AccessableDataPath(uint32_t dwGenericAccessMask, const std::string& appname)
{
	std::array<const fs::path, 3> paths = {
		AppDirectry(),
		AppDataPath(appname),
		LocalAppDataPath(appname)
	};

	for (auto& path : paths) {

		if (CanAccessFile(path.string().c_str(), dwGenericAccessMask)) {
			return path;
		}
	}

	throw std::runtime_error("no data file is accessable");
}

const fs::path& WriteableDataPath()
{
	return AccessableDataPath(GENERIC_WRITE);
}

const char* WriteableDataPathA()
{
	static std::string apppath;
	if (0 == apppath.size())
	{
		try {
			apppath.assign(WriteableDataPath().string());
		}
		catch (std::exception ex)
		{
			printf(ex.what());
		}
	}
	return apppath.c_str();
}

const wchar_t* WriteableDataPathW()
{
	static std::wstring apppath;
	if (0 == apppath.size())
	{
		try {
			apppath.assign(WriteableDataPath().wstring());
		}
		catch (std::exception ex)
		{
			printf(ex.what());
		}
	}
	return apppath.c_str();
}