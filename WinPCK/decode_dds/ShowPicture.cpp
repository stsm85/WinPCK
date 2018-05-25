#include <Windows.h>
#include "ShowPicture.h"
#include <tchar.h>


CShowPicture::CShowPicture(HWND hWndShow, LPBYTE &_buffer, size_t _bufsize, LPCTSTR _lpszFileTitle, PICFORMAT _picFormat) :
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

LPCTSTR	CShowPicture::GetWindowTitle(LPTSTR	lpszTitle, size_t bufSize)
{
	static TCHAR szTitle[MAX_PATH];

	if(NULL == lpszTitle)
		lpszTitle = szTitle;

	//窗口文字
	if(FMT_RAW != m_picFormat) {
#ifdef UNICODE
		_stprintf_s(lpszTitle, MAX_PATH, TEXT("图片查看 - %s, %dx%d, %S"), m_lpszFileTitle, m_picWidth, m_picHeight, m_szPictureFormat);
#else
		_stprintf_s(lpszTitle, MAX_PATH, TEXT("图片查看 - %s, %dx%d, %s"), m_lpszFileTitle, m_picWidth, m_picHeight, m_szPictureFormat);
#endif
	} else {
		_stprintf_s(lpszTitle, MAX_PATH, TEXT("图片查看 - %s, %dx%d"), m_lpszFileTitle, m_picWidth, m_picHeight);
	}

	//SetWindowText(m_hWndShow, lpszTitle);
	return lpszTitle;
}

BOOL CShowPicture::ShowTitleOnWindow()
{
	TCHAR szTitle[MAX_PATH];

	//窗口文字
	if(FMT_RAW != m_picFormat) {
#ifdef UNICODE
		_stprintf_s(szTitle, MAX_PATH, TEXT("图片查看 - %s, %dx%d, %S"), m_lpszFileTitle, m_picWidth, m_picHeight, m_szPictureFormat);
#else
		_stprintf_s(szTitle, MAX_PATH, TEXT("图片查看 - %s, %dx%d, %s"), m_lpszFileTitle, m_picWidth, m_picHeight, m_szPictureFormat);
#endif
	} else {
		_stprintf_s(szTitle, MAX_PATH, TEXT("图片查看 - %s, %dx%d"), m_lpszFileTitle, m_picWidth, m_picHeight);
	}

	SetWindowText(m_hWndShow, szTitle);
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

//在设备上显示图像
BOOL CShowPicture::Paint(int nXOriginDest, int nYOriginDest, int nXOriginSrc, int nYOriginSrc, double dZoomRatio)
{
	/*
	设定窗口左上角坐标为 0,0
	pic_x:待显示图像 的左上角相对于窗口的位置
	*/
	RECT rectDlg;

	HDC pDC = ::GetDC(m_hWndShow);
	GetClientRect(m_hWndShow, &rectDlg);

	//重计算源起点、宽高和目标起点、宽高
	int src_x = nXOriginSrc;
	int src_y = nYOriginSrc;
	int src_w = m_picWidth;
	int src_h = m_picHeight;
	int dst_x = nXOriginDest;
	int dst_y = nYOriginDest;
	int dst_w = m_picWidth;
	int dst_h = m_picHeight;

	PAINTSTRUCT ps;
	HDC pDC1 = BeginPaint(m_hWndShow, &ps);

	if((0.999 < dZoomRatio) && (1.0001 > dZoomRatio)) {
		//不缩放
		if(nXOriginDest < 0) {

			dst_x = 0;
			src_x = nXOriginSrc - nXOriginDest;
			dst_w = ((m_picWidth - src_x) > rectDlg.right) ? rectDlg.right : (m_picWidth - src_x);
		}
		if(nYOriginSrc < 0) {

			dst_y = 0;
			src_y = nYOriginSrc - nYOriginDest;
			dst_h = ((m_picHeight - src_y) > rectDlg.bottom) ? rectDlg.bottom : (m_picHeight - src_y);
		}

		BitBlt(	
			pDC,				// 目标 DC 句柄
			dst_x,				// 目标左上角X坐标
			dst_y,				// 目标左上角Y坐标
			dst_w,				// 目标宽度
			dst_h,				// 目标高度
			m_MemDC, 			// 源 DC 句柄
			src_x,				// 源左上角X坐标
			src_y,				// 源左上角Y坐标
			SRCCOPY);

	} else {
		//缩放
		int tmp_dst_w = m_picWidth * dZoomRatio, tmp_dst_h = m_picHeight * dZoomRatio;
		dst_w = m_picWidth * dZoomRatio;
		dst_h = m_picHeight * dZoomRatio;

		//	/* StretchBlt() Modes */
		//	#define BLACKONWHITE                 1
		//	#define WHITEONBLACK                 2
		//	#define COLORONCOLOR                 3
		//	#define HALFTONE                     4
		//	#define MAXSTRETCHBLTMODE            4
		//	#if(WINVER >= 0x0400)
		//	/* New StretchBlt() Modes */
		//	#define STRETCH_ANDSCANS    BLACKONWHITE
		//	#define STRETCH_ORSCANS     WHITEONBLACK
		//	#define STRETCH_DELETESCANS COLORONCOLOR
		//	#define STRETCH_HALFTONE    HALFTONE
#if 1
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
		if(rectDlg.right < tmp_dst_w) {

			int		left_invisible = -nXOriginDest;
			double	left_invisible_real_double = left_invisible / dZoomRatio;
			int		left_invisible_real = (int)left_invisible_real_double;
			int		left_invisible_rem = left_invisible - left_invisible_real * dZoomRatio;
			int		right_invisible = tmp_dst_w - left_invisible - rectDlg.right ;
			int		right_invisible_real_double = right_invisible / dZoomRatio;
			int		right_invisible_real = (int)right_invisible_real_double;
			int		right_invisible_rem = right_invisible - right_invisible_real * dZoomRatio;
			int		visible_width_real = m_picWidth - left_invisible_real - right_invisible_real;


			dst_x = -left_invisible_rem;
			src_x = left_invisible_real;
			src_w = visible_width_real;
			dst_w = rectDlg.right + right_invisible_rem + left_invisible_rem;

			double dZoomRatio_2 = dst_w / (double)src_w;
			dZoomRatio_2 = 1;
		}

		if(rectDlg.bottom < tmp_dst_h) {
			int		top_invisible = -nYOriginDest;
			double	top_invisible_real_double = top_invisible / dZoomRatio;
			int		top_invisible_real = (int)top_invisible_real_double;
			int		top_invisible_rem = top_invisible - top_invisible_real * dZoomRatio;
			int		bottom_invisible = tmp_dst_h - top_invisible - rectDlg.bottom;
			int		bottom_invisible_real_double = bottom_invisible / dZoomRatio;
			int		bottom_invisible_real = (int)bottom_invisible_real_double;
			int		bottom_invisible_rem = bottom_invisible - bottom_invisible_real * dZoomRatio;
			int		visible_height_real = m_picHeight - top_invisible_real - bottom_invisible_real;


			dst_y = -top_invisible_rem;
			src_y = top_invisible_real;
			src_h = visible_height_real;
			dst_h = rectDlg.bottom + bottom_invisible_rem + top_invisible_rem;

			double dZoomRatio_2 = dst_h / (double)src_h;
			dZoomRatio_2 = 1;
		}
#endif
		SetStretchBltMode(pDC, COLORONCOLOR);
		StretchBlt(
			pDC,				// 目标 DC 句柄
			dst_x,				// 目标左上角X坐标
			dst_y,				// 目标左上角Y坐标
			dst_w,				// 目标宽度
			dst_h,				// 目标高度
			m_MemDC,			// 源 DC 句柄
			src_x,				// 源左上角X坐标
			src_y,				// 源左上角Y坐标
			src_w,				// 源宽度
			src_h,				// 源高度
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

	for(int j = 0;j < m_picHeight;j += 8) {
		for(int i = 0;i < m_picWidth;i += 16) {
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