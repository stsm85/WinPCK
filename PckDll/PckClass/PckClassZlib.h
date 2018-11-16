#pragma once
#define Z_MAX_COMPRESSION				12
#define Z_Default_COMPRESSION			9

#include "PckClassLog.h"
#include "PckDefines.h"
#include "PckClassBaseFeatures.h"

class CPckClassZlib :
	protected virtual CPckClassBaseFeatures
{
public:
	CPckClassZlib();
	~CPckClassZlib();

private:

	struct COMPRESS_FUNC
	{
		unsigned long(*compressBound)(unsigned long);
		int(*compress)(void *dest, unsigned long *destLen, const void *source, unsigned long sourceLen, int level);
	}m_PckCompressFunc;

	int	m_compress_level;

public:

	void init_compressor();

	int check_zlib_header(void *data);
	unsigned long compressBound(unsigned long sourceLen);
	int	compress(void *dest, unsigned long *destLen, const void *source, unsigned long sourceLen);
	int decompress(void *dest, unsigned long  *destLen, const void *source, unsigned long sourceLen);
	int decompress_part(void *dest, unsigned long  *destLen, const void *source, unsigned long sourceLen, unsigned long  fullDestLen);

	//获取数据压缩后的大小，如果源大小小于一定值就不压缩
	unsigned long GetCompressBoundSizeByFileSize(unsigned long &dwFileClearTextSize, unsigned long &dwFileCipherTextSize, unsigned long dwFileSize);

private:

	static unsigned long compressBound_zlib(unsigned long sourceLen);
	static int	compress_zlib(void *dest, unsigned long *destLen, const void *source, unsigned long sourceLen, int level);
	static unsigned long compressBound_libdeflate(unsigned long sourceLen);
	static int	compress_libdeflate(void *dest, unsigned long *destLen, const void *source, unsigned long sourceLen, int level);

};

