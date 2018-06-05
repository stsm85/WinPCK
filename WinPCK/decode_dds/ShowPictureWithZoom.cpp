//////////////////////////////////////////////////////////////////////
// ShowPictureWithZoom.cpp: 显示缩放的dds、tga图像
// 将解码的dds、tga图像进行缩放后进行显示
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2018.5.29
//////////////////////////////////////////////////////////////////////

#include "ShowPictureWithZoom.h"

#define CHANGE_RATIO	1.2
#define MAX_ZOOM_RATIO	32.0

CShowPictureWithZoom::CShowPictureWithZoom(HWND hWndShow, LPBYTE &_buffer, size_t _bufsize, LPCWSTR _lpszFileTitle, PICFORMAT _picFormat) : 
	CShowPicture( hWndShow, _buffer,  _bufsize, _lpszFileTitle, _picFormat),
	m_fixedZoomRatio(1.0),
	m_dZoomRatioShow(1.0),
	m_dZoomRatio(1.0),
	m_ShowX(0),
	m_ShowY(0)
{
}


CShowPictureWithZoom::~CShowPictureWithZoom()
{}

double CShowPictureWithZoom::GetZoomRatio()
{
	return m_dZoomRatioShow;
}

void CalcZoomPictureAtOneDimensional(int iClientPointXY, __int64 &_showXY, UINT64 &_picWidthHeight, double _dZoomRatio, UINT _RealPicWidthHeight, LONG	lClientWidthHeight)
{
	/*
	计算m_ShowX,m_ShowY
	当前显示大小大于窗口时：
	缩放以当前鼠标位置为中心缩放
	假设鼠标的点在图片上的位置为rx,ry, 鼠标在窗口上的位置为mx,my，图片0,0在窗口上的显示点为sx,sy，则rx=mx-sx
	缩放以当前鼠标位置为中心缩放，则鼠标在图片上的点不动，就是向量值不动，设x方向上的向量值为p,设当前图片宽度为sw，则p=rx/sw=(mx-sx)/sw
	缩放后图片宽度变化为sw1,显示点变化为sx1，p不变，p=(mx-sx1)/sw1，得到sx1 = mx - (mx-sx)*sw1/sw
	*/

	int mouseXY = iClientPointXY;
	//当鼠标点在图像之外时，以图像边界为准
	if(_showXY > mouseXY)mouseXY = _showXY;
	__int64 iShowRightBottom = _showXY + _picWidthHeight;
	if(iShowRightBottom < mouseXY)mouseXY = iShowRightBottom;

	//计算缩放后的m_ShowX, m_ShowY
	UINT64	dwShowPicWidthHeight = _dZoomRatio * _RealPicWidthHeight;

	//当显示的图片宽度大于当前窗口时

	if(lClientWidthHeight < dwShowPicWidthHeight) {
		_showXY = mouseXY - (mouseXY - _showXY) * dwShowPicWidthHeight / _picWidthHeight;
		if(0 < _showXY)_showXY = 0;
	} else {

		_showXY = (lClientWidthHeight - dwShowPicWidthHeight) / 2;
	}

	_picWidthHeight = dwShowPicWidthHeight;
}

void CShowPictureWithZoom::ZoomPictureAtPoint(double dZoomRatio, __int64 pos_x, __int64 pos_y)
{

	//分别计算X和Y方向上以当前点进行缩放的结果

	RECT	rectWin;
	GetClientRect(m_hWndShow, &rectWin);

	CalcZoomPictureAtOneDimensional(pos_x, m_ShowX, m_ShowPicWidth, dZoomRatio, m_picWidth, rectWin.right);
	CalcZoomPictureAtOneDimensional(pos_y, m_ShowY, m_ShowPicHeight, dZoomRatio, m_picHeight, rectWin.bottom);

	Paint();
}

void CShowPictureWithZoom::ZoomWithMousePoint(UINT nFlags, short zDelta, __int64 pos_x, __int64 pos_y)
{

	//最大放大倍数为8倍，最小为适应当前窗口大小或1.0
	double dZoomChangeRatio = 1.0;
	double dNextZoomRatio;
	//放大
	if(0 < zDelta) {

		dZoomChangeRatio *= (zDelta / 120.0 * CHANGE_RATIO);

		if((MAX_ZOOM_RATIO + EPSILON) < m_dZoomRatio)
			return;

		 dNextZoomRatio = m_dZoomRatio * dZoomChangeRatio;

		if(dNextZoomRatio > MAX_ZOOM_RATIO) {

			m_dZoomRatioShow = MAX_ZOOM_RATIO;
		} else {

			m_dZoomRatioShow = dNextZoomRatio;
		}

		//缩小
	} else if(0 > zDelta) {
		dZoomChangeRatio /= (zDelta / (-120.0) * CHANGE_RATIO);

		//限制缩放范围
		double dMinZoomRatio = (m_fixedZoomRatio > (1.0 - EPSILON)) ? 1.0 : m_fixedZoomRatio;

		if(dMinZoomRatio > (m_dZoomRatio + EPSILON))
			return;

		 dNextZoomRatio = m_dZoomRatio * dZoomChangeRatio;

		if(dNextZoomRatio < dMinZoomRatio) {

			m_dZoomRatioShow = dMinZoomRatio;
		} else {

			m_dZoomRatioShow = dNextZoomRatio;
		}

	}

	m_dZoomRatio = dNextZoomRatio;

	ZoomPictureAtPoint(m_dZoomRatioShow, pos_x, pos_y);
}

void CShowPictureWithZoom::ZoomToOriginalResolution(__int64 pos_x, __int64 pos_y)
{
	m_dZoomRatioShow = m_dZoomRatio = 1.0;
	ZoomPictureAtPoint(1.0, pos_x, pos_y);
}


void CShowPictureWithZoom::MovePicture(int xOffset, int yOffset)
{

	RECT	rectWin;
	GetClientRect(m_hWndShow, &rectWin);

	//当显示的图片宽度大于当前窗口时
	if(rectWin.right < m_ShowPicWidth) {

		int min_x = (int)rectWin.right - (int)m_ShowPicWidth;

		//当显示区域大于窗口时，图片左上角不能大于0
		//鼠标向左动
		if(xOffset > 0) {

			if(min_x <= m_ShowX) {
				m_ShowX -= xOffset;
				if(m_ShowX < min_x) m_ShowX = min_x;
			}
		} else {

			if(0 >= m_ShowX) {
				m_ShowX -= xOffset;
				if(m_ShowX > 0)m_ShowX = 0;
			}
		}

	}

	//当显示的图片高度大于当前窗口时
	if(rectWin.bottom < m_ShowPicHeight) {

		int min_y = (int)rectWin.bottom - (int)m_ShowPicHeight;

		//鼠标向上动
		if(yOffset > 0) {

			if(min_y <= m_ShowY) {
				m_ShowY -= yOffset;
				if(m_ShowY < min_y) m_ShowY = min_y;
			}
		} else {

			if(0 >= m_ShowY) {
				m_ShowY -= yOffset;
				if(m_ShowY > 0)m_ShowY = 0;
			}
		}

	}

	Paint();
}


void CShowPictureWithZoom::CalcMinZoomRatio()
{
	double dZoomRatio;

	for(dZoomRatio = 1.0; dZoomRatio >= m_dZoomRatioShow; dZoomRatio /= CHANGE_RATIO) {
	}

	m_dZoomRatio = dZoomRatio;
}

BOOL CShowPictureWithZoom::CalcFixedRatioAndSizeOnInit(__int64 &_inout_cx, __int64 &_inout_cy, int _in_min_cx, int _in_min_cy)
{

	//当前分辨率
	const int width = GetSystemMetrics(SM_CXSCREEN);
	const int height = GetSystemMetrics(SM_CYSCREEN);

	double screenx_d_realx = width / (double)m_picWidth;
	double screeny_d_realy = height / (double)m_picHeight;

	BOOL bNeedShowMax = TRUE;
	//比较screenx_d_realx和screeny_d_realy，哪个小说明哪个边先越界
	if(screenx_d_realx < screeny_d_realy) {

		if(width < m_picWidth) {
			m_dZoomRatio = screenx_d_realx;

		} else {
			bNeedShowMax = FALSE;

		}
	} else {

		if(height < m_picHeight) {
			m_dZoomRatio = screeny_d_realy;

		} else {
			bNeedShowMax = FALSE;

		}
	}
	m_dZoomRatioShow = m_dZoomRatio;

	if((1.0 - EPSILON) > m_dZoomRatioShow) {
		CalcMinZoomRatio();
	}


	m_ShowPicWidth = m_dZoomRatioShow * m_picWidth;
	m_ShowPicHeight = m_dZoomRatioShow * m_picHeight;

	if(bNeedShowMax) {
		return TRUE;
	} else {
		_inout_cx = (m_picWidth > _in_min_cx) ? m_picWidth + 10 : _in_min_cx + 10;
		_inout_cy = (m_picHeight > _in_min_cy) ? m_picHeight + 30 : _in_min_cy + 30;
		return FALSE;
	}

}



double CShowPictureWithZoom::CalcFixedRatioByClientSize(UINT uRealX, UINT uRealY, UINT uToX, UINT uToY)
{
	//计算一下多大是正好的缩放率
	double zoomx = uToX / (double)uRealX;
	double zoomy = uToX / (double)uRealX;

	return zoomx > zoomy ? zoomy : zoomx;
}

void CShowPictureWithZoom::ChangeClientSize(WORD nWidth, WORD nHeight)
{
	m_ShowX = ((int)nWidth - (int)m_ShowPicWidth) / 2;
	m_ShowY = ((int)nHeight - (int)m_ShowPicHeight) / 2;

	//计算一下多大是正好的缩放率
	m_fixedZoomRatio = CalcFixedRatioByClientSize(m_picWidth, m_picHeight, nWidth, nHeight);

	Paint();
}

void CShowPictureWithZoom::Paint()
{
	CShowPicture::Paint(m_ShowX, m_ShowY, 0, 0, m_dZoomRatioShow);

	//计算无效区域
	//x
	RECT	rectWin;
	GetClientRect(m_hWndShow, &rectWin);

	if(m_ShowX > 0) {

		RECT rectInvalid = { 0, 0, m_ShowX, rectWin.bottom };
		InvalidateRect(m_hWndShow, &rectInvalid, TRUE);
	}

	if(m_ShowY > 0) {

		RECT rectInvalid = { m_ShowX >0 ? m_ShowX : 0, 0, rectWin.right, m_ShowY };
		InvalidateRect(m_hWndShow, &rectInvalid, TRUE);
	}

	int iShowBottom = m_ShowY + m_ShowPicHeight;

	if(iShowBottom < rectWin.bottom) {

		RECT rectInvalid = { m_ShowX >0 ? m_ShowX : 0, iShowBottom, rectWin.right, rectWin.bottom };
		InvalidateRect(m_hWndShow, &rectInvalid, TRUE);
	}

	int iShowRight = m_ShowX + m_ShowPicWidth;
	if(iShowRight < rectWin.right) {

		RECT rectInvalid = { iShowRight, m_ShowY >0 ? m_ShowY : 0,rectWin.right, iShowBottom < rectWin.bottom ? iShowBottom : rectWin.bottom };
		InvalidateRect(m_hWndShow, &rectInvalid, TRUE);
	}
}