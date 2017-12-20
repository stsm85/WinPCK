//////////////////////////////////////////////////////////////////////
// PckClassCompress.cpp: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 文件头、尾等结构体的读取
//
// 此程序由 李秋枫/stsm/liqf 编写，pck结构参考若水的pck结构.txt，并
// 参考了其易语言代码中并于读pck文件列表的部分
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2017.7.3
//////////////////////////////////////////////////////////////////////

#include "PckClass.h"

#include "zlib.h"
#include "libdeflate.h"

#pragma warning ( disable : 4267 )

void CPckClass::init_compressor()
{

	if(Z_BEST_COMPRESSION < lpPckParams->dwCompressLevel){

		m_PckCompressFunc.compressBound = compressBound_libdeflate;
		m_PckCompressFunc.compress = compress_libdeflate;

	}else{

		m_PckCompressFunc.compressBound = compressBound_zlib;
		m_PckCompressFunc.compress = compress_zlib;

	}
}

unsigned long CPckClass::compressBound(unsigned long sourceLen)
{
	return m_PckCompressFunc.compressBound(sourceLen);
}

int	CPckClass::compress(void *dest, unsigned long *destLen, const void *source, unsigned long sourceLen, int level)
{
	return m_PckCompressFunc.compress(dest, destLen, source, sourceLen, level);
}

unsigned long CPckClass::compressBound_zlib(unsigned long sourceLen)
{
	return ::compressBound(sourceLen);
}

int	CPckClass::compress_zlib(void *dest, unsigned long *destLen, const void *source, unsigned long sourceLen, int level)
{
#ifdef _DEBUG
	int rtnc = compress2((Bytef*)dest, destLen, (Bytef*)source, sourceLen, level);
	assert(rtnc == Z_OK);
	return (rtnc == Z_OK);
#else
	return (Z_OK == compress2((Bytef*)dest, destLen, (Bytef*)source, sourceLen, level));
#endif
}

unsigned long CPckClass::compressBound_libdeflate(unsigned long sourceLen)
{
	return libdeflate_zlib_compress_bound(NULL, sourceLen);
}

int	CPckClass::compress_libdeflate(void *dest, unsigned long *destLen, const void *source, unsigned long sourceLen, int level)
{
	struct libdeflate_compressor* compressor;
	compressor = libdeflate_alloc_compressor(level);

	*destLen = libdeflate_zlib_compress(compressor, source, sourceLen, dest, *destLen);

	libdeflate_free_compressor(compressor);

#ifdef _DEBUG
	assert(0 != *destLen);
#endif
	if (!(*destLen))
		return 0;

	return 1;
}

int CPckClass::decompress(void *dest, unsigned long  *destLen, const void *source, unsigned long sourceLen)
{
#ifdef _DEBUG
	int rtnd = uncompress((Bytef*)dest, destLen, (Bytef*)source, sourceLen);
	assert(rtnd == Z_OK);
	return (rtnd == Z_OK);
#else
	return (Z_OK == uncompress((Bytef*)dest, destLen, (Bytef*)source, sourceLen));
#endif
}

//如果只需要解压一部分数据
int CPckClass::decompress_part(void *dest, unsigned long  *destLen, const void *source, unsigned long sourceLen, unsigned long  fullDestLen)
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

	int rtn = uncompress((Bytef*)dest, destLen, (Bytef*)source, sourceLen);
//#ifdef USE_ZLIB
	if (Z_OK != rtn && !((Z_BUF_ERROR == rtn) && ((*destLen) < fullDestLen))) {
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
		PrintLogE(TEXT_UNCOMPRESSDATA_FAIL_REASON, lpReason, __FILE__, __FUNCTION__, __LINE__);
		return 0;
	} else {
		return 1;
	}

//#else
//	if(LIBDEFLATE_SUCCESS)
//		return 1;
//	else
//		return 0;
//
//#endif
}