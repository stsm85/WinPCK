#pragma once
#define Z_MAX_COMPRESSION				12
#define Z_Default_COMPRESSION			9

#include <functional>

typedef std::function<uint32_t(uint32_t)>	CompressBoundFunc;
typedef std::function<int(void*, ulong_t*, const void *, uint32_t, int)> CompressFunc;


class CPckClassZlib
{
public:
	CPckClassZlib();
	~CPckClassZlib();

private:

	struct COMPRESS_FUNC
	{
		CompressBoundFunc	compressBound;
		CompressFunc		compress;
		//uint32_t(*compressBound)(uint32_t);
		//int(*compress)(void *dest, ulong_t *destLen, const void *source, uint32_t sourceLen, int level);
	}m_PckCompressFunc;

	int	m_compress_level;

public:

	int init_compressor(int level);

	int check_zlib_header(void *data);
	uint32_t compressBound(uint32_t sourceLen);
	int	compress(void *dest, ulong_t *destLen, const void *source, uint32_t sourceLen);
	int decompress(void *dest, ulong_t *destLen, const void *source, uint32_t sourceLen);
	int decompress_part(void *dest, ulong_t  *destLen, const void *source, uint32_t sourceLen, uint32_t fullDestLen);

	//获取数据压缩后的大小，如果源大小小于一定值就不压缩
	unsigned long GetCompressBoundSizeByFileSize(ulong_t &dwFileClearTextSize, ulong_t &dwFileCipherTextSize, uint32_t dwFileSize);

private:

	static uint32_t compressBound_zlib(uint32_t sourceLen);
	static int	compress_zlib(void *dest, ulong_t *destLen, const void *source, uint32_t sourceLen, int level);
	static uint32_t compressBound_libdeflate(uint32_t sourceLen);
	static int	compress_libdeflate(void *dest, ulong_t *destLen, const void *source, uint32_t sourceLen, int level);

};

