#pragma once

#include "ShowPicture.h"

class CShowPictureWithZoom;

class CShowPictureWithZoom :
	public CShowPicture
{
public:
	//CShowPictureWithZoom(HWND hWndShow, LPBYTE& _buffer, size_t _bufsize);
	//CShowPictureWithZoom(HWND hWndShow, LPBYTE &_buffer, size_t _bufsize, LPCWSTR _lpszFileTitle, PICFORMAT _picFormat);
	CShowPictureWithZoom();
	virtual ~CShowPictureWithZoom();

	virtual bool	Attach(HWND hWndShow, LPBYTE _buffer, size_t _bufsize, LPCWSTR _lpszFileTitle = L"", PICFORMAT _picFormat = FMT_RAW) override;
	virtual void	Detach() override;

#pragma region 缩放
public:

	//缩放到窗口大小
	virtual void	ZoomToWindowSize() override;

	//恢复原始分辨率
	virtual void	ZoomToOriginalResolution(int64_t pos_x, int64_t pos_y) override;

	//使用鼠标滚轮进行缩放
	virtual void	ZoomWithMousePoint(uint32_t nFlags, int16_t zDelta, int64_t pos_x, int64_t pos_y) override;

	//返回缩放率
	virtual double	GetZoomRatio() override;
protected:
	//在给定点进行缩放
	void	ZoomPictureAtPoint(double dZoomRatio, int64_t pos_x, int64_t pos_y);

	//计算缩放率在m_fixedZoomRatio以下的第一个数值
	void	CalcMinZoomRatio();
#pragma endregion

#pragma region 窗口大小关联
public:
	//窗口大小变化后，要重新计算值
	virtual void	ChangeClientSize(uint16_t nWidth, uint16_t nHeight) override;
	virtual double	CalcFixedRatioByClientSize(uint32_t uRealX, uint32_t uRealY, uint32_t uToX, uint32_t uToY) override;
	//窗口初始化时，计算最佳值，如果图片大于分辨率，窗口就最大化，返回值为窗口是否需要最大化，需要返回TRUE
	virtual bool	CalcFixedRatioAndSizeOnInit(int64_t &_inout_cx, int64_t&_inout_cy, int _in_min_cx, int _in_min_cy) override;

#pragma endregion

#pragma region 显示图形消息函数
public:


	//OldMsgProc = (WNDPROC)SetWindowLong(g_arrHwnd[ID_EDIT_CRC32], GWL_WNDPROC, (LONG)EditProc);
	static LRESULT CALLBACK WinProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	//static std::map<HWND, WNDINFO> sm_WinClassMap;
	WNDPROC m_OldMsgProc = nullptr;
#pragma endregion



public:
	//移动图像
	virtual void	MovePicture(int xOffset, int yOffset) override;

	//显示
	virtual void	Paint(bool bErase = true) override;

	//双击功能
private:
	std::function<bool(void)> m_dbClickFunc = nullptr;
public:
	virtual void	SetDbClickFunc(std::function<bool(void)> func) override {
		this->m_dbClickFunc = func;
	};

private:
	BOOL	EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight);
	BOOL	EventButton(UINT uMsg, int nHitTest, POINTS pos);
	BOOL	EvMouseWheel(UINT nFlags, short zDelta, POINTS pos);
	BOOL	EvMouseMove(UINT fwKeys, POINTS pos);
	BOOL	EvPaint(void);

	//显示相关参数
	bool isMouseDown = false;
	//记录当前鼠标位置
	POINT			pointMouse{ 0 };

protected:


	//HWND m_hWndShow;

	//double		m_dFixedRatio = 1.0;
	double		m_dZoomRatioShow = 1.0;	//当前正在显示的缩放率
	double		m_dZoomRatio = 1.0;		//当前图片缩放率，当小于m_fixedZoomRatio时m_dZoomRatioShow等于m_fixedZoomRatio，其他情况m_dZoomRatioShow = m_dZoomRatio
	//适应当前窗口大小 的缩放率
	double		m_fixedZoomRatio = 1.0;

	//当前显示坐标，以窗口左上角为0,0
	int64_t		m_ShowX = 0, m_ShowY = 0;
	//当前显示大小
	uint64_t	m_ShowPicWidth = 0;
	uint64_t	m_ShowPicHeight = 0;

};
