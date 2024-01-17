#pragma once

#include "dds/DdsTgaDecoderDefine.h"
#pragma comment(lib,"gdiplus.lib")
#include <functional>

class IShowPicture
{
public:
	//IShowPicture() {};
	virtual ~IShowPicture() = default;

	virtual bool	Attach(HWND hWndShow, uint8_t* _buffer, size_t _bufsize, const wchar_t* _lpszFileTitle = L"", PICFORMAT _picFormat = FMT_RAW) = 0;
	virtual void	Detach() = 0;

	//返回图像的宽和高
	virtual uint32_t	GetWidth() const = 0;
	virtual uint32_t	GetHeight() const = 0;

	//
	virtual bool	IsDecoded() const = 0;

	virtual std::wstring	GetWindowTitle() const = 0;

	//在设备上显示图像
	virtual bool	Paint(int64_t nXOriginDest, int64_t nYOriginDest, int nXOriginSrc, int nYOriginSrc, double dZoomRatio = 1.00, bool bErase = true) = 0;
	//保存显示的图片
	virtual bool	Save(const wchar_t* lpszFilename) = 0;

	//缩放到窗口大小
	virtual void	ZoomToWindowSize() = 0;

	//恢复原始分辨率
	virtual void	ZoomToOriginalResolution(int64_t pos_x, int64_t pos_y) = 0;

	//使用鼠标滚轮进行缩放
	virtual void	ZoomWithMousePoint(uint32_t nFlags, int16_t zDelta, int64_t pos_x, int64_t pos_y) = 0;

	//返回缩放率
	virtual double	GetZoomRatio() = 0;

	//窗口大小变化后，要重新计算值
	virtual void	ChangeClientSize(uint16_t nWidth, uint16_t nHeight) = 0;
	virtual double	CalcFixedRatioByClientSize(uint32_t uRealX, uint32_t uRealY, uint32_t uToX, uint32_t uToY) = 0;
	//窗口初始化时，计算最佳值，如果图片大于分辨率，窗口就最大化，返回值为窗口是否需要最大化，需要返回TRUE
	virtual bool	CalcFixedRatioAndSizeOnInit(int64_t& _inout_cx, int64_t& _inout_cy, int _in_min_cx, int _in_min_cy) = 0;


	//移动图像
	virtual void	MovePicture(int xOffset, int yOffset) = 0;

	//显示
	virtual void	Paint(bool bErase = true) = 0;

	//双击功能
	virtual void	SetDbClickFunc(std::function<bool(void)> func) = 0;
};

std::unique_ptr<IShowPicture> MakeShowPictureInstance();
std::unique_ptr<IShowPicture> MakeZoomShowPictureInstance();
