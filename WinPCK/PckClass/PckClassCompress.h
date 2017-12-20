//////////////////////////////////////////////////////////////////////
// PckClassCompress.h: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 文件头、尾等结构体的读取
//
// 此程序由 李秋枫/stsm/liqf 编写，pck结构参考若水的pck结构.txt，并
// 参考了其易语言代码中并于读pck文件列表的部分
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2017.7.3
//////////////////////////////////////////////////////////////////////
#define Z_BEST_COMPRESSION 9


protected:

struct COMPRESS_FUNC {
	unsigned long (*compressBound)(unsigned long);
	int (*compress)(void *dest, unsigned long *destLen, const void *source, unsigned long sourceLen, int level);
}m_PckCompressFunc;

public:

void	init_compressor();

protected:
unsigned long compressBound(unsigned long sourceLen);
int	compress(void *dest, unsigned long *destLen, const void *source, unsigned long sourceLen, int level);
int decompress(void *dest, unsigned long  *destLen, const void *source, unsigned long sourceLen);
int decompress_part(void *dest, unsigned long  *destLen, const void *source, unsigned long sourceLen, unsigned long  fullDestLen);

static unsigned long compressBound_zlib(unsigned long sourceLen);
static int	compress_zlib(void *dest, unsigned long *destLen, const void *source, unsigned long sourceLen, int level);
static unsigned long compressBound_libdeflate(unsigned long sourceLen);
static int	compress_libdeflate(void *dest, unsigned long *destLen, const void *source, unsigned long sourceLen, int level);