//////////////////////////////////////////////////////////////////////
// DrawPicture.cpp: 显示dds、tga图像
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
#include "dds/DdsTgaDecoder.h"

BOOL CShowPicture::BuildImage()
{

	try {

		switch (this->m_picFormat) {

#if INIT_DDS_MOD
		case FMT_DDS:
		{
			CDdsTgaDecoder cDdsTgaDecoder;

			if (!cDdsTgaDecoder.DecodeDDS(m_resBufferInClass, m_bufsize, m_DecodedDIBData, m_picWidth, m_picHeight, m_szPictureFormat))
				throw std::runtime_error("gui-str.show-picture-error");
			{
				free(m_resBufferInClass);
				m_resBufferInClass = nullptr;
				m_lpmyImage = new Gdiplus::Bitmap(m_picWidth, m_picHeight, cDdsTgaDecoder.GetStride(), cDdsTgaDecoder.GetPixelFormat(), m_DecodedDIBData);
			}
		}
			break;

		case FMT_TGA:
		{
			CDdsTgaDecoder cDdsTgaDecoder;
			if (!cDdsTgaDecoder.DecodeTGA(m_resBufferInClass, m_bufsize, m_DecodedDIBData, m_picWidth, m_picHeight, m_szPictureFormat))
				throw std::runtime_error("gui-str.show-picture-error");

			free(m_resBufferInClass);
			m_resBufferInClass = nullptr;

			m_lpmyImage = new Gdiplus::Bitmap(m_picWidth, m_picHeight, cDdsTgaDecoder.GetStride(), cDdsTgaDecoder.GetPixelFormat(), m_DecodedDIBData);
		}
			break;
#endif
		case FMT_RAW:	//bmp,jpg,png..

			CreateStreamOnHGlobal(m_hGlobal, FALSE, &m_stream);
			if (!m_stream)
				throw std::runtime_error("gui-str.show-picture-error");

			//show picture
			m_lpmyImage = new Gdiplus::Image(m_stream);

			//看看图片大小和显示窗口大小
			m_picWidth = m_lpmyImage->GetWidth();
			m_picHeight = m_lpmyImage->GetHeight();

			break;

		default:
			throw std::runtime_error("gui-str.show-picture-unknown-type");
		}
	}
	catch (std::exception ex)
	{
		MessageBoxA(NULL, ex.what(), "gui-str.error", MB_OK | MB_ICONERROR);
		assert(FALSE);
		return FALSE;
	}

	return TRUE;
}

BOOL CShowPicture::DrawPictureOnDC()
{
	DrawBlockOnDlg();

	if(nullptr != m_MemDC){
		//用于将m_lpmyImage中的图像画到m_MemDC上
		std::unique_ptr<Gdiplus::Graphics> pGraph(new Gdiplus::Graphics(m_MemDC));
		if (nullptr != pGraph) {
			pGraph->DrawImage(m_lpmyImage, 0, 0, m_picWidth, m_picHeight);
		}
	}

	//以下在源代码中是当FMT_RAW == iFormat时执行的
	if (FMT_RAW == m_picFormat) {

		//不可以现在释放m_lpmyImage，保存为png时会用到
		if(NULL != m_lpmyImage)
			delete m_lpmyImage;
		m_lpmyImage = NULL;

		if (NULL != m_stream)
			m_stream->Release();
		m_stream = NULL;

		if (NULL != m_hGlobal)
			GlobalFree(m_hGlobal);
		m_hGlobal = NULL;
	}

	return TRUE;
}

//在窗口上先显示好透明背景的格子
BOOL CShowPicture::DrawBlockOnDlg()
{
	HDC pDC = ::GetDC(m_hWndShow);

	//随后建立与屏幕显示兼容的内存显示设备  
	m_MemDC = CreateCompatibleDC(NULL);
	//显示设备对象
	HDC MemDCTemp = CreateCompatibleDC(NULL);
	//这时还不能绘图，因为没有地方画
	//下面建立一个与屏幕显示兼容的位图，位图的大小可以用窗口的大小 
	m_MemBitmap = CreateCompatibleBitmap(pDC, m_picWidth, m_picHeight);
	//位图对象
	HBITMAP MemBitmapTemp = CreateCompatibleBitmap(pDC, 24, 8);

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

	for (int j = 0; j < m_picHeight; j += 8) {
		for (int i = 0; i < m_picWidth; i += 16) {
			if (j & 8)
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
