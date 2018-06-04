//////////////////////////////////////////////////////////////////////
// DdsTgaDecoderDetectFormat.cpp: 检测数据类型
// 解码dds、tga的模块
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2018.5.29
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4996 )

#include <Windows.h>
#include "DdsTgaDecoder.h"
#include "DDS.h"


PixelFormat CDdsTgaDecoder::DetectDdsFormatAndAllocByFormat(D3DFORMAT fmt)
{

	switch(fmt) {
	case D3DFMT_DXT1:
	case D3DFMT_DXT3:
	case D3DFMT_DXT5:
	case D3DFMT_A8R8G8B8:
	case D3DFMT_A8B8G8R8:
	case D3DFMT_A2R10G10B10:
	case D3DFMT_A2B10G10R10:
	case D3DFMT_A4R4G4B4:
	case D3DFMT_A8R3G3B2:

		m_decodedDIBFormat = PixelFormat32bppARGB;
		//bytesPerLine = picWidth << 2;
		break;

	case D3DFMT_X8R8G8B8:
	case D3DFMT_X8B8G8R8:
	case D3DFMT_X4R4G4B4:

		m_decodedDIBFormat = PixelFormat32bppRGB;
		//bytesPerLine = picWidth << 2;
		break;

	case D3DFMT_R8G8B8:

		m_decodedDIBFormat = PixelFormat24bppRGB;
		//bytesPerLine = picWidth * 3;	//dds文件一定是4字节对齐的
		break;

	case D3DFMT_A1R5G5B5:

		m_decodedDIBFormat = PixelFormat16bppARGB1555;
		//bytesPerLine = picWidth <<1;	//dds文件一定是4字节对齐的
		break;

	case D3DFMT_X1R5G5B5:

		m_decodedDIBFormat = PixelFormat16bppRGB555;
		//bytesPerLine = picWidth <<1;	//dds文件一定是4字节对齐的
		break;

	case D3DFMT_R5G6B5:

		m_decodedDIBFormat = PixelFormat16bppRGB565;
		//bytesPerLine = picWidth <<1;	//dds文件一定是4字节对齐的
		break;

	default:
		m_decodedDIBFormat = PixelFormat32bppARGB;
		//bytesPerLine = picWidth << 2;
		break;
	}
	return m_decodedDIBFormat;
}

PixelFormat CDdsTgaDecoder::DetectTgaFormatAndAllocByFormat(int fmt)
{
	switch(fmt) {
	case FMTTGA_A1R5G5B5:
		m_decodedDIBFormat = PixelFormat16bppRGB555;
		//bytesPerLine = picWidth <<1;	//dds文件一定是4字节对齐的
		break;

	case FMTTGA_R8G8B8:
		m_decodedDIBFormat = PixelFormat24bppRGB;
		//bytesPerLine = picWidth * 3;	//dds文件一定是4字节对齐的
		break;

	case FMTTGA_A8R8G8B8:
	default:
		m_decodedDIBFormat = PixelFormat32bppARGB;
		//bytesPerLine = picWidth << 2;
		break;
	}
	return m_decodedDIBFormat;
}

BOOL CDdsTgaDecoder::CalaStride_DetectDIBFormat_AllocByFormat(UINT16 wWidth, UINT16 wHeight, PixelFormat pixelFormat)
{

	UINT32 dwPixels = wWidth * wHeight;

	m_picWidth = wWidth;
	m_picHeight = wHeight;

	switch(pixelFormat) {
	case PixelFormat8bppIndexed:
		m_bytesPerPixel = 1;
		m_bytesPerLine = m_picWidth;
		break;
	case PixelFormat16bppRGB555:
	case PixelFormat16bppRGB565:
	case PixelFormat16bppARGB1555:
		m_bytesPerPixel = 2;
		m_bytesPerLine = m_picWidth << 1;
		break;
	case PixelFormat24bppRGB:
		m_bytesPerPixel = 3;
		m_bytesPerLine = m_picWidth * 3;
		break;
	case PixelFormat32bppRGB:
	case PixelFormat32bppARGB:
	case PixelFormat32bppPARGB:
	case PixelFormat32bppCMYK:
		m_bytesPerPixel = 4;
		m_bytesPerLine = m_picWidth << 2;
		break;
	case PixelFormat48bppRGB:
		m_bytesPerPixel = 6;
		m_bytesPerLine = m_picWidth * 6;
		break;
	case PixelFormat64bppARGB:
	case PixelFormat64bppPARGB:
		m_bytesPerPixel = 8;
		m_bytesPerLine = m_picWidth << 3;
		break;

	default:
		m_bytesPerPixel = 4;
		m_bytesPerLine = m_picWidth << 2;

	}

	m_stride = (m_bytesPerLine & 0x3) ? (m_bytesPerLine & (~0x3)) + 4 : m_bytesPerLine;

	m_dwDdsTgaToDIBDataSize = m_stride * wHeight;//dwPixels * bytesPerPixel;

	if(NULL == (m_DecodedDIBData = (char*)malloc(m_dwDdsTgaToDIBDataSize)))
		return FALSE;

	return TRUE;

}

#define define_one_switch_dxt(_name, _sub) \
case D3DFMT_##_name:\
	strcpy(lpszPictureFormat, #_name);\
	decode_dds_dxtN = &CDdsTgaDecoder::decode_dds_##_sub;\
break;

BOOL CDdsTgaDecoder::GetDDSFormatStringAndDecodeEntry(D3DFORMAT fmt, LPSTR lpszPictureFormat, void(CDdsTgaDecoder::* &decode_dds_dxtN)(LPBYTE))
{
	switch(fmt) {

		define_one_switch_dxt(DXT1, dxt1);
		define_one_switch_dxt(DXT3, dxt3);
		define_one_switch_dxt(DXT5, dxt5);
		define_one_switch_dxt(A8R8G8B8, a8r8g8b8);
		define_one_switch_dxt(X8R8G8B8, x8r8g8b8);
		define_one_switch_dxt(A8B8G8R8, a8b8g8r8);
		define_one_switch_dxt(X8B8G8R8, x8b8g8r8);
		define_one_switch_dxt(R8G8B8, r8g8b8);
		define_one_switch_dxt(A2R10G10B10, a2r10g10b10);
		define_one_switch_dxt(A2B10G10R10, a2b10g10r10);
		define_one_switch_dxt(A1R5G5B5, a1r5g5b5);
		define_one_switch_dxt(X1R5G5B5, x1r5g5b5);
		define_one_switch_dxt(R5G6B5, r5g6b5);
		define_one_switch_dxt(A4R4G4B4, a4r4g4b4);
		define_one_switch_dxt(X4R4G4B4, x4r4g4b4);
		define_one_switch_dxt(A8R3G3B2, a8r3g3b2);
	default:
		decode_dds_dxtN = NULL;
		MessageBoxA(NULL, TEXT_DDS_NOT_SUPPORT, NULL, MB_OK | MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}
#undef define_one_switch_dxt