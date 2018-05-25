
#if 0
UINT64	MakeRgbWithAlpha(UINT32 *dwBackColor, UINT32 *dwSrc)
{
	//将前景图片与背景图片进行混合，混合的alpha值取自前景的alpha值
	//////////////////////////////////////////////
	//	dwBackColor:dst
	//	lpcRGB:src
	//	dst = src * alpha/255 + dst * (255-alpha)/255
	//	mm2=0
	//	mm0=dst=a0|a1|a2|a3|a4|a5|a6|a7=00|00|00|00|a4|a5|a6|a7
	//	mm1=src=b0|b1|b2|b3|b4|b5|b6|b7=00|00|00|00|b4|b5|b6|b7
	//////////////////////////////////////////////
#if 0
	//此处为C语言
	//c部分一次可以计算1个字节
	UINT32 a = (dwSrc >> 24) & 0xff;
	switch(a) {
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
			cRGB.cRGBVal.r = o + ((a * (r - o)) >> 8);
		else
			cRGB.cRGBVal.r = o - ((a * (o - r)) >> 8);

		if(g > o)
			cRGB.cRGBVal.g = o + ((a * (g - o)) >> 8);
		else
			cRGB.cRGBVal.g = o - ((a * (o - g)) >> 8);

		if(b > o)
			cRGB.cRGBVal.b = o + ((a * (b - o)) >> 8);
		else
			cRGB.cRGBVal.b = o - ((a * (o - b)) >> 8);

		return cRGB.dwargb;
	}

#endif

#if 0
	//MMX
	//c部分一次可以计算1个字节
	//MMX
	//  mm0=dst=00|00|00|00|04|a5|a6|ff
	//	mm1=src=00|00|00|00|ff|b5|b6|07
	__m64 mm2 = _m_from_int(0), mm0 = _m_from_int(dwBackColor), mm1 = _m_from_int(dwSrc);
	//mm0 = 00|04|00|a5|00|a6|00|ff
	mm0 = _m_punpcklbw(mm0, mm2);
	//mm1 = 00|ff|00|b5|00|b6|00|07 //Overlay:8 bit到16 bit以容纳结果.32bit expand to 64 bit
	mm1 = _m_punpcklbw(mm1, mm2);
	//mm3 = 00|ff|00|b5|00|b6|00|07// 因为要用Overlay的Alpha值
	__m64 mm3 = mm1;
	mm3 = _m_punpckhwd(mm3, mm3);
	//mm3 = 00|ff|00|ff|00|ff|00|ff // 双字移动到四字，现在有八个像素的Alpha了!
	mm3 = _m_punpckhdq(mm3, mm3);
	//mm4 = 00|04|00|a5|00|a6|00|ff //Source->mm4
	__m64 mm4 = mm0;
	//mm5 = 00|ff|00|b5|00|b6|00|07 //Overlay->mm5
	__m64 mm5 = mm1;
	//mm4 = 00|00|00|00|00|00|00|f8 //Source-Overlay，饱和减，小于0为0
	//mm5 = 00|fb|00|10|00|10|00|00 //Overlay-Source，饱和减，小于0为0
	mm4 = _m_psubusw(mm4, mm1);
	mm5 = _m_psubusw(mm5, mm0);
	//mm4 = 00|00|00|00|00|00|f7|08 //Alpha * (Source-Overlay)
	//mm5 = fa|05|0f|f0|0f|f0|00|00 //Alpha * (Overlay-Source)
	mm4 = _m_pmullw(mm4, mm3);
	mm5 = _m_pmullw(mm5, mm3);
	//mm4 = 00|00|00|00|00|00|00|f7 //除以256，now mm4 get the result，(Source-Overlay)<0 部分
	//mm5 = 00|fa|00|0f|00|0f|00|00 //除以256，now mm5 get the result，(Overlay-Source)>0 部分
	mm4 = _m_psrlw(mm4, _m_from_int(8));
	mm5 = _m_psrlw(mm5, _m_from_int(8));
	//mm0 = 00|fe|00|b4|00|b5|00|ff //饱和加到原图象:D=Alpha*(O-S)+S，(Source-Overlay)<0 部分
	//mm0 = 00|fe|00|b4|00|b5|00|08 //饱和加到原图象D=S-Alpha*(S-O)，(Overlay-Source)>0 部分
	mm0 = _m_paddusw(mm0, mm5);
	mm0 = _m_psubusw(mm0, mm4);
	//mm0 = fe|b4|b5|08|fe|b4|b5|08//紧缩到低32bit
	//movd eax, mm0
	//emms ///必要的!Empty MMX Status
	mm0 = _m_packuswb(mm0, mm0);
	_m_empty();
	return _m_to_int(mm0);
#endif

#if 0
	//SSE2
	//c部分一次可以计算2个字节
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
#endif
}
#endif
#if 0
void MakeRgbWithAlphaAll(BOOL hasAlpha)
{

	UINT32	*lpdwBuffer;
	UINT32	*lpdwSrc;

	BYTE *dst = (BYTE*)hG + sizeof(BMPHEAD);
	BYTE *src = (BYTE*)cleanimage + sizeof(BMPHEAD);

	//画背景色格子，0x00ffffff,0x00cccccc
	UINT32	dwBlockX, dwBlockY, dwLastBlockY;
	dwBlockX = picWidth>>3;
	dwBlockY = picHeight>>3;
	dwLastBlockY = picHeight & 7;

	UINT32	*lpdwLine = (UINT32*) malloc ((dwBlockX + 2)<<5);
	
	lpdwBuffer = lpdwLine;

	for(UINT32 i=0;i<dwBlockX + 2;++i)
	{
		if(i & 1)
		{
			for(int j=0;j<8;++j)
				*lpdwBuffer++ = 0x00ffffff;
		}
		else
		{
			for(int j=0;j<8;++j)
				*lpdwBuffer++ = 0x00cccccc;
		}
	}

	lpdwBuffer = (UINT32*)(dst);

	for(UINT32 i=0;i<=dwBlockY;++i)
	{
		int nPixs = dwBlockY == i ? dwLastBlockY : 8;
		if(i & 1)
		{
			for(int j=0;j<nPixs;++j)
			{
				memcpy(lpdwBuffer, lpdwLine, picWidth<<2);
				lpdwBuffer += picWidth;
			}
		}else{
			for(int j=0;j<nPixs;++j)
			{
				memcpy(lpdwBuffer, lpdwLine + 8, picWidth<<2);
				lpdwBuffer += picWidth;
			}
		}
	}

	free(lpdwLine);

	lpdwBuffer = (UINT32*)(dst);
	lpdwSrc =  (UINT32*)(src);
	
	SIMD thisCPU;
	if(thisCPU.HasSSE2()){

		UINT dwPixels = picHeight * picWidth / 2;
		for(int i = 0;i<dwPixels;++i){
				*(UINT64*)lpdwBuffer = MakeRgbWithAlphaSSE2(lpdwBuffer, lpdwSrc);
				lpdwBuffer += 2;
				lpdwSrc += 2;
		}
	}else{

		for(int i = 0;i<picHeight;++i){
			for(int j = 0;j<picWidth;++j){
				//cMyRGB.dwargb = *lpdwSrc;
				*lpdwBuffer = MakeRgbWithAlpha((*lpdwBuffer), *lpdwSrc);
				++lpdwBuffer;
				++lpdwSrc;
			}
		}
	}
#endif

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
//	return TRUE;
//}
//
