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

#include "showimgpch.h"
#include "ShowPicture.h"

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#pragma warning ( disable : 4018 )
#pragma warning ( disable : 4996 )

CShowPicture::CShowPicture()
{
	Gdiplus::GdiplusStartup(&this->m_pGdiPlusToken, &this->m_oGdiPlusStartupInput, NULL);
}

CShowPicture::~CShowPicture()
{
	if(NULL != this->m_lpmyImage)
		delete this->m_lpmyImage;

	if(NULL != this->m_stream)
		this->m_stream->Release();

	if(FMT_RAW != this->m_picFormat) {
		if(NULL != this->m_resBufferInClass) {
			free(this->m_resBufferInClass);
		}
	} else {

		if(NULL != this->m_hGlobal)
			GlobalFree(this->m_hGlobal);
	}

	if(NULL != this->m_DecodedDIBData) {
		free(this->m_DecodedDIBData);
	}

	DeleteObject(this->m_MemBitmap);
	DeleteDC(this->m_MemDC);

	Gdiplus::GdiplusShutdown(this->m_pGdiPlusToken);
}

bool CShowPicture::Attach(HWND hWndShow, LPBYTE _buffer, size_t _bufsize, LPCWSTR _lpszFileTitle, PICFORMAT _picFormat)
{
	this->m_hWndShow = hWndShow;
	this->m_lpszFileTitle = _lpszFileTitle;
	this->m_picFormat = _picFormat;
	this->m_bufsize = _bufsize;

	if (0 == _bufsize)
		return false;

	if (FMT_RAW != (this->m_picFormat))	//dds,tga
	{
		this->m_resBufferInClass = (LPBYTE)malloc(this->m_bufsize);
	}
	else {							//bmp,jpg,png..
		this->m_resBufferInClass = (LPBYTE)(this->m_hGlobal = GlobalAlloc(GMEM_FIXED, m_bufsize));
	}

	if (nullptr != this->m_resBufferInClass) {
		//复制数据
		memcpy(this->m_resBufferInClass, _buffer, _bufsize);
	}

	return this->Decode();
}

_inline BOOL CShowPicture::isEqual(double d1, double d2)
{
	return (EPSILON > fabs((d1-d2)));
}

std::wstring CShowPicture::GetWindowTitle() const
{
	std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> 
		converter(new std::codecvt<wchar_t, char, std::mbstate_t>("chs"));

	//窗口文字
	if(FMT_RAW != this->m_picFormat) {
		return std::format(L"图片查看 - {}, {}x{}, {}", this->m_lpszFileTitle, this->m_picWidth, this->m_picHeight, converter.from_bytes(this->m_szPictureFormat));
	} else {
		return std::format(L"图片查看 - {}, {}x{}", this->m_lpszFileTitle, this->m_picWidth, this->m_picHeight);
	}
}

//返回图像的宽和高
uint32_t CShowPicture::GetWidth() const
{
	return m_picWidth;
}

uint32_t CShowPicture::GetHeight() const
{
	return m_picHeight;
}


void CalcZoomInAreaAndDrawPosition(int64_t& dst_xy, int64_t& dst_wh, int64_t& src_xy, int64_t& src_wh, double dZoomRatio, LONG iClientWidthHeight, UINT uiPicWidthHeight)
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

	int64_t		left_invisible = -dst_xy;
	double	left_invisible_real_double = left_invisible / dZoomRatio;
	int64_t		left_invisible_real = (int64_t)left_invisible_real_double;
	int64_t		left_invisible_rem = (int64_t)(left_invisible - left_invisible_real * dZoomRatio);
	int64_t		right_invisible = dst_wh - left_invisible - iClientWidthHeight;
	int64_t		right_invisible_real_double = (int64_t)(right_invisible / dZoomRatio);
	int64_t		right_invisible_real = (int64_t)right_invisible_real_double;
	int64_t		right_invisible_rem = (int64_t)(right_invisible - right_invisible_real * dZoomRatio);
	int64_t		visible_width_real = uiPicWidthHeight - left_invisible_real - right_invisible_real;

	dst_xy = -left_invisible_rem;
	src_xy = left_invisible_real;
	src_wh = visible_width_real;
	dst_wh = iClientWidthHeight + right_invisible_rem + left_invisible_rem;

}

//在设备上显示图像
bool CShowPicture::Paint(int64_t nXOriginDest, int64_t nYOriginDest, int nXOriginSrc, int nYOriginSrc, double dZoomRatio, bool bErase)
{
	/*
	设定窗口左上角坐标为 0,0
	pic_x:待显示图像 的左上角相对于窗口的位置
	*/
	RECT rectDlg;
	::GetClientRect(this->m_hWndShow, &rectDlg);

	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint(this->m_hWndShow, &ps);

	//只有使用GetDC才能即时刷新
	HDC pDC = ::GetDC(this->m_hWndShow);

	if (this->IsDecoded()) {

		if (bErase)
			RedrawInvalidRect(pDC, rectDlg, nXOriginDest, nYOriginDest, dZoomRatio);

		DrawImage(pDC, rectDlg, ps, nXOriginDest, nYOriginDest, nXOriginSrc, nYOriginSrc, dZoomRatio);
	}
	else {
		if (bErase)
			::FillRect(pDC, &rectDlg, this->m_hBrushBack);
	}

	::ReleaseDC(this->m_hWndShow, pDC);
	::EndPaint(this->m_hWndShow, &ps);
	return true;
}

//绘制图像
void CShowPicture::DrawImage(HDC& hDC, RECT& rectDlg, PAINTSTRUCT& ps, int64_t nXOriginDest, int64_t nYOriginDest, int nXOriginSrc, int nYOriginSrc, double dZoomRatio)
{
	//重计算源起点、宽高和目标起点、宽高
	int64_t src_x = nXOriginSrc;
	int64_t src_y = nYOriginSrc;
	int64_t src_w = this->m_picWidth;
	int64_t src_h = this->m_picHeight;
	int64_t dst_x = nXOriginDest;
	int64_t dst_y = nYOriginDest;
	int64_t dst_w = this->m_picWidth;
	int64_t dst_h = this->m_picHeight;

	if (isEqual(dZoomRatio, 1.0)) {
		//不缩放
		if (nXOriginDest < 0) {

			dst_x = 0;
			src_x = nXOriginSrc - nXOriginDest;
			dst_w = (((int64_t)this->m_picWidth - src_x) > (int64_t)rectDlg.right) ? (int64_t)rectDlg.right : ((int64_t)this->m_picWidth - src_x);
		}
		if (nYOriginSrc < 0) {

			dst_y = 0;
			src_y = nYOriginSrc - nYOriginDest;
			dst_h = (((int64_t)this->m_picHeight - src_y) > (int64_t)rectDlg.bottom) ? (int64_t)rectDlg.bottom : ((int64_t)this->m_picHeight - src_y);
		}

		::BitBlt(
			hDC,				// 目标 DC 句柄
			(int)dst_x,				// 目标左上角X坐标
			(int)dst_y,				// 目标左上角Y坐标
			(int)dst_w,				// 目标宽度
			(int)dst_h,				// 目标高度
			this->m_MemDC, 			// 源 DC 句柄
			(int)src_x,				// 源左上角X坐标
			(int)src_y,				// 源左上角Y坐标
			SRCCOPY);

	} else {
		//缩放
		dst_w = (int64_t)(this->m_picWidth * dZoomRatio + 0.5);
		dst_h = (int64_t)(this->m_picHeight * dZoomRatio + 0.5);

		if (rectDlg.right < dst_w)
			CalcZoomInAreaAndDrawPosition(dst_x, dst_w, src_x, src_w, dZoomRatio, rectDlg.right, this->m_picWidth);

		if (rectDlg.bottom < dst_h)
			CalcZoomInAreaAndDrawPosition(dst_y, dst_h, src_y, src_h, dZoomRatio, rectDlg.bottom, this->m_picHeight);

		//当图片为缩小时，使用STRETCH_HALFTONE(质量更好)，放大时，使用STRETCH_DELETESCANS
		if (1.0 > dZoomRatio)
			::SetStretchBltMode(hDC, STRETCH_HALFTONE);
		else
			::SetStretchBltMode(hDC, this->m_upscale_mode);

		::StretchBlt(
			hDC,				// 目标 DC 句柄
			(int)dst_x,				// 目标左上角X坐标
			(int)dst_y,				// 目标左上角Y坐标
			(int)dst_w,				// 目标宽度
			(int)dst_h,				// 目标高度
			this->m_MemDC,			// 源 DC 句柄
			(int)src_x,				// 源左上角X坐标
			(int)src_y,				// 源左上角Y坐标
			(int)src_w,				// 源宽度
			(int)src_h,				// 源高度
			SRCCOPY);
	}
}

//绘制无效区域
void CShowPicture::RedrawInvalidRect(HDC& hdc, RECT& rectWin, int64_t nXOriginDest, int64_t nYOriginDest, double dZoomRatio)
{
	uint64_t dst_width = this->m_picWidth * dZoomRatio;
	uint64_t dst_height = this->m_picHeight * dZoomRatio;

	if (nXOriginDest > 0) {

		RECT rectInvalid = { 0, 0, nXOriginDest, rectWin.bottom };
		::FillRect(hdc, &rectInvalid, this->m_hBrushBack);
	}

	if (nYOriginDest > 0) {

		RECT rectInvalid = { nXOriginDest > 0 ? nXOriginDest : 0, 0, rectWin.right, nYOriginDest };
		::FillRect(hdc, &rectInvalid, this->m_hBrushBack);
	}

	int iShowBottom = nYOriginDest + dst_height;

	if (iShowBottom < rectWin.bottom) {

		RECT rectInvalid = { nXOriginDest > 0 ? nXOriginDest : 0, iShowBottom, rectWin.right, rectWin.bottom };
		::FillRect(hdc, &rectInvalid, this->m_hBrushBack);
	}

	int iShowRight = nXOriginDest + dst_width;
	if (iShowRight < rectWin.right) {

		RECT rectInvalid = { iShowRight, nYOriginDest > 0 ? nYOriginDest : 0,rectWin.right, iShowBottom < rectWin.bottom ? iShowBottom : rectWin.bottom };
		::FillRect(hdc, &rectInvalid, this->m_hBrushBack);
	}
}

bool CShowPicture::Decode()
{
	this->m_isDecoded = false;

	if(NULL == this->m_resBufferInClass)
		return false;

	if (!this->BuildImage())
		return false;

	if(nullptr != this->m_hWndShow)
		if (!this->DrawPictureOnDC())
		return false;

	this->m_isDecoded = true;
	return true;
}