#include "StdFileFuncs.h"
#include <memory>

namespace StdFile {


	void _read(std::ifstream& file, char* out, int count)
	{
		if (!file.read(out, count)) {

			if (!file.eof())
				throw std::runtime_error("file read fail");
		}
	}


	std::string _read_all(std::ifstream& file)
	{

		if (!file.is_open())
			throw std::runtime_error("Failed to open source file!");

		auto nFileSize = file.tellg();

		if (0 == nFileSize)
			throw std::runtime_error("file size is 0");

		file.seekg(0);

		std::string m_buffer;
		m_buffer.resize(nFileSize);

		StdFile::_read(file, m_buffer.data(), nFileSize);

		return std::move(m_buffer);
	}


	void _write(std::ofstream& file, const std::string& content)
	{
		if (!file.is_open())
			throw std::runtime_error("Failed to open source file!");

		if (!file.write(content.data(), content.size())) {

			throw std::runtime_error("file write fail");
		}
	}

	template<typename T>
	std::string _read_all(const T* file)
	{
		auto srcfile = std::ifstream(file, std::ios::binary | std::ios::ate);
		return std::move(StdFile::_read_all(srcfile));
	}

	template<typename T>
	void _write(const T* file, const std::string& content)
	{
		auto srcfile = std::ofstream(file, std::ios::binary | std::ios::trunc);
		StdFile::_write(srcfile, content);
	}

	std::string read_all_bytes(const char* file)
	{
		return std::move(StdFile::_read_all<char>(file));
	}

	std::string read_all_bytes(const wchar_t* file)
	{
		return std::move(StdFile::_read_all<wchar_t>(file));
	}

	void write(const char* file, const std::string& content)
	{
		StdFile::_write<char>(file, content);
	}

	void write(const wchar_t* file, const std::string& content)
	{
		StdFile::_write<wchar_t>(file, content);
	}


#if ENABLE_C_FOPEN

	std::string _read_c(FILE* fp)
	{
		if (!fp)
			throw std::runtime_error("open source file fail");

		fseek(fp, 0, SEEK_END);

		auto nFileSize = ftell(fp);

		if (0 == nFileSize)
			throw std::runtime_error("file size is 0");

		rewind(fp);

		//m_buffer = std::make_unique<uint8_t[]>(nFileSize);
		std::string m_buffer;
		m_buffer.resize(nFileSize);

		auto bytes_read = fread(m_buffer.data(), 1, nFileSize, fp);

		if (bytes_read < nFileSize) {
			m_buffer.clear();
			throw std::runtime_error("file read fail");
		}

		return std::move(m_buffer);
	}

	void _write_c(FILE* fp, const std::string& content)
	{
		if (!fp)
			throw std::runtime_error("open source file fail");

		auto bytes_read = fwrite(content.data(), 1, content.size(), fp);

		if (bytes_read < content.size()) 
			throw std::runtime_error("file read fail");
	}

	FILE* _open_c(const char* file, const char* mode)
	{
		return fopen(file, mode);
	}

	FILE* _open_c(const wchar_t* file, const wchar_t* mode)
	{
		return _wfopen(file, mode);
	}

	template<typename T>
	std::string _read_c(const T* file)
	{
		const T mode[] = { 'r','b' };

		std::unique_ptr<FILE, void(*)(FILE*)> fp_src(_open_c(file, mode), [](FILE* fp) {
			if (nullptr != fp)
				std::fclose(fp);
			});

		return std::move(_read_c(fp_src.get()));
	}

	//template std::string _read_c<char>(const char* file);
	//template std::string _read_c<wchar_t>(const wchar_t* file);

	std::string read_c(const char* file)
	{
		return std::move(StdFile::_read_c<char>(file));
	}

	std::string read_c(const wchar_t* file)
	{
		return std::move(StdFile::_read_c<wchar_t>(file));
	}

	template<typename T>
	void _write_c(const T* file, const std::string& content)
	{
		const T mode[] = { 'w','b' };
		std::unique_ptr<FILE, void(*)(FILE*)> fp_src(_open_c(file, mode), [](FILE* fp) {
			if (nullptr != fp)
				std::fclose(fp);
			});

		StdFile::_write_c(fp_src.get(), content);
	}

	//template void _write_c<char>(const char* file, const std::string& content);
	//template void _write_c<wchar_t>(const wchar_t* file, const std::string& content);

	void write_c(const char* file, const std::string& content)
	{
		StdFile::_write_c<char>(file, content);
	}

	void write_c(const wchar_t* file, const std::string& content)
	{
		StdFile::_write_c<wchar_t>(file, content);
	}

#endif

}

template<typename T>
std::string StdFileRead::_read_all(const T* file)
{
	auto srcfile = std::ifstream(file, std::ios::binary | std::ios::ate);
#if ENABLE_BIG_IO_BUFFER
	srcfile.rdbuf()->pubsetbuf(this->m_io_buffer, _FILE_IO_BUFFER_SIZE_);
#endif
	return std::move(StdFile::_read_all(srcfile));
}

//template std::string StdFileRead::read<char>(const char* file);
//template std::string StdFileRead::read<wchar_t>(const wchar_t* file);

std::string StdFileRead::read_all_bytes(const char* file)
{
	return std::move(StdFileRead::_read_all<char>(file));
}

std::string StdFileRead::read_all_bytes(const wchar_t* file)
{
	return std::move(StdFileRead::_read_all<wchar_t>(file));
}

template<typename T>
void StdFileWrite::_write(const T* file, const std::string& content)
{
	auto srcfile = std::ofstream(file, std::ios::binary | std::ios::trunc);
#if ENABLE_BIG_IO_BUFFER
	srcfile.rdbuf()->pubsetbuf(this->m_io_buffer, _FILE_IO_BUFFER_SIZE_);
#endif
	StdFile::_write(srcfile, content);
	srcfile.flush();
}

void StdFileWrite::write(const char* file, const std::string& content)
{
	this->_write<char>(file, content);
}

void StdFileWrite::write(const wchar_t* file, const std::string& content)
{
	this->_write<wchar_t>(file, content);
}