#pragma once

#define EPSILON 0.000001

#include "DdsTgaDecoderDefine.h"
#include <Gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")

class CShowPicture
{
public:
	CShowPicture(HWND hWndShow, LPBYTE &_buffer, size_t _bufsize, LPCWSTR _lpszFileTitle, PICFORMAT _picFormat);
	~CShowPicture();

	//返回图像的宽和高
	UINT	GetWidth();
	UINT	GetHeight();

	//开始解码
	BOOL	Decode();
	LPCWSTR	GetWindowTitle(LPWSTR	lpszTitle = NULL, size_t bufSize = 0);

	//在设备上显示图像
	BOOL	Paint(__int64 nXOriginDest, __int64 nYOriginDest, int nXOriginSrc, int nYOriginSrc, double dZoomRatio = 1.00);

	//保存显示的图片
	BOOL	Save(const WCHAR *lpszFilename);

//继承类不可访问
private:

//继承类可访问
protected:

	//Gdiplus
	GdiplusStartupInput m_oGdiPlusStartupInput;
	ULONG_PTR	m_pGdiPlusToken;

	//图片类型dds,tga,other
	//enum { FMT_DDS, FMT_TGA, FMT_RAW} m_picFormat;
	PICFORMAT	m_picFormat;
	//输入的文件名，可以根据后缀判断类型
	LPCWSTR		m_lpszFileTitle;
	//绑定的用于显示的窗口的句柄
	HWND		m_hWndShow;
	//图片的宽和高
	UINT		m_picWidth, m_picHeight;
	//将要显示的图像会画到这个HDC上面
	HDC			m_MemDC;
	//
	HBITMAP		m_MemBitmap;
	//用于在窗口上显示图片的GDIPLUS对象
	Image		*m_lpmyImage;
	//用于将m_lpmyImage中的图像画到m_MemDC上
	Graphics	*m_lpoGraph;
	//原始的图片文件数据
	//LPBYTE		m_resBuffer;
	//类内部使用的原始的图片文件数据，当类型为RAW时，指向hGlobal的内存
	LPBYTE		m_resBufferInClass;
	//用于显示bmp,jpg,png等图片，使用Gdiplus从hGlobal内存创建的IStream s，然后画图
	IStream*	m_stream;
	HGLOBAL		m_hGlobal;
	//传入的文件的数据的大小
	UINT32		m_bufsize;
	//显示的图片的具体格式
	char		m_szPictureFormat[32];
	//已经解码的图片的DIB数据（DDS，TGA）
	LPBYTE		m_DecodedDIBData;


	//在窗口上先显示好透明背景的格子
	BOOL	DrawBlockOnDlg();
	//根据文件名判断类型
	//

	//在m_lpmyImage创建上创建图像image
	BOOL	BuildImage();
	//在Graphics图像上画图
	BOOL	DrawPictureOnDC();
	//更新目标窗口标题
	BOOL	ShowTitleOnWindow();

	//判断两个double相等
	BOOL	isEqual(double d1, double d2);
};

