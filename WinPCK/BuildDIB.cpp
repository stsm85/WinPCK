//////////////////////////////////////////////////////////////////////
// BuildDIB.cpp: 构建DIB数据
// 解码dds、tga的模块
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2015.6.12
//////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <Gdiplus.h>
#include "miscpicdlg.h"
#include "miscdlg.h"
#include <intrin.h>

_inline void CalcRgbsDXT1(UINT32 dwRGBIndex[4], UINT16 wBlockColor2[2])
{
	__m128i xmmrmask = _mm_set_epi32(0, 0, 0xfff800, 0xfff800);
	__m128i xmmgmask = _mm_set_epi32(0, 0, 0x07e0, 0x07e0);
	__m128i xmmbmask = _mm_set_epi32(0, 0, 0x001f, 0x001f);

	__m128i xmmres = _mm_set_epi16(	0, 0, 0, 0, \
									0xff, wBlockColor2[1], 0xff, wBlockColor2[0]);

	__m128i xmmr = _mm_and_si128(xmmres, xmmrmask);
	__m128i xmmg = _mm_and_si128(xmmres, xmmgmask);
	__m128i xmmb = _mm_and_si128(xmmres, xmmbmask);

	xmmr = _mm_slli_epi32(xmmr, 8);
	xmmg = _mm_slli_epi32(xmmg, 5);
	xmmb = _mm_slli_epi32(xmmb, 3);

	__m128i xmm2 = _mm_or_si128(_mm_or_si128(xmmr, xmmg), xmmb);
	__m128i xmm1 = _mm_shuffle_epi32(xmm2, _MM_SHUFFLE(3, 2, 0, 1));

	dwRGBIndex[0] = xmm2.m128i_u32[0];
	dwRGBIndex[1] = xmm2.m128i_u32[1];

	__m128i xmm0 = _mm_setzero_si128();

	if( wBlockColor2[0] > wBlockColor2[1] ){
		xmm1 = _mm_slli_epi64(_mm_unpacklo_epi8(xmm1, xmm0), 1);
		xmm2 = _mm_unpacklo_epi8(xmm2, xmm0);
		xmmres = _mm_set_epi32(0x00000001, 0x00010001, 0x00000001, 0x00010001);
		xmm1 = _mm_adds_epi16(_mm_adds_epi16(xmm1, xmm2), xmmres);
		xmmr = _mm_set_epi32(0xaaabaaab, 0xaaabaaab, 0xaaabaaab, 0xaaabaaab);
		xmm1 = _mm_srli_epi16(_mm_mulhi_epu16(xmm1, xmmr), 1);
		xmm1 = _mm_packus_epi16(xmm1, xmm0);
		dwRGBIndex[2] = xmm1.m128i_u32[1];
		dwRGBIndex[3] = xmm1.m128i_u32[0];
	}else{
		xmm1 = _mm_avg_epu8(xmm1, xmm2);
		dwRGBIndex[2] = xmm1.m128i_u32[1];
		dwRGBIndex[3] = 0;
	}
}

_inline void CalcRgbsDXT35(UINT32 dwRGBIndex[4], UINT16 wBlockColor2[2])
{

	__m128i xmmrmask = _mm_set_epi32(0, 0, 0xf800, 0xf800);
	__m128i xmmgmask = _mm_set_epi32(0, 0, 0x07e0, 0x07e0);
	__m128i xmmbmask = _mm_set_epi32(0, 0, 0x001f, 0x001f);

	__m128i xmmres = _mm_set_epi16(	0, 0, 0, 0, \
									0, wBlockColor2[1], 0, wBlockColor2[0]);

	__m128i xmmr = _mm_and_si128(xmmres, xmmrmask);
	__m128i xmmg = _mm_and_si128(xmmres, xmmgmask);
	__m128i xmmb = _mm_and_si128(xmmres, xmmbmask);

	xmmr = _mm_slli_epi32(xmmr, 8);
	xmmg = _mm_slli_epi32(xmmg, 5);
	xmmb = _mm_slli_epi32(xmmb, 3);

	__m128i xmm2 = _mm_or_si128(_mm_or_si128(xmmr, xmmg), xmmb);
	__m128i xmm1 = _mm_shuffle_epi32(xmm2, _MM_SHUFFLE(3, 2, 0, 1));

	dwRGBIndex[0] = xmm2.m128i_u32[0];
	dwRGBIndex[1] = xmm2.m128i_u32[1];

	__m128i xmm0 = _mm_setzero_si128();
	//__m128i xmm1 = _mm_set_epi32(0, 0, cRGB[0].dwargb, cRGB[1].dwargb);
	//__m128i xmm2 = _mm_set_epi32(0, 0, cRGB[1].dwargb, cRGB[0].dwargb);
	__m128i xmm3 = _mm_slli_epi64(_mm_unpacklo_epi8(xmm1, xmm0), 1);
	__m128i xmm4 = _mm_unpacklo_epi8(xmm2, xmm0);
	__m128i xmm5 = _mm_set_epi32(0x00000001, 0x00010001, 0x00000001, 0x00010001);
	xmm3 = _mm_adds_epi16(_mm_adds_epi16(xmm3, xmm4), xmm5);
	xmm5 = _mm_set_epi32(0x0000aaab, 0xaaabaaab, 0x0000aaab, 0xaaabaaab);
	xmm3 = _mm_srli_epi16(_mm_mulhi_epu16(xmm3, xmm5), 1);
	xmm3 = _mm_packus_epi16(xmm3, xmm0);

	if( wBlockColor2[0] > wBlockColor2[1] ){
		dwRGBIndex[2] = xmm3.m128i_u32[1];
	}else{
		xmm1 = _mm_avg_epu8(xmm1, xmm2);
		dwRGBIndex[2] = xmm1.m128i_u32[1];
	}
	dwRGBIndex[3] = xmm3.m128i_u32[0];

}

void TPicDlg::decode_dds_dxt1(BYTE *ddsimage)
{

	UINT32	*lpdwBuffer = (UINT32*)cleanimage;
	UINT16	*lpwDdsBuffer = (UINT16*)ddsimage;

	UINT32	dwQuotX, dwQuotY;
	UINT32	dwColorIndexInBlock[4];

	//dxt格式要求x和y必须是4的倍数
	//dwRemX = picWidth & 0x3;
	//dwRemY = picHeight & 0x3;

	dwQuotX = picWidth & 0xfffffffc;
	dwQuotY = picHeight & 0xfffffffc;

	for(UINT32 ly = 0;ly<dwQuotY;ly += 4)
	{
		for(UINT32 lx = 0;lx<dwQuotX;lx += 4)
		{

			CalcRgbsDXT1(dwColorIndexInBlock, lpwDdsBuffer);
			lpwDdsBuffer += 2;

			UINT32	iColorIndex = *(UINT32*)lpwDdsBuffer;
			lpwDdsBuffer += 2;

			for(int j = 0, k = 0; j < 4; ++j )
			{
				for(int i = 0; i < 4; ++i, k += 2 )
				{
					int index = (iColorIndex >> k) & 0x03 ;
					*(lpdwBuffer + (ly + j) * picWidth + lx + i) = dwColorIndexInBlock[index];
				}
			}

		}
				
	}
}

void TPicDlg::decode_dds_dxt3(BYTE *ddsimage)
{

	UINT32	*lpdwBuffer = (UINT32*)cleanimage;
	UINT16	*lpwDdsBuffer = (WORD*)ddsimage;

	//4x4
	UINT32	dwQuotX, dwQuotY;
	UINT32	dwColorIndexInBlock[4];

	dwQuotX = picWidth & 0xfffffffc;
	dwQuotY = picHeight & 0xfffffffc;

	for(UINT32 ly = 0;ly<dwQuotY;ly += 4)
	{
		for(UINT32 lx = 0;lx<dwQuotX;lx += 4)
		{
			BYTE	*dwAlpha;

			__m128i xmm0, xmm1, xmm2, xmm3;
			//__m128i xmmAlphas[4];

			dwAlpha = (BYTE*)&xmm3;

			xmm0 = _mm_setzero_si128();
			xmm1 = _mm_loadl_epi64((__m128i*)lpwDdsBuffer);
			xmm2 = _mm_unpacklo_epi8(xmm1, xmm0);
			xmm1 = _mm_unpacklo_epi8(xmm0, xmm1);
			xmm3 = _mm_slli_epi32(xmm2, 4);
			xmm2 = _mm_or_si128(xmm3, xmm1);
			xmm1 = _mm_set_epi32(0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0);
			xmm3 = _mm_and_si128(xmm2, xmm1);

			lpwDdsBuffer += 4;

			CalcRgbsDXT35(dwColorIndexInBlock, lpwDdsBuffer);
			lpwDdsBuffer +=2;

			UINT32	iColorIndex = *(UINT32*)lpwDdsBuffer;
			lpwDdsBuffer +=2;

			for(int j = 0, k = 0; j < 4; ++j )
			{
				for(int i = 0; i < 4; ++i, ++k )
				{
					int cindex = iColorIndex & 0x03;
					iColorIndex>>=2;

					UINT32	*lpbufptr = lpdwBuffer + (ly + j) * picWidth + lx + i;
					*lpbufptr = dwColorIndexInBlock[cindex] | (dwAlpha[k] << 24);

				}
			}

		}

	}

}


void TPicDlg::decode_dds_dxt5(BYTE *ddsimage)
{

	UINT32	*lpdwBuffer = (UINT32*)cleanimage;
	UINT16	*lpwDdsBuffer = (WORD*)ddsimage;

	//4x4
	UINT32	dwQuotX, dwQuotY;
	UINT32	dwColorIndexInBlock[4];

	dwQuotX = picWidth & 0xfffffffc;
	dwQuotY = picHeight & 0xfffffffc;

	QWORD	iAlphaIndex;

	for(UINT32 ly = 0;ly<dwQuotY;ly += 4)
	{
		for(UINT32 lx = 0;lx<dwQuotX;lx += 4)
		{
			//DXT5使用64bit 保存 alpha 信息，其中前2个8bit用来存放alpha0,alpha1,这是最大值和最小值，后3*16个bit存放索引，如果
			//a0>a1,就使用8级插值，如果a0<=a1,就保留110(a6)和111(a7)来显示alpha为0时和255时的情况。
			BYTE *dwAlpha01 = (BYTE*)lpwDdsBuffer;
			++lpwDdsBuffer;

			//BYTE bAlpha[8];
			//*(UINT16*)bAlpha = *(UINT16*)dwAlpha01;
			//BYTE	*lpdwAlpha = bAlpha + 2;
			////BYTE	*lpdwAlpha1 = dwAlpha + 1;

			//if(bAlpha[0] > bAlpha[1])
			//{
			//	for(BYTE i=1;i<7;++i)
			//	{
			//		*lpdwAlpha = bAlpha[0] - ((bAlpha[0] - bAlpha[1]) * i)/ 7.0;
			//		++lpdwAlpha;
			//	}
			//}else{
			//	for(BYTE i=1;i<5;++i)
			//	{	
			//		*lpdwAlpha = bAlpha[0] + ((bAlpha[1] - bAlpha[0]) * i)/ 5.0;
			//		++lpdwAlpha;
			//	}
			//	bAlpha[6] = 0;
			//	bAlpha[7] = 255;
			//}

			UINT16	*dwAlpha;
			__m128i xmm0, xmm1, xmma, xmm3, xmmab;
			dwAlpha = (UINT16*)&xmm3;
			xmm0 = _mm_setzero_si128();

			if(dwAlpha01[0] > dwAlpha01[1])
			{
				//x=((uint16(y)*M)>>17+y)>>2 m=0x924a ==>x=y/7
				xmma = _mm_set1_epi16(dwAlpha01[0]);
				xmmab = _mm_set1_epi16(dwAlpha01[0] - dwAlpha01[1]);
				xmm1 = _mm_setr_epi16(0, 7, 1, 2, 3, 4, 5, 6);
				xmm3 = _mm_mullo_epi16(xmmab, xmm1);
				xmm1 = _mm_set1_epi16(3);
				xmm3 = _mm_adds_epu16(xmm3, xmm1);
				xmm1 = _mm_set_epi32(0x924a924a, 0x924a924a, 0x924a924a, 0x924a924a);
				xmm3 = _mm_srli_epi16(_mm_mulhi_epu16(xmm3, xmm1), 2);
				xmm3 = _mm_subs_epu8(xmma, xmm3);

			}else{
				//x=((uint16(y)*M)>>17+y)>>1 m=0x6667 ==>x=y/5
				xmma = _mm_set1_epi16(dwAlpha01[0]);
				xmmab = _mm_set1_epi16(dwAlpha01[1] - dwAlpha01[0]);
				xmm1 = _mm_setr_epi16(0, 5, 1, 2, 3, 4, 0, 0);
				xmm3 = _mm_mullo_epi16(xmmab, xmm1);
				xmm1 = _mm_set1_epi16(2);
				xmm3 = _mm_adds_epu16(xmm3, xmm1);
				xmm1 = _mm_set_epi32(0x66676667, 0x66676667, 0x66676667, 0x66676667);
				xmm3 = _mm_srli_epi16(_mm_mulhi_epu16(xmm3, xmm1), 1);
				xmm3 = _mm_adds_epu8(xmma, xmm3);

				xmm3 = _mm_insert_epi16(xmm3, 0, 6);
				xmm3 = _mm_insert_epi16(xmm3, 255, 7);

			}

			iAlphaIndex = *(QWORD*)lpwDdsBuffer & 0x0000ffffffffffff;
			lpwDdsBuffer +=3;

			CalcRgbsDXT35(dwColorIndexInBlock, lpwDdsBuffer);
			lpwDdsBuffer +=2;

			UINT32	iColorIndex = *(UINT32*)lpwDdsBuffer;
			lpwDdsBuffer +=2;

			for(int j = 0, k = 0; j < 4; ++j ){
				for(int i = 0; i < 4; ++i, ++k ){
					int cindex = iColorIndex & 0x03;
					iColorIndex>>=2;

					int aindex =  iAlphaIndex & 0x07;
					iAlphaIndex>>=3;

					UINT32	*lpbufptr = lpdwBuffer + (ly + j) * picWidth + lx + i;
					*lpbufptr = dwColorIndexInBlock[cindex] | (dwAlpha[aindex] << 24);

				}
			}
		}
	}
}

void TPicDlg::decode_dds_a8r8g8b8(BYTE *ddsimage)
{

	UINT32	*lpdwBuffer = (UINT32*)cleanimage;
	UINT32	*lpwDdsBuffer = (UINT32*)ddsimage;

	for(UINT32 j = 0;j<picHeight;++j)
	{
		UINT32 *lpdwBufferPtr = lpdwBuffer + picWidth * j;
		for(UINT32 i = 0;i<picWidth;++i)
		{
			*lpdwBufferPtr = *lpwDdsBuffer;
			++lpdwBufferPtr;
			++lpwDdsBuffer;
		}
	}
}

void TPicDlg::decode_dds_x8r8g8b8(BYTE *ddsimage)
{
	decode_dds_a8r8g8b8(ddsimage);
	//decodedDIBFormat = PixelFormat32bppRGB;
}

void TPicDlg::decode_dds_a8b8g8r8(BYTE *ddsimage)
{

	UINT32	*lpdwBuffer = (UINT32*)cleanimage;
	UINT32	*lpwDdsBuffer = (UINT32*)ddsimage;

	for(UINT32 j = 0;j<picHeight;++j)
	{
		UINT32 *lpdwBufferPtr = lpdwBuffer + picWidth * j;
		for(UINT32 i = 0;i<picWidth;++i)
		{
			*lpdwBufferPtr = (*lpwDdsBuffer & 0xff00ff00) | (_lrotr(*lpwDdsBuffer, 16) & 0xff00ff);
			++lpdwBufferPtr;
			++lpwDdsBuffer;
		}
	}
}

void TPicDlg::decode_dds_x8b8g8r8(BYTE *ddsimage)
{
	decode_dds_a8b8g8r8(ddsimage);
	//decodedDIBFormat = PixelFormat32bppRGB;
}

void TPicDlg::decode_dds_r8g8b8(BYTE *ddsimage)
{

	BYTE	*lpdwBuffer = (BYTE*)cleanimage;
	BYTE	*lpwDdsBuffer = (BYTE*)ddsimage;

	//int bitsPerLine = picWidth * 3;
	//stride = (bitsPerLine & 0xfffffffc) + ((bitsPerLine & 3) ? 1 : 0);

	//for(int j = 0;j<picHeight;++j){
	//	memcpy(lpdwBuffer, lpwDdsBuffer, stride);
	//	lpdwBuffer += stride;
	//	lpwDdsBuffer += stride;
	//}

	memcpy(lpdwBuffer, lpwDdsBuffer, stride * picHeight);
}

void TPicDlg::decode_dds_a1r5g5b5(BYTE *ddsimage)
{


	BYTE	*lpdwBuffer = (BYTE*)cleanimage;
	BYTE	*lpwDdsBuffer = (BYTE*)ddsimage;

	memcpy(lpdwBuffer, lpwDdsBuffer, stride * picHeight);
}

void TPicDlg::decode_dds_x1r5g5b5(BYTE *ddsimage)
{
	decode_dds_a1r5g5b5(ddsimage);
	//decodedDIBFormat = PixelFormat16bppRGB555;
}

void TPicDlg::decode_dds_r5g6b5(BYTE *ddsimage)
{


	BYTE	*lpdwBuffer = (BYTE*)cleanimage;
	BYTE	*lpwDdsBuffer = (BYTE*)ddsimage;

	memcpy(lpdwBuffer, lpwDdsBuffer, stride * picHeight);

}

void TPicDlg::decode_dds_a4r4g4b4(BYTE *ddsimage)
{

	UINT32	*lpdwBuffer = (UINT32*)cleanimage;
	UINT32	*lpwDdsBuffer = (UINT32*)ddsimage;

	UINT32	nLoopCount = picHeight * picWidth / 4;

	for(UINT32 i = 0;i<nLoopCount;++i)
	{
		__m128i mm1 = _mm_setzero_si128();
		__m128i mm0 = _mm_set_epi32(0, 0, *(lpwDdsBuffer + 1), *lpwDdsBuffer);
		//__m128i mm0 = _mm_set_epi32(0, 0, 0x12345678, 0x9abcdef1);
		__m128i mm2;

		mm2 = _mm_unpacklo_epi8(mm0, mm1);
		mm0 = _mm_unpacklo_epi8(mm1, mm0);

		mm1 = _mm_slli_epi32(mm2, 4);
		mm2 = _mm_or_si128(mm1, mm0);

		mm0 = _mm_set_epi32(0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0);
		mm1 = _mm_and_si128(mm2, mm0);

		memcpy(lpdwBuffer, &mm1, sizeof(__m128i));
		lpdwBuffer += 4;
		lpwDdsBuffer += 2;
	}

}

void TPicDlg::decode_dds_x4r4g4b4(BYTE *ddsimage)
{
	decode_dds_a4r4g4b4(ddsimage);
	//decodedDIBFormat = PixelFormat32bppRGB;
}

void TPicDlg::decode_dds_a8r3g3b2(BYTE *ddsimage)
{

	UINT32	*lpdwBuffer = (UINT32*)cleanimage;
	UINT16	*lpwDdsBuffer = (UINT16*)ddsimage;

	for(UINT32 j = 0;j<picHeight;++j)
	{
		for(UINT32 i = 0;i<picWidth;++i)
		{
			//no test
			*lpdwBuffer =	(((*lpwDdsBuffer) & 0xff00) << 24) |
							(((*lpwDdsBuffer) & 0xe0) << 4) | 
							(((*lpwDdsBuffer) & 0x1c) << 3) | 
							(((*lpwDdsBuffer) & 0x3) << 2);
			++lpdwBuffer;
			++lpwDdsBuffer;
		}
	}
}

void TPicDlg::decode_dds_a2r10g10b10(BYTE *ddsimage)
{

	UINT32	*lpdwBuffer = (UINT32*)cleanimage;
	UINT32	*lpwDdsBuffer = (UINT32*)ddsimage;

	for(UINT32 j = 0;j<picHeight;++j)
	{
		for(UINT32 i = 0;i<picWidth;++i)
		{
			//no test
			*lpdwBuffer =	((*lpwDdsBuffer) & 0xc0000000) |
							(((*lpwDdsBuffer) & 0x3fc00000) >> 6) |
							(((*lpwDdsBuffer) & 0xff000) >> 4) |
							(((*lpwDdsBuffer) & 0x3fc) >> 2);
			++lpdwBuffer;
			++lpwDdsBuffer;
		}
	}
}

void TPicDlg::decode_dds_a2b10g10r10(BYTE *ddsimage)
{

	UINT32	*lpdwBuffer = (UINT32*)cleanimage;
	UINT32	*lpwDdsBuffer = (UINT32*)ddsimage;

	for(UINT32 j = 0;j<picHeight;++j)
	{
		for(UINT32 i = 0;i<picWidth;++i)
		{
			//no test
			*lpdwBuffer =	((*lpwDdsBuffer) & 0xc0000000) |
							(((*lpwDdsBuffer) & 0x3fc00000) >> 22) |
							(((*lpwDdsBuffer) & 0xff000) >> 4) |
							(((*lpwDdsBuffer) & 0x3fc) <<14 );
			++lpdwBuffer;
			++lpwDdsBuffer;
		}
	}
}


/////////////////////////////////////////////
// TGA

//////////////////////////////////////////////

BOOL TPicDlg::makeTgaColorMappedData(BYTE *&tgaimage, int fmt, char * &bufferOfColorMappedData, UINT16 wColorTableSize/*, int nTgaBitsPerPixel*/)
{

	switch(fmt)
	{
	case FMTTGA_RAWTBL:
	case FMTTGA_RLETBL:

		//dwColorTableCount = pHeader->wColorTableSize;

		//if(NULL == (bufferOfColorMappedData = (char*) malloc (wColorTableSize * 4)))
		//{
		//	return FALSE;
		//}

		bufferOfColorMappedData = (char*)tgaimage;

		//switch(nTgaBitsPerPixel)
		//{
		//case FMTTGA_A1R5G5B5:	//16bit
		//	{

		//		BYTE	*lpColorTableData = (BYTE*)tgaimage/*pHeader->wColorTableOffset*/;
		//		UINT32	*lpdwColorTablePtr = (UINT32*)bufferOfColorMappedData;

		//		hasAlpha = TRUE;

		//		for(unsigned long i = 0;i<wColorTableSize;++i)
		//		{
		//			*lpdwColorTablePtr = MakeRgb(0, *lpColorTableData, 
		//											0x8000, 0x7c00, 0x3e0, 0x1f,
		//											1, 5, 5, 5, 
		//											15, 10, 5, 0,
		//											hasAlpha);
		//			++lpdwColorTablePtr;
		//			++lpColorTableData;

		//		}
		//	}
		//	break;

		//case FMTTGA_R8G8B8:
		//	{
		//		BYTE	*lpColorTableData = (BYTE*)tgaimage/*pHeader->wColorTableOffset*/;
		//		UINT32	*lpdwColorTablePtr = (UINT32*)bufferOfColorMappedData;

		//		for(unsigned long i = 0;i<wColorTableSize;++i)
		//		{
		//			*lpdwColorTablePtr = (*(UINT32*)lpColorTableData) & 0x00ffffff;
		//			lpColorTableData += 3;
		//			++lpdwColorTablePtr;
		//		}

		//	}
		//	break;

		//case FMTTGA_A8R8G8B8:

		//	hasAlpha = TRUE;

		//	memcpy(bufferOfColorMappedData, tgaimage/*pHeader->wColorTableOffset*/, wColorTableSize * 4);

		//	break;

		//default:

		//	break;
		//}

		tgaimage += wColorTableSize;

		break;

	default:
		break;
	}
	return TRUE;
}


void TPicDlg::decode_tga_ColorMapped8(BYTE *tgaimage, char *lpColorMappedData)
{

	char	*lpdwBuffer = cleanimage + stride * (picHeight - 1);
	BYTE *lpRgbdata = tgaimage;

	for(UINT i = 0;i<picHeight;++i){

		char* lpdwBufferPtr = lpdwBuffer;

		for(UINT j = 0;j<picWidth;++j){

			memcpy(lpdwBufferPtr, lpColorMappedData + (*lpRgbdata * bytesPerPixel), bytesPerPixel);
			++lpRgbdata;
			lpdwBufferPtr += bytesPerPixel;

		}
		lpdwBuffer -= stride;

	}

}

void TPicDlg::decode_tga_ColorMapped16(BYTE *tgaimage, char *lpColorMappedData)
{
	char	*lpdwBuffer = /*(UINT16*)*/(cleanimage + stride * (picHeight - 1));
	UINT16 *lpRgbdata = (UINT16*)tgaimage;
				
	for(UINT i = 0;i<picHeight;++i){

		char* lpdwBufferPtr = lpdwBuffer;

		for(UINT j = 0;j<picWidth;++j){

			memcpy(lpdwBufferPtr, lpColorMappedData + (*lpRgbdata * bytesPerPixel), bytesPerPixel);
			++lpRgbdata;
			lpdwBufferPtr += bytesPerPixel;

		}
		lpdwBuffer -= stride;

	}

}

void TPicDlg::decode_tga_RGB(BYTE *tgaimage)
{

	char	*lpdwBuffer = /*(UINT16*)*/(cleanimage + stride * (picHeight - 1));
	BYTE	*lpRgbdata = /*(UINT16*)*/tgaimage;
	
	//memcpy(lpdwBuffer, lpRgbdata, stride * picHeight);

	for(UINT i = 0;i<picHeight;++i){

		memcpy(lpdwBuffer, lpRgbdata, bytesPerLine);
		lpdwBuffer -= stride;
		lpRgbdata += bytesPerLine;

	}
}

void TPicDlg::decode_tga_RGBREL(BYTE *tgaimage)
{
	char	*lpdwBuffer = cleanimage + stride * (picHeight - 1);

	BYTE	*lpRgbdata = (BYTE*)tgaimage;
	BYTE	byteRleRepeatDataCount;

	UINT dwPixels = picHeight * picWidth;

	UINT32 i = 0, iRowPoint = 0;

	char* lpdwBufferPtr = lpdwBuffer;

	while(i < dwPixels)
	{
		
		if(bytesPerLine == iRowPoint)
			iRowPoint = 0, lpdwBufferPtr = (lpdwBuffer -= stride);

		//检查是否run-length 数据包
		byteRleRepeatDataCount = (*lpRgbdata & 0x7f) + 1;

		if( *lpRgbdata & 0x80)
		{
			++lpRgbdata;

			for(int j = 0;j<byteRleRepeatDataCount;++j, ++i){

				memcpy(lpdwBufferPtr, lpRgbdata, bytesPerPixel);
				//*(UINT16*)lpdwBufferPtr = *(UINT16*)lpRgbdata;
				iRowPoint += bytesPerPixel;
				//if(stride == (iRowPoint += 2))
					//iRowPoint = 0, lpdwBufferPtr = (lpdwBuffer -= stride);
				//else
				lpdwBufferPtr += bytesPerPixel;

			}

			lpRgbdata+=bytesPerPixel;

		}else{
			++lpRgbdata;

				for(int j = 0;j<byteRleRepeatDataCount;++j, ++i)
				{
					memcpy(lpdwBufferPtr, lpRgbdata, bytesPerPixel);
					//*(UINT16*)lpdwBufferPtr = *(UINT16*)lpRgbdata;
					iRowPoint += bytesPerPixel;
					//if(stride == (iRowPoint += 2))
					//	iRowPoint = 0, lpdwBufferPtr = (lpdwBuffer -= stride);
					//else
					lpdwBufferPtr += bytesPerPixel;
					lpRgbdata+=bytesPerPixel;
				}

		}

	}
}

void TPicDlg::decode_tga_ColorMapped8REL(BYTE *tgaimage, char *lpColorMappedData)
{
	char	*lpdwBuffer = cleanimage + stride * (picHeight - 1);

	BYTE	*lpRgbdata = (BYTE*)tgaimage;
	BYTE	byteRleRepeatDataCount;

	UINT dwPixels = picHeight * picWidth;

	UINT32 i = 0, iRowPoint = 0;

	char* lpdwBufferPtr = lpdwBuffer;

	while(i < dwPixels)
	{

		if(bytesPerLine == iRowPoint)
			iRowPoint = 0, lpdwBufferPtr = (lpdwBuffer -= stride);

		//检查是否run-length 数据包
		byteRleRepeatDataCount = *lpRgbdata & 0x7f;

		if( *lpRgbdata & 0x80)
		{
			++lpRgbdata;

			for(int j = 0;j<byteRleRepeatDataCount;++j, ++i)
			{
				//*lpdwBufferPtr = *(lpColorMappedData + *lpRgbdata);
				memcpy(lpdwBufferPtr, lpColorMappedData + (*lpRgbdata * bytesPerPixel), bytesPerPixel);

				iRowPoint += bytesPerPixel;
				lpdwBufferPtr += bytesPerPixel;
				//++lpdwBuffer;
			}
			++lpRgbdata;

		}else{
			++lpRgbdata;

			for(int j = 0;j<byteRleRepeatDataCount;++j, ++i)
			{
				//*lpdwBufferPtr = *(lpColorMappedData + *lpRgbdata);
				memcpy(lpdwBufferPtr, lpColorMappedData + (*lpRgbdata * bytesPerPixel), bytesPerPixel);

				iRowPoint += bytesPerPixel;
				lpdwBufferPtr += bytesPerPixel;
				++lpRgbdata;
				//++lpdwBuffer;
			}

		}

	}
}

void TPicDlg::decode_tga_ColorMapped16REL(BYTE *tgaimage, char *lpColorMappedData)
{
	char	*lpdwBuffer = cleanimage + stride * (picHeight - 1);

	UINT16	*lpRgbdata = (UINT16*)tgaimage;
	BYTE	byteRleRepeatDataCount;

	UINT	dwPixels = picHeight * picWidth;

	UINT32 i = 0, iRowPoint = 0;

	char* lpdwBufferPtr = lpdwBuffer;

	while(i < dwPixels)
	{
		if(bytesPerLine == iRowPoint)
			iRowPoint = 0, lpdwBufferPtr = (lpdwBuffer -= stride);

		//检查是否run-length 数据包
		byteRleRepeatDataCount = *lpRgbdata & 0x7f;

		if( *lpRgbdata & 0x80)
		{
			++lpRgbdata;

			for(int j = 0;j<byteRleRepeatDataCount;++j, ++i)
			{
				//*lpdwBufferPtr = *(lpColorMappedData + *(WORD*)lpRgbdata);
				memcpy(lpdwBufferPtr, lpColorMappedData + (*lpRgbdata * bytesPerPixel), bytesPerPixel);

				iRowPoint += bytesPerPixel;
				lpdwBufferPtr += bytesPerPixel;
				//++lpdwBuffer;
			}
			++lpRgbdata;


		}else{
			++lpRgbdata;

			for(int j = 0;j<byteRleRepeatDataCount;++j, ++i)
			{
				//*lpdwBufferPtr = *(lpColorMappedData + *(WORD*)lpRgbdata);
				memcpy(lpdwBufferPtr, lpColorMappedData + (*lpRgbdata * bytesPerPixel), bytesPerPixel);

				iRowPoint += bytesPerPixel;
				lpdwBufferPtr += bytesPerPixel;
				++lpRgbdata;
				//++lpdwBuffer;
			}

		}

	}
}