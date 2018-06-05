#pragma once
#include "ShowPicture.h"
class CShowPictureWithZoom :
	public CShowPicture
{
public:
	CShowPictureWithZoom(HWND hWndShow, LPBYTE &_buffer, size_t _bufsize, LPCWSTR _lpszFileTitle, PICFORMAT _picFormat);
	virtual ~CShowPictureWithZoom();

	//恢复原始分辨率
	void	ZoomToOriginalResolution(__int64 pos_x, __int64 pos_y);

	void	ZoomWithMousePoint(UINT nFlags, short zDelta, __int64 pos_x, __int64 pos_y);

	//窗口大小变化后，要重新计算值
	void	ChangeClientSize(WORD nWidth, WORD nHeight);
	double	CalcFixedRatioByClientSize(UINT uRealX, UINT uRealY, UINT uToX, UINT uToY);
	//窗口初始化时，计算最佳值，如果图片大于分辨率，窗口就最大化，返回值为窗口是否需要最大化，需要返回TRUE
	BOOL	CalcFixedRatioAndSizeOnInit(__int64 &_inout_cx, __int64 &_inout_cy, int _in_min_cx, int _in_min_cy);


	//返回缩放率
	double	GetZoomRatio();

	//移动图像
	void	MovePicture(int xOffset, int yOffset);

	//显示
	void	Paint();


protected:


	double		m_dZoomRatioShow;	//当前正在显示的缩放率
	double		m_dZoomRatio;		//当前图片缩放率，当小于m_fixedZoomRatio时m_dZoomRatioShow等于m_fixedZoomRatio，其他情况m_dZoomRatioShow = m_dZoomRatio
	//适应当前窗口大小 的缩放率
	double		m_fixedZoomRatio;


	//当前显示坐标，以窗口左上角为0,0
	__int64		m_ShowX, m_ShowY;
	//当前显示大小
	UINT64		m_ShowPicWidth;
	UINT64		m_ShowPicHeight;

	//在给定点进行缩放
	void	ZoomPictureAtPoint(double dZoomRatio, __int64 pos_x, __int64 pos_y);

	//计算缩放率在m_fixedZoomRatio以下的第一个数值
	void	CalcMinZoomRatio();

};
