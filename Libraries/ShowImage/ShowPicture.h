#pragma once

#define EPSILON 0.000001

#include "ShowPictureInterface.h"

class CShowPicture : public IShowPicture
{
public:
	CShowPicture();
	virtual ~CShowPicture();

	virtual bool	Attach(HWND hWndShow, LPBYTE _buffer, size_t _bufsize, LPCWSTR _lpszFileTitle = L"", PICFORMAT _picFormat = FMT_RAW) override;
	virtual void	Detach() override {};

	//返回图像的宽和高
	virtual uint32_t	GetWidth() const override;
	virtual uint32_t	GetHeight() const override;

	//
	virtual bool	IsDecoded() const override { return this->m_isDecoded; }

	void	SetHQUpscaleMode(bool isHQ = true)
	{
		this->m_upscale_mode = isHQ ? STRETCH_HALFTONE : STRETCH_DELETESCANS;
	}

#pragma region Window
public:
	virtual std::wstring	GetWindowTitle() const override;

#pragma endregion


#pragma region DrawPicture

protected:
	//在m_lpmyImage创建上创建图像image
	BOOL	BuildImage();
	//在Graphics图像上画图
	BOOL	DrawPictureOnDC();
	//在窗口上先显示好透明背景的格子
	BOOL	DrawBlockOnDlg();
#pragma endregion

public:

	//在设备上显示图像
	virtual bool	Paint(int64_t nXOriginDest, int64_t nYOriginDest, int nXOriginSrc, int nYOriginSrc, double dZoomRatio = 1.00, bool bErase = true) override;
private:

	//绘制图像
	void	DrawImage(HDC& hDC, RECT& rectWin, PAINTSTRUCT& ps, int64_t nXOriginDest, int64_t nYOriginDest, int nXOriginSrc, int nYOriginSrc, double dZoomRatio);

	//绘制无效区域
	void	RedrawInvalidRect(HDC& hDC, RECT& rectWin, int64_t nXOriginDest, int64_t nYOriginDest, double dZoomRatio);

#pragma region 保存图片
public:
	//保存显示的图片
	virtual bool	Save(const wchar_t* lpszFilename) override;
#pragma endregion

	//继承类不可访问


	//继承类可访问
protected:

	//Gdiplus
	Gdiplus::GdiplusStartupInput m_oGdiPlusStartupInput;
	ULONG_PTR	m_pGdiPlusToken;

	//图片类型dds,tga,other
	//enum { FMT_DDS, FMT_TGA, FMT_RAW} m_picFormat;
	PICFORMAT	m_picFormat;
	//输入的文件名，可以根据后缀判断类型
	LPCWSTR		m_lpszFileTitle;
	//绑定的用于显示的窗口的句柄
	HWND		m_hWndShow;
	//图片的宽和高
	UINT		m_picWidth = 0, m_picHeight = 0;
	//将要显示的图像会画到这个HDC上面
	HDC			m_MemDC = NULL;
	//
	HBITMAP		m_MemBitmap = NULL;
	//用于在窗口上显示图片的GDIPLUS对象
	Gdiplus::Image* m_lpmyImage = NULL;
	//类内部使用的原始的图片文件数据，当类型为RAW时，指向hGlobal的内存
	LPBYTE		m_resBufferInClass = nullptr;
	//用于显示bmp,jpg,png等图片，使用Gdiplus从hGlobal内存创建的IStream s，然后画图
	IStream* m_stream = NULL;
	HGLOBAL		m_hGlobal = NULL;
	//传入的文件的数据的大小
	UINT32		m_bufsize;
	//显示的图片的具体格式
	char		m_szPictureFormat[32] = { 0 };
	//已经解码的图片的DIB数据（DDS，TGA）
	LPBYTE		m_DecodedDIBData = NULL;

	//绘制背景
	HBRUSH		m_hBrushBack = GetSysColorBrush(COLOR_MENU);

	//是否成功加载图片
	bool		m_isDecoded = false;

	int m_upscale_mode = STRETCH_HALFTONE;

	//判断两个double相等
	BOOL	isEqual(double d1, double d2);

	//开始解码
	bool	Decode();

public:
	//缩放到窗口大小
	virtual void	ZoomToWindowSize() override {};

	//恢复原始分辨率
	virtual void	ZoomToOriginalResolution(int64_t pos_x, int64_t pos_y) override {};

	//使用鼠标滚轮进行缩放
	virtual void	ZoomWithMousePoint(uint32_t nFlags, int16_t zDelta, int64_t pos_x, int64_t pos_y) override {};

	//返回缩放率
	virtual double	GetZoomRatio() override {
		return 1;
	};

	//窗口大小变化后，要重新计算值
	virtual void	ChangeClientSize(uint16_t nWidth, uint16_t nHeight) override {};
	virtual double	CalcFixedRatioByClientSize(uint32_t uRealX, uint32_t uRealY, uint32_t uToX, uint32_t uToY) override { return 0; };
	//窗口初始化时，计算最佳值，如果图片大于分辨率，窗口就最大化，返回值为窗口是否需要最大化，需要返回TRUE
	virtual bool	CalcFixedRatioAndSizeOnInit(int64_t& _inout_cx, int64_t& _inout_cy, int _in_min_cx, int _in_min_cy) override { return false; };

	//移动图像
	virtual void	MovePicture(int xOffset, int yOffset) override {};

	//显示
	virtual void	Paint(bool bErase = true) override {};

	//双击功能
	virtual void	SetDbClickFunc(std::function<bool(void)> func) override {};
};

