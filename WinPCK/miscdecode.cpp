//////////////////////////////////////////////////////////////////////
// miscdecode.cpp: WinPCK 界面线程部分
// 对话框代码，解码dds、tga的模块
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4244 )
#pragma warning ( disable : 4311 )
//#pragma warning ( disable : 4018 )

#include "miscdlg.h"

#include <olectl.h>
#include "DDSTextureLoader.h"

//#include <intrin.h>
//#include "SIMD.h"
/*
_inline UINT64	TPicDlg::MakeRgbWithAlphaSSE2(UINT32 *dwBackColor, UINT32 *dwSrc)
{

	__m128i mm2 = _mm_setzero_si128();
	__m128i mm0 = _mm_set_epi32(0, 0, *(dwBackColor + 1), *dwBackColor);
	__m128i mm1 = _mm_set_epi32(0, 0, *(dwSrc + 1), *dwSrc);

	mm0 = _mm_unpacklo_epi8(mm0, mm2);
	mm1 = _mm_unpacklo_epi8(mm1, mm2);
	__m128i mm3;// = mm1;
	mm3 = _mm_set_epi16(0xff, mm1.m128i_i16[7], mm1.m128i_i16[7], mm1.m128i_i16[7], 0xff, mm1.m128i_i16[3], mm1.m128i_i16[3], mm1.m128i_i16[3]);
	__m128i mm4;// = mm0;
	__m128i mm5;// = mm1;
	mm4 = _mm_subs_epu8(mm0, mm1);
	mm5 = _mm_subs_epu8(mm1, mm0);
	mm4 = _mm_mullo_epi16(mm4, mm3);
	mm5 = _mm_mullo_epi16(mm5, mm3);
	mm4 = _mm_srli_epi16(mm4, 8);
	mm5 = _mm_srli_epi16(mm5, 8);
	mm0 = _mm_adds_epi16(mm0, mm5);
	mm0 = _mm_subs_epu16(mm0, mm4);
	mm0 = _mm_packus_epi16(mm0, mm2);
	return mm0.m128i_u64[0];

//
//#else
//	//MMX
////  mm0=dst=00|00|00|00|04|a5|a6|ff
////	mm1=src=00|00|00|00|ff|b5|b6|07
//
//	__m64 mm2 = _m_from_int(0), mm0 = _m_from_int(dwBackColor), mm1 = _m_from_int(dwSrc);
//	//mm0 = 00|04|00|a5|00|a6|00|ff
//	mm0 = _m_punpcklbw(mm0, mm2);
//    //mm1 = 00|ff|00|b5|00|b6|00|07 //Overlay:8 bit到16 bit以容纳结果.32bit expand to 64 bit
//	mm1 = _m_punpcklbw(mm1, mm2);
//    //mm3 = 00|ff|00|b5|00|b6|00|07// 因为要用Overlay的Alpha值
//	__m64 mm3 = mm1;
//	mm3 = _m_punpckhwd(mm3, mm3);
//    //mm3 = 00|ff|00|ff|00|ff|00|ff // 双字移动到四字，现在有八个像素的Alpha了!
//	mm3 = _m_punpckhdq(mm3, mm3);
//    //mm4 = 00|04|00|a5|00|a6|00|ff //Source->mm4
//	__m64 mm4 = mm0;
//    //mm5 = 00|ff|00|b5|00|b6|00|07 //Overlay->mm5
//	__m64 mm5 = mm1;
//    //mm4 = 00|00|00|00|00|00|00|f8 //Source-Overlay，饱和减，小于0为0
//    //mm5 = 00|fb|00|10|00|10|00|00 //Overlay-Source，饱和减，小于0为0
//	mm4 = _m_psubusw(mm4, mm1);
//	mm5 = _m_psubusw(mm5, mm0);
//    //mm4 = 00|00|00|00|00|00|f7|08 //Alpha * (Source-Overlay)
//    //mm5 = fa|05|0f|f0|0f|f0|00|00 //Alpha * (Overlay-Source)
//	mm4 = _m_pmullw(mm4, mm3);
//	mm5 = _m_pmullw(mm5, mm3);
//	//mm4 = 00|00|00|00|00|00|00|f7 //除以256，now mm4 get the result，(Source-Overlay)<0 部分
//    //mm5 = 00|fa|00|0f|00|0f|00|00 //除以256，now mm5 get the result，(Overlay-Source)>0 部分
//	mm4 = _m_psrlw(mm4, _m_from_int(8));
//	mm5 = _m_psrlw(mm5, _m_from_int(8));
//    //mm0 = 00|fe|00|b4|00|b5|00|ff //饱和加到原图象:D=Alpha*(O-S)+S，(Source-Overlay)<0 部分
//    //mm0 = 00|fe|00|b4|00|b5|00|08 //饱和加到原图象D=S-Alpha*(S-O)，(Overlay-Source)>0 部分
//	mm0 = _m_paddusw(mm0, mm5);
//	mm0 = _m_psubusw(mm0, mm4);
//    //mm0 = fe|b4|b5|08|fe|b4|b5|08//紧缩到低32bit
//	//movd eax, mm0
//    //emms ///必要的!Empty MMX Status
//	mm0 = _m_packuswb(mm0, mm0);
//	_m_empty();
//	return _m_to_int(mm0);
//	//->0xfeb4b508
//
//#endif
}

_inline UINT32	TPicDlg::MakeRgbWithAlpha(UINT32 dwBackColor, UINT32 dwSrc)
{
//#ifdef _WIN64

	UINT32 a = (dwSrc >> 24) & 0xff;
	switch(a)
	{
	case 255:
		return dwSrc;

	case 0:
		
		return dwBackColor;

	default:

		CMYRGB cRGB;
		
		UINT32 r = (dwSrc >> 16) & 0xff;
		UINT32 g = (dwSrc >> 8) & 0xff;
		UINT32 b = dwSrc & 0xff;

		//因为dwBackColor的r=g=b,所以都用0x000000ff
		UINT32 o = dwBackColor & 0x000000ff;

		if(r > o)
			cRGB.cRGBVal.r = o + ((a * (r - o))>>8);
		else
			cRGB.cRGBVal.r = o - ((a * (o - r))>>8);

		if(g > o)
			cRGB.cRGBVal.g = o + ((a * (g - o))>>8);
		else
			cRGB.cRGBVal.g = o - ((a * (o - g))>>8);

		if(b > o)
			cRGB.cRGBVal.b = o + ((a * (b - o))>>8);
		else
			cRGB.cRGBVal.b = o - ((a * (o - b))>>8);

		return cRGB.dwargb;
	}
//#else
//__asm {
//	////////////////////////////////////////////
//	/	dwBackColor:dst
//	/	lpcRGB:src
//	/	dst = src * alpha/255 + dst * (255-alpha)/255
//	/	mm2=0
//	/	mm0=dst=a0|a1|a2|a3|a4|a5|a6|a7=00|00|00|00|a4|a5|a6|a7
//	/	mm1=src=b0|b1|b2|b3|b4|b5|b6|b7=00|00|00|00|b4|b5|b6|b7
//	////////////////////////////////////////////
//    pxor mm2, mm2 //把MM2清0
//    lea edx, dword ptr [dwBackColor] //取32bit像素地址到edx
//    movd mm0, dword ptr [edx] //把Source像素取到mm0低32bit
//    //mov edx, dword ptr [lpcRGB]
//    lea edx, dword ptr [dwSrc]
//    movd mm1, dword ptr [edx] //把32 bit Overlay像素取到mm1低32bit
//	//mm0=00|a4|00|a5|00|a6|00|a7
//    punpcklbw mm0, mm2 //Source:8 bit到16 bit以容纳结果，32bit expand to 64 bit
//	//mm1=00|b4|00|b5|00|b6|00|b7
//    punpcklbw mm1, mm2 //Overlay:8 bit到16 bit以容纳结果.32bit expand to 64 bit
//	//mm3=00|b4|00|b5|00|b6|00|b7
//    movq mm3, mm1 // 因为要用Overlay的Alpha值
//	//mm3=00|b4|00|b4|00|b5|00|b5
//    punpckhwd mm3, mm3 // 高字移动到双字
//	//mm3=00|b4|00|b4|00|b4|00|b4
//    punpckhdq mm3, mm3 // 双字移动到四字，现在有八个像素的Alpha了!
//	//mm4=00|a4|00|a5|00|a6|00|a7
//	//mm5=00|b4|00|b5|00|b6|00|b7
//    movq mm4, mm0 //Source->mm4
//    movq mm5, mm1 //Overlay->mm5
//    psubusw mm4, mm1 //Source-Overlay，饱和减，小于0为0
//    psubusw mm5, mm0 //Overlay-Source，饱和减，小于0为0
//    pmullw mm4, mm3 //Alpha * (Source-Overlay)
//    pmullw mm5, mm3 //Alpha * (Overlay-Source)
//    psrlw mm4, 8 //除以256，now mm4 get the result，(Source-Overlay)<0 部分
//    psrlw mm5, 8 //除以256，now mm5 get the result，(Overlay-Source)>0 部分
//    paddusw mm0, mm5 //饱和加到原图象:D=Alpha*(O-S)+S，(Source-Overlay)<0 部分
//    psubusw mm0, mm4 //饱和加到原图象D=S-Alpha*(S-O)，(Overlay-Source)>0 部分
//    packuswb mm0, mm0 //紧缩到低32bit
//	movd eax, mm0
//    emms ///必要的!Empty MMX Status
//}
//
//#endif
}

*/
//
//UINT32	TPicDlg::MakeRgb(UINT32 dwBackColor, UINT32 dwSource,  
//								UINT32 dwABitMask, UINT32 dwRBitMask, UINT32 dwGBitMask, UINT32 dwBBitMask, 
//								BYTE abit, BYTE rbit, BYTE gbit, BYTE bbit, 
//								BYTE ashift, BYTE rshift, BYTE gshift, BYTE bshift,
//								BOOL hasAlpha)
//{
//	//BYTE ashift, rshift, gshift, bshift;
//	BYTE afix, rfix, gfix, bfix;
//	//BYTE abit, rbit, gbit, bbit;
//	BOOL bLeftMove;
//
//	if(10 == bbit)
//	{
//		//D3DFMT_A2R10G10B10
//		bfix = bbit - 8;
//		gfix = gbit - 8;
//		rfix = rbit - 8;
//		if(hasAlpha)
//			afix = 8 - abit;
//
//		bLeftMove = FALSE;
//	}else{
//		bfix = 8 - bbit;
//		gfix = 8 - gbit;
//		rfix = 8 - rbit;
//		if(hasAlpha)
//			afix = 8 - abit;
//
//		bLeftMove = TRUE;
//	}
//
//	UINT32 r = dwSource & dwRBitMask;
//	UINT32 g = dwSource & dwGBitMask;
//	UINT32 b = dwSource & dwBBitMask;
//	UINT32 a = dwSource & dwABitMask;
//
//	if(hasAlpha)
//		a>>=ashift;
//	r>>=rshift;
//	g>>=gshift;
//	b>>=bshift;
//
//	if(bLeftMove)
//	{
//		if(hasAlpha)
//			a<<=afix;
//		r<<=rfix;
//		g<<=gfix;
//		b<<=bfix;
//
//	}else{
//		if(hasAlpha)
//			a<<=afix;
//		r>>=rfix;
//		g>>=gfix;
//		b>>=bfix;
//	}
//	return (a<<24L)+(r<<16L)+(g<<8L)+(b);
//}

//_inline VOID TPicDlg::CalcRgbs(CMYRGB cRGB[])
//{
//	if( cRGB[0].dwargb > cRGB[1].dwargb )
//	{
//		cRGB[2].cRGBVal.r = ( (cRGB[0].cRGBVal.r<<1) + cRGB[1].cRGBVal.r + 1 ) / 3;
//		cRGB[2].cRGBVal.g = ( (cRGB[0].cRGBVal.g<<1) + cRGB[1].cRGBVal.g + 1 ) / 3;
//		cRGB[2].cRGBVal.b = ( (cRGB[0].cRGBVal.b<<1) + cRGB[1].cRGBVal.b + 1 ) / 3;
//		cRGB[2].cRGBVal.a = 0xff;
//
//		cRGB[3].cRGBVal.r = ( cRGB[0].cRGBVal.r + (cRGB[1].cRGBVal.r<<1) + 1 ) / 3;
//		cRGB[3].cRGBVal.g = ( cRGB[0].cRGBVal.g + (cRGB[1].cRGBVal.g<<1) + 1 ) / 3;
//		cRGB[3].cRGBVal.b = ( cRGB[0].cRGBVal.b + (cRGB[1].cRGBVal.b<<1) + 1 ) / 3;
//		cRGB[3].cRGBVal.a = 0xff;
//	}
//	else
//	{
//		cRGB[2].cRGBVal.r = ( cRGB[0].cRGBVal.r + cRGB[1].cRGBVal.r ) >>1;
//		cRGB[2].cRGBVal.g = ( cRGB[0].cRGBVal.g + cRGB[1].cRGBVal.g ) >>1;
//		cRGB[2].cRGBVal.b = ( cRGB[0].cRGBVal.b + cRGB[1].cRGBVal.b ) >>1;
//		cRGB[2].cRGBVal.a = 0xff;
//
//		cRGB[3].cRGBVal.r = ( cRGB[0].cRGBVal.r + (cRGB[1].cRGBVal.r<<1) + 1 ) / 3;
//		cRGB[3].cRGBVal.g = ( cRGB[0].cRGBVal.g + (cRGB[1].cRGBVal.g<<1) + 1 ) / 3;
//		cRGB[3].cRGBVal.b = ( cRGB[0].cRGBVal.b + (cRGB[1].cRGBVal.b<<1) + 1 ) / 3;
//		//cRGB[3].dwargb = 0;
//		cRGB[3].cRGBVal.a = 0x0;
//	}
//
//	cRGB[0].cRGBVal.a = 0xff;
//	cRGB[1].cRGBVal.a = 0xff;
//
//}
//
//
//BOOL TPicDlg::FormatBmpData(UINT16 wWidth, UINT16 wHeight, UINT32 &dwPixels)
//{
//
//	dwPixels = wWidth * wHeight;
//
//	dwDdsToBmpImageDataSize = dwPixels * 4;
//
//	//UINT32	dwBmpSize = dwDdsToBmpFileSize = sizeof(BMPHEAD) + dwDdsToBmpImageDataSize;
//
//	if(NULL == (cleanimage = (char*)malloc(dwDdsToBmpImageDataSize)))
//		return FALSE;
//
//	//BYTE	*buffer = (BYTE*)cleanimage;
//
//	//LPBMPHEAD lpBmpHead = (LPBMPHEAD)buffer;
//
//	picWidth = wWidth;
//	picHeight = wHeight;
//
//	//lpBmpHead->filehead.bfType = 0x4d42;
//	//lpBmpHead->filehead.bfSize = dwBmpSize;
//	////lpBmpHead->filehead.bfReserved1 = lpBmpHead->filehead.bfReserved2 = 0;
//	//lpBmpHead->filehead.bfOffBits = sizeof(BMPHEAD);
//	//lpBmpHead->DIBhead.bV4Size = sizeof(lpBmpHead->DIBhead);
//
//	//lpBmpHead->DIBhead.bV4Width = picWidth;
//	//lpBmpHead->DIBhead.bV4Height = picHeight;
//	//lpBmpHead->DIBhead.bV4Planes = 1;
//	//lpBmpHead->DIBhead.bV4BitCount = 32;
//	//lpBmpHead->DIBhead.bV4V4Compression = BI_RGB;
//	//lpBmpHead->DIBhead.bV4SizeImage = dwBmpSize - sizeof(BMPHEAD);
//	//lpBmpHead->DIBhead.bV4XPelsPerMeter = 0xec4;
//	//lpBmpHead->DIBhead.bV4YPelsPerMeter = 0xec4;
//
//	//lpBmpHead->DIBhead.bV4RedMask = 0x0000ff00;
//	//lpBmpHead->DIBhead.bV4GreenMask = 0x00ff0000;
//	//lpBmpHead->DIBhead.bV4BlueMask = 0xff000000;
//	//lpBmpHead->DIBhead.bV4AlphaMask = 0x000000ff;
//
//	//lpBmpHead->DIBhead.bV4CSType = LCS_sRGB;
//
//
//	return TRUE;
//}
//

BOOL TPicDlg::DetectDIBFormatAndAllocByFormat(UINT16 wWidth, UINT16 wHeight, ...)
{

	UINT32 dwPixels = wWidth * wHeight;

	picWidth = wWidth;
	picHeight = wHeight;

	

	va_list	ap;
	va_start(ap, wHeight);

	if(FMT_DDS == iFormat){

		D3DFORMAT fmt = va_arg( ap, D3DFORMAT );

		switch(fmt)
		{
		case D3DFMT_DXT1:
		case D3DFMT_DXT3:
		case D3DFMT_DXT5:
		case D3DFMT_A8R8G8B8:
		case D3DFMT_A8B8G8R8:
		case D3DFMT_A2R10G10B10:
		case D3DFMT_A2B10G10R10:
		case D3DFMT_A4R4G4B4:
		case D3DFMT_A8R3G3B2:

			decodedDIBFormat = PixelFormat32bppARGB;
			//bytesPerLine = picWidth << 2;
			break;

		case D3DFMT_X8R8G8B8:
		case D3DFMT_X8B8G8R8:
		case D3DFMT_X4R4G4B4:

			decodedDIBFormat = PixelFormat32bppRGB;
			//bytesPerLine = picWidth << 2;
			break;

		case D3DFMT_R8G8B8:

			decodedDIBFormat = PixelFormat24bppRGB;
			//bytesPerLine = picWidth * 3;	//dds文件一定是4字节对齐的
			break;

		case D3DFMT_A1R5G5B5:

			decodedDIBFormat = PixelFormat16bppARGB1555;
			//bytesPerLine = picWidth <<1;	//dds文件一定是4字节对齐的
			break;

		case D3DFMT_X1R5G5B5:

			decodedDIBFormat = PixelFormat16bppRGB555;
			//bytesPerLine = picWidth <<1;	//dds文件一定是4字节对齐的
			break;

		case D3DFMT_R5G6B5:

			decodedDIBFormat = PixelFormat16bppRGB565;
			//bytesPerLine = picWidth <<1;	//dds文件一定是4字节对齐的
			break;

		default:
			decodedDIBFormat = PixelFormat32bppARGB;
			//bytesPerLine = picWidth << 2;
			break;
		}
		//stride = bytesPerLine;

	}else{
		int fmt = va_arg( ap, int );
		switch(fmt)
		{
		case FMTTGA_A1R5G5B5:
			decodedDIBFormat = PixelFormat16bppRGB555;
			//bytesPerLine = picWidth <<1;	//dds文件一定是4字节对齐的
			break;

		case FMTTGA_R8G8B8:
			decodedDIBFormat = PixelFormat24bppRGB;
			//bytesPerLine = picWidth * 3;	//dds文件一定是4字节对齐的
			break;

		case FMTTGA_A8R8G8B8:
		default:
			decodedDIBFormat = PixelFormat32bppARGB;
			//bytesPerLine = picWidth << 2;
			break;
		}
		//stride = (bytesPerLine & 0x3) ? (bytesPerLine & 0xfffffffc) + 4 : bytesPerLine;

	}
	va_end(ap);

	switch(decodedDIBFormat)
	{
	case PixelFormat8bppIndexed:
		bytesPerPixel = 1;
		bytesPerLine = picWidth;
		break;
	case PixelFormat16bppRGB555:
	case PixelFormat16bppRGB565:
	case PixelFormat16bppARGB1555:
		bytesPerPixel = 2;
		bytesPerLine = picWidth <<1;
		break;
	case PixelFormat24bppRGB:
		bytesPerPixel = 3;
		bytesPerLine = picWidth * 3;
		break;
	case PixelFormat32bppRGB:
	case PixelFormat32bppARGB:
	case PixelFormat32bppPARGB:
	case PixelFormat32bppCMYK:
		bytesPerPixel = 4;
		bytesPerLine = picWidth << 2;
		break;
	case PixelFormat48bppRGB:
		bytesPerPixel = 6;
		bytesPerLine = picWidth * 6;
		break;
	case PixelFormat64bppARGB:
	case PixelFormat64bppPARGB:
		bytesPerPixel = 8;
		bytesPerLine = picWidth << 3;
		break;

	default:
		bytesPerPixel = 4;
		bytesPerLine = picWidth << 2;

	}

	stride = (bytesPerLine & 0x3) ? (bytesPerLine & 0xfffffffc) + 4 : bytesPerLine;

	dwDdsToBmpImageDataSize = stride * wHeight ;//dwPixels * bytesPerPixel;

	if(NULL == (cleanimage = (char*)malloc(dwDdsToBmpImageDataSize)))
		return FALSE;

	return TRUE;

}

//void TPicDlg::MakeRgbWithAlphaAll(BOOL hasAlpha)
//{
//
//	UINT32	*lpdwBuffer;
//	UINT32	*lpdwSrc;
//
//	BYTE *dst = (BYTE*)hG + sizeof(BMPHEAD);
//	BYTE *src = (BYTE*)cleanimage + sizeof(BMPHEAD);
//
//	//画背景色格子，0x00ffffff,0x00cccccc
//	UINT32	dwBlockX, dwBlockY, dwLastBlockY;
//	dwBlockX = picWidth>>3;
//	dwBlockY = picHeight>>3;
//	dwLastBlockY = picHeight & 7;
//
//	UINT32	*lpdwLine = (UINT32*) malloc ((dwBlockX + 2)<<5);
//	
//	lpdwBuffer = lpdwLine;
//
//	for(UINT32 i=0;i<dwBlockX + 2;++i)
//	{
//
//		if(i & 1)
//		{
//			for(int j=0;j<8;++j)
//				*lpdwBuffer++ = 0x00ffffff;
//		}
//		else
//		{
//			for(int j=0;j<8;++j)
//				*lpdwBuffer++ = 0x00cccccc;
//		}
//
//	}
//
//	lpdwBuffer = (UINT32*)(dst);
//
//	for(UINT32 i=0;i<=dwBlockY;++i)
//	{
//		int nPixs = dwBlockY == i ? dwLastBlockY : 8;
//		if(i & 1)
//		{
//			for(int j=0;j<nPixs;++j)
//			{
//				memcpy(lpdwBuffer, lpdwLine, picWidth<<2);
//				lpdwBuffer += picWidth;
//			}
//		}else{
//			for(int j=0;j<nPixs;++j)
//			{
//				memcpy(lpdwBuffer, lpdwLine + 8, picWidth<<2);
//				lpdwBuffer += picWidth;
//			}
//		}
//	}
//
//	free(lpdwLine);
//
//	lpdwBuffer = (UINT32*)(dst);
//	lpdwSrc =  (UINT32*)(src);
//	
//	SIMD thisCPU;
//	if(thisCPU.HasSSE2()){
//
//		UINT dwPixels = picHeight * picWidth / 2;
//		for(int i = 0;i<dwPixels;++i){
//				*(UINT64*)lpdwBuffer = MakeRgbWithAlphaSSE2(lpdwBuffer, lpdwSrc);
//				lpdwBuffer += 2;
//				lpdwSrc += 2;
//		}
//	}else{
//
//		for(int i = 0;i<picHeight;++i){
//			for(int j = 0;j<picWidth;++j){
//				//cMyRGB.dwargb = *lpdwSrc;
//				*lpdwBuffer = MakeRgbWithAlpha((*lpdwBuffer), *lpdwSrc);
//				++lpdwBuffer;
//				++lpdwSrc;
//			}
//		}
//	}
//
//}

BOOL TPicDlg::CreateBmpBufferByDds()
{

	if(DDS_MAGIC != *((UINT32*)*buf))
		return FALSE;

	DDS_HEADER *pHeader = (DDS_HEADER*) (((BYTE*)*buf) + 4);

    if( pHeader->dwSize != sizeof(DDS_HEADER) || pHeader->ddspf.dwSize != sizeof(DDS_PIXELFORMAT) )
        return FALSE;

    // Check for DX10 extension
    bool bDXT10Header = false;
    if ( (pHeader->ddspf.dwFlags & DDS_FOURCC) && (MAKEFOURCC( 'D', 'X', '1', '0' ) == pHeader->ddspf.dwFourCC) )
    {
        // Must be long enough for both headers and magic value
		//sizeof(DDS_HEADER_DXT10) = 0x14;
        if( dwSize < (sizeof(DDS_HEADER) + sizeof(UINT32) + 0x14) )
            return FALSE;

        bDXT10Header = true;
    }

    // setup the pointers in the process request
    DDS_HEADER	*ppHeader = pHeader;
    INT offset = sizeof( UINT32 ) + sizeof( DDS_HEADER )
                 + (bDXT10Header ? 0x14/*sizeof( DDS_HEADER_DXT10 )*/ : 0);
    BYTE	*ppBitData = (BYTE*)*buf + offset;

	//UINT iMipCount = pHeader->dwMipMapCount;
	//if( 0 == iMipCount )
	//	iMipCount = 1;
	
	// For now only support 2D textures, not cubemaps or volumes
	if (pHeader->dwCubemapFlags != 0 || (pHeader->dwHeaderFlags & DDS_HEADER_FLAGS_VOLUME) )
		return FALSE;


	D3DFORMAT fmt = GetD3D9Format( pHeader->ddspf );
	//UINT32	dwPixels;

	//Get hG
	//if(!FormatBmpData(pHeader->dwWidth, pHeader->dwHeight, dwPixels))
	//	return FALSE;

	if(!DetectDIBFormatAndAllocByFormat(pHeader->dwWidth, pHeader->dwHeight, fmt))
		return FALSE;

	//UINT32	*lpdwBuffer = (UINT32*)((BYTE*)cleanimage + sizeof(BMPHEAD));

	///*BOOL	*/hasAlpha = FALSE;
	//BYTE	*lpDdsData = ppBitData;

	switch(fmt)
	{
	case D3DFMT_DXT1:

		memcpy(szDdsFormat, "DXT1\0\0\0\0", 8);//strcpy_s(szDdsFormat, 32, "DXT1\0\0\0\0");
		//hasAlpha = TRUE;
		decode_dds_dxt1(ppBitData);

		break;
	case D3DFMT_DXT3:

		memcpy(szDdsFormat, "DXT3\0\0\0\0", 8);	//strcpy_s(szDdsFormat, 32, "DXT3");
		//hasAlpha = TRUE;
		decode_dds_dxt3(ppBitData);

		break;
	case D3DFMT_DXT5:

		memcpy(szDdsFormat, "DXT5\0\0\0\0", 8);	//strcpy_s(szDdsFormat, 32, "DXT5");
		//hasAlpha = TRUE;
		decode_dds_dxt5(ppBitData);

		break;
	case D3DFMT_A8R8G8B8:
		//hasAlpha = TRUE;
		memcpy(szDdsFormat, "A8R8G8B8\0\0\0\0", 12);	//strcpy_s(szDdsFormat, 32, "A8R8G8B8");
		decode_dds_a8r8g8b8(ppBitData);

		break;
	case D3DFMT_X8R8G8B8:

		//hasAlpha = FALSE;
		memcpy(szDdsFormat, "X8R8G8B8\0\0\0\0", 12);	//strcpy_s(szDdsFormat, 32, "X8R8G8B8");
		decode_dds_x8r8g8b8(ppBitData);
		break;

	case D3DFMT_A8B8G8R8:

		//hasAlpha = TRUE;
		memcpy(szDdsFormat, "A8B8G8R8\0\0\0\0", 12);	//strcpy_s(szDdsFormat, 32, "A8B8G8R8");
		decode_dds_a8b8g8r8(ppBitData);
		break;

	case D3DFMT_X8B8G8R8:

		memcpy(szDdsFormat, "X8B8G8R8\0\0\0\0", 12);	//strcpy_s(szDdsFormat, 32, "X8B8G8R8");
		//hasAlpha = FALSE;
		decode_dds_x8b8g8r8(ppBitData);
		break;

	case D3DFMT_R8G8B8:

		memcpy(szDdsFormat, "R8G8B8\0\0", 8);	//strcpy_s(szDdsFormat, 32, "R8G8B8");
		decode_dds_r8g8b8(ppBitData);
		break;

	case D3DFMT_A2R10G10B10:

		//hasAlpha = TRUE;
		memcpy(szDdsFormat, "A2R10G10B10\0", 12);	//strcpy_s(szDdsFormat, 32, "A2R10G10B10");
		decode_dds_a2r10g10b10(ppBitData);

		//UINT32	*lpwDdsBuffer = (UINT32*)lpDdsData;

		////for(int j = picHeight - 1;j>=0;--j)
		//for(int j = 0;j<picHeight;++j)
		//{
		//	UINT32 *lpdwBufferPtr = lpdwBuffer + picWidth * j;
		//	for(UINT32 i = 0;i<picWidth;++i)
		//	{
		//		*lpdwBufferPtr = MakeRgb(*lpdwBuffer, *lpwDdsBuffer, 
		//								pHeader->ddspf.dwABitMask, pHeader->ddspf.dwRBitMask, pHeader->ddspf.dwGBitMask, pHeader->ddspf.dwBBitMask,
		//								2, 10, 10, 10, 
		//								30, 20, 10, 0,
		//								TRUE);
		//		++lpdwBufferPtr;
		//		++lpwDdsBuffer;
		//	}
		//}

		break;
	case D3DFMT_A2B10G10R10:

		//hasAlpha = TRUE;
		memcpy(szDdsFormat, "A2B10G10R10\0", 12);	//strcpy_s(szDdsFormat, 32, "A2B10G10R10");
		decode_dds_a2b10g10r10(ppBitData);
		//UINT32	*lpwDdsBuffer = (UINT32*)lpDdsData;

		////for(int j = picHeight - 1;j>=0;--j)
		//for(int j = 0;j<picHeight;++j)
		//{
		//	UINT32 *lpdwBufferPtr = lpdwBuffer + picWidth * j;
		//	for(DWORD i = 0;i<picWidth;++i)
		//	{
		//		*lpdwBufferPtr = MakeRgb(*lpdwBuffer, *lpwDdsBuffer, 
		//								pHeader->ddspf.dwABitMask, pHeader->ddspf.dwRBitMask, pHeader->ddspf.dwGBitMask, pHeader->ddspf.dwBBitMask, 
		//								2, 10, 10, 10, 
		//								30, 0, 10, 20,
		//								hasAlpha);
		//		++lpdwBufferPtr;
		//		++lpwDdsBuffer;
		//	}
		//}

		break;
	case D3DFMT_A1R5G5B5:

		memcpy(szDdsFormat, "A1R5G5B5\0\0\0\0", 12);	//strcpy_s(szDdsFormat, 32, "A1R5G5B5");
		//hasAlpha = TRUE;
		decode_dds_a1r5g5b5(ppBitData);

		break;
	case D3DFMT_X1R5G5B5:

		//hasAlpha = FALSE;
		memcpy(szDdsFormat, "X1R5G5B5\0\0\0\0", 12);	//strcpy_s(szDdsFormat, 32, "X1R5G5B5");
		decode_dds_x1r5g5b5(ppBitData);
		break;

	case D3DFMT_R5G6B5:

		//hasAlpha = FALSE;
		memcpy(szDdsFormat, "R5G6B5\0\0", 8);	//strcpy_s(szDdsFormat, 32, "R5G6B5");
		decode_dds_r5g6b5(ppBitData);

		break;
	case D3DFMT_A4R4G4B4:

		memcpy(szDdsFormat, "A4R4G4B4\0\0\0\0", 12);	//strcpy_s(szDdsFormat, 32, "A4R4G4B4");
		//hasAlpha = TRUE;
		decode_dds_a4r4g4b4(ppBitData);
		break;

	case D3DFMT_X4R4G4B4:

		//hasAlpha = FALSE;
		memcpy(szDdsFormat, "X4R4G4B4\0\0\0\0", 12);	//strcpy_s(szDdsFormat, 32, "X4R4G4B4");
		decode_dds_x4r4g4b4(ppBitData);
		break;

	case D3DFMT_A8R3G3B2:

		//hasAlpha = TRUE;
		memcpy(szDdsFormat, "A8R3G3B2\0\0\0\0", 12);	//strcpy_s(szDdsFormat, 32, "A8R3G3B2");
		decode_dds_a8r3g3b2(ppBitData);
			//UINT16	*lpwDdsBuffer = (UINT16*)lpDdsData;

			//for(int j = picHeight - 1;j>=0;--j)
			//for(int j = 0;j<picHeight;++j)
			//{
			//	UINT32 *lpdwBufferPtr = lpdwBuffer + picWidth * j;
			//	for(UINT32 i = 0;i<picWidth;++i)
			//	{
			//		*lpdwBufferPtr = MakeRgb(*lpdwBuffer, *lpwDdsBuffer,
			//								pHeader->ddspf.dwABitMask, pHeader->ddspf.dwRBitMask, pHeader->ddspf.dwGBitMask, pHeader->ddspf.dwBBitMask, 
			//								8, 3, 3, 2, 
			//								8, 5, 2, 0,
			//								hasAlpha);
			//		++lpdwBufferPtr;
			//		++lpwDdsBuffer;
			//	}
			//}
		break;

	default:
		MessageBoxA(TEXT_DDS_NOT_SUPPORT, NULL, MB_OK | MB_ICONERROR);
		return FALSE;
	}

	return TRUE;

}

BOOL TPicDlg::CreateBmpBufferByTga()
{
	LPTGAHEAD pHeader = (LPTGAHEAD) *buf;

	int fmt = pHeader->byteTgaType;
	int	nTgaBitsPerPixel, nTgaBitsOfIndexToColorMappedData;
	int nTgaAttrBits = pHeader->byteAttrBitsPerPixel;

	char	*bufferOfColorMappedData = NULL;//, dwColorTableCount;

	UINT32	dwPixels = pHeader->wWidth * pHeader->wHeight;

	//GetTgaBitsPerPixel(fmt);

	switch(fmt)
	{
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
		MessageBoxA(TEXT_DDS_NOT_SUPPORT, NULL, MB_OK | MB_ICONERROR);
		return FALSE;
	}

	if(!DetectDIBFormatAndAllocByFormat(pHeader->wWidth, pHeader->wHeight, nTgaBitsPerPixel))
		return FALSE;

	UINT32	*lpdwBuffer = (UINT32*)cleanimage;
	BYTE	*lpTgaData = (BYTE*)(*buf) + sizeof(TGAHEAD) + pHeader->byteTgaInfoSize;

	//读取颜色表
	if(!makeTgaColorMappedData(lpTgaData, fmt, (char*&)bufferOfColorMappedData, pHeader->wColorTableSize))
	{
		return FALSE;
	}
	

	//开始写bmp数据
	switch(fmt)
	{
	case FMTTGA_RAWTBL:

		switch(nTgaBitsPerPixel)
		{
		case FMTTGA_A1R5G5B5:
			memcpy(szDdsFormat, "RAWTBL_A1R5G5B5\0", 16);	//strcpy_s(szDdsFormat, 32, "RAWTBL_A1R5G5B5");
			break;
		case FMTTGA_R8G8B8:
			memcpy(szDdsFormat, "RAWTBL_R8G8B8\0\0\0", 16);	//strcpy_s(szDdsFormat, 32, "RAWTBL_R8G8B8");
			break;
		case FMTTGA_A8R8G8B8:
			memcpy(szDdsFormat, "RAWTBL_A8R8G8B8\0", 16);	//strcpy_s(szDdsFormat, 32, "RAWTBL_A8R8G8B8");
			break;
		}
		switch(nTgaBitsOfIndexToColorMappedData)
		{
		case 8:

			decode_tga_ColorMapped8(lpTgaData, bufferOfColorMappedData);
			break;

		case 16:

			decode_tga_ColorMapped16(lpTgaData, bufferOfColorMappedData);
			break;

		}
		//switch(nTgaBitsOfIndexToColorMappedData)
		//{
		//case 8:
		//	{
		//		BYTE *lpRgbdata = lpTgaData;
		//		
		//		for(UINT32 i = 0;i<dwPixels;++i)
		//		{
		//			*lpdwBuffer = *(bufferOfColorMappedData + *lpRgbdata);
		//			++lpdwBuffer;
		//			++lpRgbdata;
		//		}
		//	}
		//	break;

		//case 16:

		//	{
		//		UINT16 *lpRgbdata = (UINT16*)lpTgaData;
		//		
		//		for(UINT32 i = 0;i<dwPixels;++i)
		//		{
		//			*lpdwBuffer = *(bufferOfColorMappedData + *lpRgbdata);
		//			++lpdwBuffer;
		//			++lpRgbdata;
		//		}
		//	}

		//	break;
		//}
		break;

	case FMTTGA_RAWRGB:
		
		switch(nTgaBitsPerPixel)
		{
		case FMTTGA_A1R5G5B5:

			memcpy(szDdsFormat, "RAWRGB_A1R5G5B5\0", 16);	//strcpy_s(szDdsFormat, 32, "RAWRGB_A1R5G5B5");
			break;

		case FMTTGA_R8G8B8:

			memcpy(szDdsFormat, "RAWRGB_R8G8B8\0\0\0", 16);	//strcpy_s(szDdsFormat, 32, "RAWRGB_R8G8B8");
			break;

		case FMTTGA_A8R8G8B8:
			
			memcpy(szDdsFormat, "RAWRGB_A8R8G8B8\0", 16);	//strcpy_s(szDdsFormat, 32, "RAWRGB_A8R8G8B8");
			break;
		}

		decode_tga_RGB(lpTgaData);

		break;

	case FMTTGA_RLETBL:
		{

			switch(nTgaBitsPerPixel)
			{
			case FMTTGA_A1R5G5B5:
				memcpy(szDdsFormat, "RLETBL_A1R5G5B5\0", 16);	//strcpy_s(szDdsFormat, 32, "RLETBL_A1R5G5B5");
				break;
			case FMTTGA_R8G8B8:
				memcpy(szDdsFormat, "RLETBL_R8G8B8\0\0\0", 16);	//strcpy_s(szDdsFormat, 32, "RLETBL_R8G8B8");
				break;
			case FMTTGA_A8R8G8B8:
				memcpy(szDdsFormat, "RLETBL_A8R8G8B8\0", 16);	//strcpy_s(szDdsFormat, 32, "RLETBL_A8R8G8B8");
				break;
			}

			switch(nTgaBitsOfIndexToColorMappedData)
			{
			case 8:

				decode_tga_ColorMapped8REL(lpTgaData, bufferOfColorMappedData);
				break;

			case 16:

				decode_tga_ColorMapped16REL(lpTgaData, bufferOfColorMappedData);
				break;

			}

			//BYTE	*lpRgbdata = (BYTE*)lpTgaData;
			//BYTE	byteRleDataCount;

			//UINT32 i = 0;
			//while(i < dwPixels)
			//{
			//	//检查是否run-length 数据包
			//	byteRleDataCount = *lpRgbdata & 0x7f;

			//	if( byteRleDataCount != *lpRgbdata)
			//	{
			//		++lpRgbdata;

			//		switch(nTgaBitsOfIndexToColorMappedData)
			//		{
			//		case 8:
			//			for(int j = 0;j<byteRleDataCount;++j, ++i)
			//			{
			//				*lpdwBuffer = *(bufferOfColorMappedData + *lpRgbdata);
			//				++lpdwBuffer;
			//			}
			//			++lpRgbdata;

			//			break;

			//		case 16:
			//			for(int j = 0;j<byteRleDataCount;++j, ++i)
			//			{
			//				*lpdwBuffer = *(bufferOfColorMappedData + *(WORD*)lpRgbdata);
			//				++lpdwBuffer;
			//			}
			//			lpRgbdata+=2;

			//			break;
			//		}

			//	}else{
			//		++lpRgbdata;

			//		switch(nTgaBitsOfIndexToColorMappedData)
			//		{
			//		case 8:
			//			for(int j = 0;j<byteRleDataCount;++j, ++i)
			//			{
			//				*lpdwBuffer = *(bufferOfColorMappedData + *lpRgbdata);
			//				++lpRgbdata;
			//				++lpdwBuffer;
			//			}
			//			break;

			//		case 16:
			//			for(int j = 0;j<byteRleDataCount;++j, ++i)
			//			{
			//				*lpdwBuffer = *(bufferOfColorMappedData + *(WORD*)lpRgbdata);
			//				lpRgbdata+=2;
			//				++lpdwBuffer;
			//			}
			//			break;
			//		}
			//	}

			//}
		}

		break;

	case FMTTGA_RLERGB:

		switch(nTgaBitsPerPixel)
		{
		case FMTTGA_A1R5G5B5:

			memcpy(szDdsFormat, "RLERGB_A1R5G5B5\0", 16);	//strcpy_s(szDdsFormat, 32, "RLERGB_A1R5G5B5");
			break;

		case FMTTGA_R8G8B8:

			memcpy(szDdsFormat, "RLERGB_R8G8B8\0\0\0", 16);	//strcpy_s(szDdsFormat, 32, "RLERGB_R8G8B8");
			break;

		case FMTTGA_A8R8G8B8:

			memcpy(szDdsFormat, "RLERGB_A8R8G8B8\0", 16);	//strcpy_s(szDdsFormat, 32, "RLERGB_A8R8G8B8");
			break;
		}
		decode_tga_RGBREL(lpTgaData);
		break;

	}

	//free(bufferOfColorMappedData);

	return TRUE;
}

