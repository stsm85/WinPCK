
#include "zlib.h"
#include "libdeflate.h"
//#include <Windows.h>
#include <assert.h>
#include "PckClassLog.h"
#include "PckClassZlib.h"


#pragma warning ( disable : 4267 )

CPckClassZlib::CPckClassZlib()
{
	init_compressor(Z_Default_COMPRESSION);
}

CPckClassZlib::~CPckClassZlib()
{
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

/*
在压缩文件时进行调用，目前调用的函数为UpdatePckFile和ReCompress，压缩文件索引时不更新
*/
int CPckClassZlib::init_compressor(int level)
{

	m_compress_level = level;

	if ((0 > m_compress_level) || (Z_MAX_COMPRESSION < m_compress_level))
		m_compress_level = Z_Default_COMPRESSION;

	if (Z_Default_COMPRESSION < m_compress_level) {

		m_PckCompressFunc.compressBound = compressBound_libdeflate;
		m_PckCompressFunc.compress = compress_libdeflate;

	}
	else {

		m_PckCompressFunc.compressBound = compressBound_zlib;
		m_PckCompressFunc.compress = compress_zlib;

	}

	return m_compress_level;
}

int CPckClassZlib::check_zlib_header(void *data)
{
	char cDeflateFlag = (*(char*)data) & 0xf;
	if (Z_DEFLATED != cDeflateFlag)
		return 0;

	uint16_t header = _byteswap_ushort(*(uint16_t*)data);
	return (0 == (header % 31));
}

uint32_t CPckClassZlib::compressBound(uint32_t sourceLen)
{
	try {
		return m_PckCompressFunc.compressBound(sourceLen);
	}
	catch (std::bad_function_call ex) {
		Logger.e(ex.what());
	}
	catch (...) {
		Logger_el("Unknown Error.");
	}
	return sourceLen + 12;
}

int	CPckClassZlib::compress(void *dest, ulong_t *destLen, const void *source, uint32_t sourceLen)
{
	try {
		return m_PckCompressFunc.compress(dest, destLen, source, sourceLen, m_compress_level);
	}
	catch (std::bad_function_call ex) {
		Logger.e(ex.what());
	}
	catch (...) {
		Logger_el("Unknown Error.");
	}
	return Z_ERRNO;
}

uint32_t CPckClassZlib::compressBound_zlib(uint32_t sourceLen)
{
	return ::compressBound(sourceLen);
}

int	CPckClassZlib::compress_zlib(void *dest, ulong_t *destLen, const void *source, uint32_t sourceLen, int level)
{
	assert(0 != *destLen);
	int rtnc = compress2((Bytef*)dest, destLen, (Bytef*)source, sourceLen, level);
	assert(0 != *destLen);
	assert(rtnc == Z_OK);
	return (rtnc == Z_OK);
}

uint32_t CPckClassZlib::compressBound_libdeflate(uint32_t sourceLen)
{
	return libdeflate_zlib_compress_bound(NULL, sourceLen);
}

int	CPckClassZlib::compress_libdeflate(void *dest, ulong_t *destLen, const void *source, uint32_t sourceLen, int level)
{
	struct libdeflate_compressor* compressor;
	compressor = libdeflate_alloc_compressor(level);

	assert(0 != *destLen);

	*destLen = libdeflate_zlib_compress(compressor, source, sourceLen, dest, *destLen);
	libdeflate_free_compressor(compressor);

	assert(0 != *destLen);

	if (!(*destLen))
		return 0;

	return 1;
}

int CPckClassZlib::decompress(void *dest, ulong_t  *destLen, const void *source, uint32_t sourceLen)
{
	assert(0 != *destLen);
	int rtnd = uncompress((Bytef*)dest, destLen, (Bytef*)source, sourceLen);
	assert(0 != *destLen);
	//return (0 != *destLen);
	assert(rtnd == Z_OK);
	return (rtnd == Z_OK);
}

//如果只需要解压一部分数据
int CPckClassZlib::decompress_part(void *dest, ulong_t  *destLen, const void *source, uint32_t sourceLen, uint32_t  fullDestLen)
{
	/*
	#define Z_OK            0
	#define Z_STREAM_END    1
	#define Z_NEED_DICT     2
	#define Z_ERRNO        (-1)
	#define Z_STREAM_ERROR (-2)
	#define Z_DATA_ERROR   (-3)
	#define Z_MEM_ERROR    (-4)
	#define Z_BUF_ERROR    (-5)
	#define Z_VERSION_ERROR (-6)
	*/

	unsigned long partlen = *destLen;
	assert(0 != *destLen);
	int rtn = uncompress((Bytef*)dest, destLen, (Bytef*)source, sourceLen);
	assert(0 != *destLen);

	if (Z_OK != rtn && !((Z_BUF_ERROR == rtn) && ((partlen == (*destLen)) || ((*destLen) < fullDestLen)))) {
		char *lpReason;
		switch (rtn) {
		case Z_NEED_DICT:
			lpReason = "需要字典";
			break;
		case Z_STREAM_ERROR:
			lpReason = "流状态错误";
			break;
		case Z_DATA_ERROR:
			lpReason = "数据无效";
			break;
		case Z_MEM_ERROR:
			lpReason = "没有足够的内存";
			break;
		case Z_BUF_ERROR:
			lpReason = "缓冲区空间不足";
			break;
		default:
			lpReason = "其他错误";
		}
		assert(FALSE);
		Logger_el(TEXT_UNCOMPRESSDATA_FAIL_REASON, lpReason);
		return rtn;
	}
	else {
		return Z_OK;
	}
}

//获取数据压缩后的大小，如果源大小小于一定值就不压缩
unsigned long CPckClassZlib::GetCompressBoundSizeByFileSize(ulong_t &dwFileClearTextSize, ulong_t &dwFileCipherTextSize, uint32_t dwFileSize)
{
	if (PCK_BEGINCOMPRESS_SIZE < dwFileSize) {
		dwFileClearTextSize = dwFileSize;
		dwFileCipherTextSize = compressBound(dwFileSize);
	}
	else {
		dwFileCipherTextSize = dwFileClearTextSize = dwFileSize;
	}

	return dwFileCipherTextSize;
}