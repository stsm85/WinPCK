//////////////////////////////////////////////////////////////////////
// ShowPicture.cpp: 显示dds、tga图像
// 解码dds、tga并显示
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2018.5.29
//////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include "ShowPicture.h"
#include <stdio.h>
#include <tchar.h>

#include <math.h>

CShowPicture::CShowPicture(HWND hWndShow, LPBYTE &_buffer, size_t _bufsize, LPCWSTR _lpszFileTitle, PICFORMAT _picFormat) :
	m_hWndShow(hWndShow),
	m_lpszFileTitle(_lpszFileTitle),
	m_picFormat(_picFormat),
	m_bufsize(_bufsize),
	m_stream(NULL),
	m_hGlobal(NULL),
	m_lpmyImage(NULL),
	m_lpoGraph(NULL),
	m_DecodedDIBData(NULL)
{


	if(FMT_RAW != (m_picFormat))	//dds,tga
	{
		m_resBufferInClass = (LPBYTE)malloc(m_bufsize);
	} else {							//bmp,jpg,png..
		m_resBufferInClass = (LPBYTE)(m_hGlobal = GlobalAlloc(GMEM_FIXED, m_bufsize));
	}

	if(NULL != m_resBufferInClass) {
		//复制数据
		memcpy(m_resBufferInClass, _buffer, _bufsize);
	}

	//复制完成后释放传入的内存
	free(_buffer);
	_buffer = NULL;
	
	GdiplusStartup(&m_pGdiPlusToken, &m_oGdiPlusStartupInput, NULL);
}


CShowPicture::~CShowPicture()
{
	if(NULL != m_lpoGraph)
		delete m_lpoGraph;

	if(NULL != m_lpmyImage)
		delete m_lpmyImage;

	if(NULL != m_stream)
		m_stream->Release();

	if(FMT_RAW != m_picFormat) {
		if(NULL != m_resBufferInClass) {
			free(m_resBufferInClass);
		}
	} else {

		if(NULL != m_hGlobal)
			GlobalFree(m_hGlobal);
	}

	if(NULL != m_DecodedDIBData) {
		free(m_DecodedDIBData);
	}

	DeleteObject(m_MemBitmap);
	DeleteDC(m_MemDC);

	GdiplusShutdown(m_pGdiPlusToken);
}

BOOL CShowPicture::isEqual(double d1, double d2)
{
	return (EPSILON > fabs((d1-d2)));
}

LPCWSTR	CShowPicture::GetWindowTitle(LPWSTR	lpszTitle, size_t bufSize)
{
	static wchar_t szTitle[MAX_PATH];

	if(NULL == lpszTitle)
		lpszTitle = szTitle;

	//窗口文字
	if(FMT_RAW != m_picFormat) {
		swprintf_s(lpszTitle, MAX_PATH, L"图片查看 - %s, %dx%d, %S", m_lpszFileTitle, m_picWidth, m_picHeight, m_szPictureFormat);
	} else {
		swprintf_s(lpszTitle, MAX_PATH, L"图片查看 - %s, %dx%d", m_lpszFileTitle, m_picWidth, m_picHeight);
	}

	//SetWindowText(m_hWndShow, lpszTitle);
	return lpszTitle;
}

BOOL CShowPicture::ShowTitleOnWindow()
{
	wchar_t szTitle[MAX_PATH];

	//窗口文字
	if(FMT_RAW != m_picFormat) {

		swprintf_s(szTitle, MAX_PATH, L"图片查看 - %s, %dx%d, %S", m_lpszFileTitle, m_picWidth, m_picHeight, m_szPictureFormat);

	} else {
		swprintf_s(szTitle, MAX_PATH, L"图片查看 - %s, %dx%d", m_lpszFileTitle, m_picWidth, m_picHeight);
	}

	SetWindowTextW(m_hWndShow, szTitle);
	return TRUE;
}

//返回图像的宽和高
UINT CShowPicture::GetWidth()
{
	return m_picWidth;
}

UINT CShowPicture::GetHeight()
{
	return m_picHeight;
}


void CalcZoomInAreaAndDrawPosition(__int64 &dst_xy, __int64 &dst_wh, __int64 &src_xy, __int64 &src_wh, double dZoomRatio, LONG iClientWidthHeight, UINT uiPicWidthHeight)
{
	/*
	当缩放后图像大于窗口后，设定窗口左上角坐标为 0,0
	左端未显示到的区域设定为left_invisible(绝对值),右端为right_invisible，设缩放率为Ratio，
	则左侧未显示的实际图像大小区域为 left_invisible_real = (int)left_invisible/Ratio，余数为left_invisible_rem
	右侧相同为right_invisible = tmp_dst_w - left_invisible - rectDlg.right，right_invisible_real = (int)right_invisible/Ratio，right_invisible_rem
	同时相除后的余数(left_invisible_rem)为要取数据在缩放后显示在窗口上的偏移
	设实际图像宽度为m_picWidth实际要取的数据为：visible_width_real = m_picWidth - left_invisible_real - right_invisible_real,
	目标左上角显示坐标dst_x = left_invisible_rem
	源左上角X坐标src_x = left_invisible_real
	*/

	__int64		left_invisible = -dst_xy;
	double	left_invisible_real_double = left_invisible / dZoomRatio;
	__int64		left_invisible_real = (__int64)left_invisible_real_double;
	__int64		left_invisible_rem = (__int64)(left_invisible - left_invisible_real * dZoomRatio);
	__int64		right_invisible = dst_wh - left_invisible - iClientWidthHeight;
	__int64		right_invisible_real_double = (__int64)(right_invisible / dZoomRatio);
	__int64		right_invisible_real = (__int64)right_invisible_real_double;
	__int64		right_invisible_rem = (__int64)(right_invisible - right_invisible_real * dZoomRatio);
	__int64		visible_width_real = uiPicWidthHeight - left_invisible_real - right_invisible_real;

	dst_xy = -left_invisible_rem;
	src_xy = left_invisible_real;
	src_wh = visible_width_real;
	dst_wh = iClientWidthHeight + right_invisible_rem + left_invisible_rem;

}

//在设备上显示图像
BOOL CShowPicture::Paint(__int64 nXOriginDest, __int64 nYOriginDest, int nXOriginSrc, int nYOriginSrc, double dZoomRatio)
{
	/*
	设定窗口左上角坐标为 0,0
	pic_x:待显示图像 的左上角相对于窗口的位置
	*/
	RECT rectDlg;

	HDC pDC = ::GetDC(m_hWndShow);
	GetClientRect(m_hWndShow, &rectDlg);

	//重计算源起点、宽高和目标起点、宽高
	__int64 src_x = nXOriginSrc;
	__int64 src_y = nYOriginSrc;
	__int64 src_w = m_picWidth;
	__int64 src_h = m_picHeight;
	__int64 dst_x = nXOriginDest;
	__int64 dst_y = nYOriginDest;
	__int64 dst_w = m_picWidth;
	__int64 dst_h = m_picHeight;

	PAINTSTRUCT ps;
	HDC pDC1 = BeginPaint(m_hWndShow, &ps);

	if(((1.0 - EPSILON) < dZoomRatio) && ((1.0 + EPSILON) > dZoomRatio)) {
		//if(isEqual(1.0, dZoomRatio)) {
			//不缩放
		if(nXOriginDest < 0) {

			dst_x = 0;
			src_x = nXOriginSrc - nXOriginDest;
			dst_w = (((__int64)m_picWidth - src_x) > (__int64)rectDlg.right) ? (__int64)rectDlg.right : ((__int64)m_picWidth - src_x);
		}
		if(nYOriginSrc < 0) {

			dst_y = 0;
			src_y = nYOriginSrc - nYOriginDest;
			dst_h = (((__int64)m_picHeight - src_y) >(__int64)rectDlg.bottom) ? (__int64)rectDlg.bottom : ((__int64)m_picHeight - src_y);
		}

		BitBlt(	
			pDC,				// 目标 DC 句柄
			(int)dst_x,				// 目标左上角X坐标
			(int)dst_y,				// 目标左上角Y坐标
			(int)dst_w,				// 目标宽度
			(int)dst_h,				// 目标高度
			m_MemDC, 			// 源 DC 句柄
			(int)src_x,				// 源左上角X坐标
			(int)src_y,				// 源左上角Y坐标
			SRCCOPY);

	} else {
		//缩放
		dst_w = (__int64)(m_picWidth * dZoomRatio + 0.5);
		dst_h = (__int64)(m_picHeight * dZoomRatio + 0.5);

		if(rectDlg.right < dst_w)
			CalcZoomInAreaAndDrawPosition(dst_x, dst_w, src_x, src_w, dZoomRatio, rectDlg.right, m_picWidth);

		if(rectDlg.bottom < dst_h)
			CalcZoomInAreaAndDrawPosition(dst_y, dst_h, src_y, src_h, dZoomRatio, rectDlg.bottom, m_picHeight);

		SetStretchBltMode(pDC, COLORONCOLOR);
		StretchBlt(
			pDC,				// 目标 DC 句柄
			(int)dst_x,				// 目标左上角X坐标
			(int)dst_y,				// 目标左上角Y坐标
			(int)dst_w,				// 目标宽度
			(int)dst_h,				// 目标高度
			m_MemDC,			// 源 DC 句柄
			(int)src_x,				// 源左上角X坐标
			(int)src_y,				// 源左上角Y坐标
			(int)src_w,				// 源宽度
			(int)src_h,				// 源高度
			SRCCOPY);
	}

	ReleaseDC(m_hWndShow, pDC);
	EndPaint(m_hWndShow, &ps);

	return TRUE;
}

//在窗口上先显示好透明背景的格子
BOOL CShowPicture::DrawBlockOnDlg()
{
	//HDC pDC = ::GetDC(GetDlgItem(IDC_STATIC_PIC));
	HDC pDC = ::GetDC(m_hWndShow);
	//GetClientRect(hWnd, &rect);

	//CDC MemDC; //首先定义一个显示设备对象
	HDC MemDCTemp;
	//CBitmap MemBitmap;//定义一个位图对象
	HBITMAP MemBitmapTemp;
	//随后建立与屏幕显示兼容的内存显示设备  
	m_MemDC = CreateCompatibleDC(NULL);
	MemDCTemp = CreateCompatibleDC(NULL);
	//这时还不能绘图，因为没有地方画
	//下面建立一个与屏幕显示兼容的位图，位图的大小可以用窗口的大小 
	m_MemBitmap = CreateCompatibleBitmap(pDC, m_picWidth, m_picHeight);
	MemBitmapTemp = CreateCompatibleBitmap(pDC, 24, 8);

	ReleaseDC(m_hWndShow, pDC);

	//将位图选入到内存显示设备中 
	//只有选入了位图的内存显示设备才有地方绘图，画到指定的位图上 
	HBITMAP pOldBit = (HBITMAP)SelectObject(m_MemDC, m_MemBitmap);
	HBITMAP pOldBit1 = (HBITMAP)SelectObject(MemDCTemp, MemBitmapTemp);

	//先用背景色将位图清除干净，这里我用的是白色作为背景 
	//你也可以用自己应该用的颜色 
	SetBkColor(MemDCTemp, RGB(204, 204, 204));
	RECT thisrect = { 0, 0, 24, 8 };
	ExtTextOut(MemDCTemp, 0, 0, ETO_OPAQUE, &thisrect, NULL, 0, NULL);

	thisrect.left += 8;
	SetBkColor(MemDCTemp, RGB(255, 255, 255));
	ExtTextOut(MemDCTemp, 0, 0, ETO_OPAQUE, &thisrect, NULL, 0, NULL);

	thisrect.left += 8;
	SetBkColor(MemDCTemp, RGB(204, 204, 204));
	ExtTextOut(MemDCTemp, 0, 0, ETO_OPAQUE, &thisrect, NULL, 0, NULL);

	for(unsigned int j = 0;j < m_picHeight;j += 8) {
		for(unsigned int i = 0;i < m_picWidth;i += 16) {
			if(j & 8)
				BitBlt(m_MemDC, i, j, i + 16, j + 8, MemDCTemp, 0, 0, SRCCOPY);
			else
				BitBlt(m_MemDC, i, j, i + 16, j + 8, MemDCTemp, 8, 0, SRCCOPY);
		}
	}
	//MemBitmap.DeleteObject(); 
	SelectObject(MemDCTemp, pOldBit1);
	//a = DeleteObject(MemBitmap);
	DeleteObject(MemBitmapTemp);
	//MemDC.DeleteDC();
	DeleteDC(MemDCTemp);

	return TRUE;
}

BOOL CShowPicture::Decode()
{
	if(NULL == m_resBufferInClass)return FALSE;
	if (!BuildImage())return FALSE;
	if (!DrawPictureOnDC())return FALSE;
	if (!ShowTitleOnWindow())return FALSE;
	return TRUE;
}