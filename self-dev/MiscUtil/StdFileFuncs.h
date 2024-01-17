#pragma once

#define ENABLE_C_FOPEN 0
#define ENABLE_BIG_IO_BUFFER 0

#define _FILE_IO_BUFFER_SIZE_ (64 * 1024)

#include <fstream>

/// <summary>
/// 文件直接读取为std::string 或直接写入 std::string
/// </summary>

namespace StdFile {

#if ENABLE_C_FOPEN

	std::string read_c(const char* file);
	std::string read_c(const wchar_t* file);

	void write_c(const char* file, const std::string& content);
	void write_c(const wchar_t* file, const std::string& content);

#endif
	std::string read_all_bytes(const char* file);
	std::string read_all_bytes(const wchar_t* file);

	void write(const char* file, const std::string& content);
	void write(const wchar_t* file, const std::string& content);

}

class StdFileRead
{
public:
	StdFileRead() = default;
	~StdFileRead() = default;

	static std::string read_all_bytes(const char* file);
	static std::string read_all_bytes(const wchar_t* file);

private:

	template<typename T>
	static std::string _read_all(const T* file);
#if ENABLE_BIG_IO_BUFFER
	char m_io_buffer[_FILE_IO_BUFFER_SIZE_];
#endif
};

class StdFileWrite
{
public:
	StdFileWrite() = default;
	~StdFileWrite() = default;

	void write(const char* file, const std::string& content);
	void write(const wchar_t* file, const std::string& content);

	//void append(const char* file, const std::string& content);
	//void append(const wchar_t* file, const std::string& content);

private:

	template<typename T>
	void _write(const T* file, const std::string& content);
#if ENABLE_BIG_IO_BUFFER
	char m_io_buffer[_FILE_IO_BUFFER_SIZE_];
#endif
};

