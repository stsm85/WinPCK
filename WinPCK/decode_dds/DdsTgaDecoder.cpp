//////////////////////////////////////////////////////////////////////
// DdsTgaDecoder.cpp: 将dds、tga的数据解码成DIB数据
// 解码dds、tga的类
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2018.5.29
//////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include "DDSTextureLoader.h"
#include "DdsTgaDecoder.h"


CDdsTgaDecoder::CDdsTgaDecoder(): 
	m_picWidth(0),
	m_picHeight(0),
	m_stride(0),
	m_DecodedDIBData(NULL)
{}

CDdsTgaDecoder::~CDdsTgaDecoder()
{}


UINT CDdsTgaDecoder::GetStride()
{
	return m_stride;
}

//返回图像的宽和高
UINT CDdsTgaDecoder::GetWidth()
{
	return m_picWidth;
}
UINT CDdsTgaDecoder::GetHeight()
{
	return m_picHeight;
}

//返回图像的m_decodedDIBFormat
PixelFormat CDdsTgaDecoder::GetPixelFormat()
{
	return m_decodedDIBFormat;
}

#pragma region 对外开放的解码函数

BOOL CDdsTgaDecoder::DecodeDDS(LPBYTE _In_data, UINT32 _in_bufsize, LPBYTE &_out_data, UINT &_out__picWidth, UINT &_out_picHeight, LPSTR _out_szFormatDdsTga)
{
	m_lpszPictureFormat = _out_szFormatDdsTga;
	m_resBufferInClass = _In_data;
	m_bufsize = _in_bufsize;

	if (CreateBmpBufferByDds()) {
		_out_data = (LPBYTE)m_DecodedDIBData;
		_out__picWidth = m_picWidth;
		_out_picHeight = m_picHeight;
		return TRUE;
	}

	return FALSE;
}

BOOL CDdsTgaDecoder::DecodeTGA(LPBYTE _In_data, UINT32 _in_bufsize, LPBYTE &_out_data, UINT &_out__picWidth, UINT &_out_picHeight, LPSTR _out_szFormatDdsTga)
{
	m_lpszPictureFormat = _out_szFormatDdsTga;
	m_resBufferInClass = _In_data;
	m_bufsize = _in_bufsize;

	if (CreateBmpBufferByTga()) {
		_out_data = (LPBYTE)m_DecodedDIBData;
		_out__picWidth = m_picWidth;
		_out_picHeight = m_picHeight;
		return TRUE;
	}

	return FALSE;
}

BOOL CDdsTgaDecoder::Decode(LPBYTE _In_data, UINT32 _in_bufsize, LPBYTE &_out_data, UINT &_out__picWidth, UINT &_out_picHeight, PICFORMAT picFormat, LPSTR _out_szFormatDdsTga)
{

	switch(picFormat) {
	case FMT_DDS:
		return DecodeDDS(_In_data, _in_bufsize, _out_data, _out__picWidth, _out_picHeight, _out_szFormatDdsTga);
		break;

	case FMT_TGA:
		return DecodeTGA(_In_data, _in_bufsize, _out_data, _out__picWidth, _out_picHeight, _out_szFormatDdsTga);
		break;
	default:

		return FALSE;
	}
	return TRUE;
}
#pragma endregion

BOOL CDdsTgaDecoder::CreateBmpBufferByDds()
{

	if(DDS_MAGIC != *((UINT32*)m_resBufferInClass))
		return FALSE;

	DDS_HEADER *pHeader = (DDS_HEADER*)(((BYTE*)m_resBufferInClass) + 4);

	if(pHeader->dwSize != sizeof(DDS_HEADER) || pHeader->ddspf.dwSize != sizeof(DDS_PIXELFORMAT))
		return FALSE;

	// Check for DX10 extension
	bool bDXT10Header = false;
	if((pHeader->ddspf.dwFlags & DDS_FOURCC) && (MAKEFOURCC('D', 'X', '1', '0') == pHeader->ddspf.dwFourCC)) {
		// Must be long enough for both headers and magic value
		//sizeof(DDS_HEADER_DXT10) = 0x14;
		if(m_bufsize < (sizeof(DDS_HEADER) + sizeof(UINT32) + 0x14))
			return FALSE;

		bDXT10Header = true;
	}

	// setup the pointers in the process request
	DDS_HEADER	*ppHeader = pHeader;
	INT offset = sizeof(UINT32) + sizeof(DDS_HEADER)
		+ (bDXT10Header ? 0x14/*sizeof( DDS_HEADER_DXT10 )*/ : 0);
	BYTE	*ppBitData = (BYTE*)m_resBufferInClass + offset;

	// For now only support 2D textures, not cubemaps or volumes
	//if(pHeader->dwCubemapFlags != 0 || (pHeader->dwHeaderFlags & DDS_HEADER_FLAGS_VOLUME))
	//	return FALSE;

	D3DFORMAT fmt = GetD3D9Format(pHeader->ddspf);

	if(!CalaStride_DetectDIBFormat_AllocByFormat(pHeader->dwWidth, pHeader->dwHeight, DetectDdsFormatAndAllocByFormat(fmt)))
		return FALSE;

	void(CDdsTgaDecoder::*decode_dds_dxtN)(LPBYTE);

	if(!GetDDSFormatStringAndDecodeEntry(fmt, m_lpszPictureFormat, decode_dds_dxtN))
		return FALSE;

	(this->*decode_dds_dxtN)(ppBitData);

	return TRUE;
}

BOOL CDdsTgaDecoder::CreateBmpBufferByTga()
{
	LPTGAHEAD pHeader = (LPTGAHEAD)m_resBufferInClass;

	int fmt = pHeader->byteTgaType;
	int	nTgaBitsPerPixel, nTgaBitsOfIndexToColorMappedData;
	int nTgaAttrBits = pHeader->byteAttrBitsPerPixel;

	char	*bufferOfColorMappedData = NULL;//, dwColorTableCount;

	UINT32	dwPixels = pHeader->wWidth * pHeader->wHeight;

	//GetTgaBitsPerPixel(fmt);

	switch(fmt) {
	case FMTTGA_RAWTBL:
	case FMTTGA_RLETBL:

		nTgaBitsPerPixel = pHeader->byteColorTableBits;
		nTgaBitsOfIndexToColorMappedData = pHeader->byteBitsPerPixel;
		break;

	case FMTTGA_RAWRGB:
	case FMTTGA_RLERGB:

		nTgaBitsPerPixel = pHeader->byteBitsPerPixel;
		nTgaBitsOfIndexToColorMappedData = 0;
		break;

	default:
		MessageBoxA(NULL, TEXT_DDS_NOT_SUPPORT, NULL, MB_OK | MB_ICONERROR);
		return FALSE;
	}

	if(!CalaStride_DetectDIBFormat_AllocByFormat(pHeader->wWidth, pHeader->wHeight, DetectTgaFormatAndAllocByFormat(nTgaBitsPerPixel)))
		return FALSE;

	UINT32	*lpdwBuffer = (UINT32*)m_DecodedDIBData;
	BYTE	*lpTgaData = (BYTE*)m_resBufferInClass + sizeof(TGAHEAD) + pHeader->byteTgaInfoSize;

	//读取颜色表
	if(!makeTgaColorMappedData(lpTgaData, fmt, (char*&)bufferOfColorMappedData, pHeader->wColorTableSize)) {
		return FALSE;
	}

	//开始写bmp数据
	switch(fmt) {
	case FMTTGA_RAWTBL:

		switch(nTgaBitsPerPixel) {
		case FMTTGA_A1R5G5B5:
			memcpy(m_lpszPictureFormat, "RAWTBL_A1R5G5B5\0", 16);	//strcpy_s(m_lpszPictureFormat, 32, "RAWTBL_A1R5G5B5");
			break;
		case FMTTGA_R8G8B8:
			memcpy(m_lpszPictureFormat, "RAWTBL_R8G8B8\0\0\0", 16);	//strcpy_s(m_lpszPictureFormat, 32, "RAWTBL_R8G8B8");
			break;
		case FMTTGA_A8R8G8B8:
			memcpy(m_lpszPictureFormat, "RAWTBL_A8R8G8B8\0", 16);	//strcpy_s(m_lpszPictureFormat, 32, "RAWTBL_A8R8G8B8");
			break;
		}
		switch(nTgaBitsOfIndexToColorMappedData) {
		case 8:
			decode_tga_ColorMapped8(lpTgaData, bufferOfColorMappedData);
			break;
		case 16:
			decode_tga_ColorMapped16(lpTgaData, bufferOfColorMappedData);
			break;
		}
		break;
	case FMTTGA_RAWRGB:
		switch(nTgaBitsPerPixel) {
		case FMTTGA_A1R5G5B5:
			memcpy(m_lpszPictureFormat, "RAWRGB_A1R5G5B5\0", 16);	//strcpy_s(m_lpszPictureFormat, 32, "RAWRGB_A1R5G5B5");
			break;
		case FMTTGA_R8G8B8:
			memcpy(m_lpszPictureFormat, "RAWRGB_R8G8B8\0\0\0", 16);	//strcpy_s(m_lpszPictureFormat, 32, "RAWRGB_R8G8B8");
			break;
		case FMTTGA_A8R8G8B8:
			memcpy(m_lpszPictureFormat, "RAWRGB_A8R8G8B8\0", 16);	//strcpy_s(m_lpszPictureFormat, 32, "RAWRGB_A8R8G8B8");
			break;
		}
		decode_tga_RGB(lpTgaData);
		break;
	case FMTTGA_RLETBL:
	{
		switch(nTgaBitsPerPixel) {
		case FMTTGA_A1R5G5B5:
			memcpy(m_lpszPictureFormat, "RLETBL_A1R5G5B5\0", 16);	//strcpy_s(m_lpszPictureFormat, 32, "RLETBL_A1R5G5B5");
			break;
		case FMTTGA_R8G8B8:
			memcpy(m_lpszPictureFormat, "RLETBL_R8G8B8\0\0\0", 16);	//strcpy_s(m_lpszPictureFormat, 32, "RLETBL_R8G8B8");
			break;
		case FMTTGA_A8R8G8B8:
			memcpy(m_lpszPictureFormat, "RLETBL_A8R8G8B8\0", 16);	//strcpy_s(m_lpszPictureFormat, 32, "RLETBL_A8R8G8B8");
			break;
		}

		switch(nTgaBitsOfIndexToColorMappedData) {
		case 8:
			decode_tga_ColorMapped8REL(lpTgaData, bufferOfColorMappedData);
			break;
		case 16:
			decode_tga_ColorMapped16REL(lpTgaData, bufferOfColorMappedData);
			break;

		}
	}

	break;

	case FMTTGA_RLERGB:

		switch(nTgaBitsPerPixel) {
		case FMTTGA_A1R5G5B5:

			memcpy(m_lpszPictureFormat, "RLERGB_A1R5G5B5\0", 16);	//strcpy_s(m_lpszPictureFormat, 32, "RLERGB_A1R5G5B5");
			break;

		case FMTTGA_R8G8B8:

			memcpy(m_lpszPictureFormat, "RLERGB_R8G8B8\0\0\0", 16);	//strcpy_s(m_lpszPictureFormat, 32, "RLERGB_R8G8B8");
			break;

		case FMTTGA_A8R8G8B8:

			memcpy(m_lpszPictureFormat, "RLERGB_A8R8G8B8\0", 16);	//strcpy_s(m_lpszPictureFormat, 32, "RLERGB_A8R8G8B8");
			break;
		}
		decode_tga_RGBREL(lpTgaData);
		break;

	}
	return TRUE;
}

