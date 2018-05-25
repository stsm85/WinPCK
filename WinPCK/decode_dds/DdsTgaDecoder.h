#pragma once
#include "DdsTgaDecoderDefine.h"
#include <Gdiplus.h>
using namespace Gdiplus;
#include "dxgiformat.h"

class CDdsTgaDecoder
{
public:
	CDdsTgaDecoder();
	~CDdsTgaDecoder();


	BOOL	Decode(LPBYTE _In_data, UINT32 _in_bufsize, LPBYTE &_out_data, UINT &_out__picWidth, UINT &_out_picHeight, PICFORMAT picFormat, LPSTR _out_szFormatDdsTga);
	BOOL	DecodeDDS(LPBYTE _In_data, UINT32 _in_bufsize, LPBYTE &_out_data, UINT &_out__picWidth, UINT &_out_picHeight, LPSTR _out_szFormatDdsTga);
	BOOL	DecodeTGA(LPBYTE _In_data, UINT32 _in_bufsize, LPBYTE &_out_data, UINT &_out__picWidth, UINT &_out_picHeight, LPSTR _out_szFormatDdsTga);

	//返回stride的值
	UINT	GetStride();
	//返回图像的宽和高
	UINT	GetWidth();
	UINT	GetHeight();
	//返回图像的m_decodedDIBFormat
	PixelFormat GetPixelFormat();

protected:

	//图片的宽和高
	UINT		m_picWidth, m_picHeight;
	//图片类型dds,tga,other
	//enum { FMT_DDS, FMT_TGA, FMT_RAW} m_picFormat;
	//PICFORMAT	m_picFormat;
	//显示的图片的具体格式
	LPSTR		m_lpszPictureFormat;
	//类内部使用的原始的图片文件数据，当类型为RAW时，指向hGlobal的内存
	LPBYTE		m_resBufferInClass;
	//传入的文件的数据的大小
	UINT32		m_bufsize;
	//已经解码的图片的DIB数据（DDS，TGA）
	char*		m_DecodedDIBData;
	//已经解码的图片的DIB数据（DDS，TGA）的大小
	DWORD		m_dwDdsTgaToDIBDataSize;
	//
	PixelFormat	m_decodedDIBFormat;
	//一行像素一共多少个字节
	UINT		m_bytesPerLine;
	//一行像素一共多少个像素点
	UINT		m_bytesPerPixel;
	//图片一行像素的数据在内存对齐后的大小(align=4byte)
	UINT		m_stride;

	//获取到dds,tga的内部DIB的格式，顺便计算一下stride(图片一行像素的数据在内存对齐后的大小),...在dds格式时传入D3DFORMAT，tga格式时传入tga格式（int ,FMTTGA_A1R5G5B5...)
	PixelFormat DetectDdsFormatAndAllocByFormat(D3DFORMAT fmt);
	PixelFormat DetectTgaFormatAndAllocByFormat(int fmt);
	BOOL CalaStride_DetectDIBFormat_AllocByFormat(UINT16 wWidth, UINT16 wHeight, PixelFormat pixelFormat);
	BOOL GetDDSFormatStringAndDecodeEntry(D3DFORMAT fmt, LPSTR lpszPictureFormat, void(CDdsTgaDecoder::* &decode_dds_dxtN)(LPBYTE));
	//从dds,tga的数据创建DIB的数据
	BOOL CreateBmpBufferByDds();
	BOOL CreateBmpBufferByTga();

#pragma region decode_dds_函数
	void	decode_dds_dxt1(BYTE *ddsimage);
	void	decode_dds_dxt3(BYTE *ddsimage);
	void	decode_dds_dxt5(BYTE *ddsimage);
	void	decode_dds_a8r8g8b8(BYTE *ddsimage);
	void	decode_dds_x8r8g8b8(BYTE *ddsimage);
	void	decode_dds_a8b8g8r8(BYTE *ddsimage);
	void	decode_dds_x8b8g8r8(BYTE *ddsimage);
	void	decode_dds_r8g8b8(BYTE *ddsimage);
	void	decode_dds_a1r5g5b5(BYTE *ddsimage);
	void	decode_dds_x1r5g5b5(BYTE *ddsimage);
	void	decode_dds_r5g6b5(BYTE *ddsimage);
	void	decode_dds_a4r4g4b4(BYTE *ddsimage);
	void	decode_dds_x4r4g4b4(BYTE *ddsimage);
	void	decode_dds_a8r3g3b2(BYTE *ddsimage);
	void	decode_dds_a2r10g10b10(BYTE *ddsimage);
	void	decode_dds_a2b10g10r10(BYTE *ddsimage);
#pragma endregion

#pragma region TGA decoder
	BOOL	makeTgaColorMappedData(BYTE * &tgaimage, int fmt, char * &bufferOfColorMappedData, UINT16 wColorTableSize/*, int nTgaBitsPerPixel*/);
	void	decode_tga_ColorMapped8(BYTE *tgaimage, char *lpColorMappedData);
	void	decode_tga_ColorMapped16(BYTE *tgaimage, char *lpColorMappedData);

	void	decode_tga_RGB(BYTE *tgaimage);

	void	decode_tga_ColorMapped8REL(BYTE *tgaimage, char *lpColorMappedData);
	void	decode_tga_ColorMapped16REL(BYTE *tgaimage, char *lpColorMappedData);

	void	decode_tga_RGBREL(BYTE *tgaimage);
#pragma endregion
};

