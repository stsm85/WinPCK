
#ifndef _MISCDLG_H_
#define _MISCDLG_H_

#include "tlib.h"
#include "resource.h"
#include "miscpicdlg.h"

#include "PckXchgDef.h"

#include <Gdiplus.h>
using namespace Gdiplus;


class TLogDlg : public TDlg
{
protected:

	//LPPCK_RUNTIME_PARAMS	lpParams;
	HWND	hWndList;
	char	szExePath[MAX_PATH];

	char*	pszLogFileName();
	char*	pszTargetListLog(int iItem);

	int		m_iCurrentHotItem;


public:
	TLogDlg(TWin *_win) : TDlg(IDD_DIALOG_LOG, _win) { }
	
	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
	//virtual BOOL	EventScroll(UINT uMsg, int nCode, int nPos, HWND scrollBar);
	virtual BOOL	EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight);
	virtual BOOL	EvClose();
	virtual BOOL	EvNotify(UINT ctlID, NMHDR *pNmHdr);

	HWND	GetListWnd();
};



class TInfoDlg : public TDlg
{
protected:
	char	*dirBuf;

public:
	TInfoDlg(char *_dirBuf, TWin *_win) : TDlg(IDD_DIALOG_INFO, _win) { dirBuf = _dirBuf; }
	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
};

class TSearchDlg : public TDlg
{
protected:
	char	*dirBuf;

public:
	TSearchDlg(char *_dirBuf, TWin *_win) : TDlg(IDD_DIALOG_SEARCH, _win) { dirBuf = _dirBuf; }
	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
};

class TCompressOptDlg : public TDlg
{
protected:

	LPPCK_RUNTIME_PARAMS	lpParams;

public:
	TCompressOptDlg(LPPCK_RUNTIME_PARAMS in, TWin *_win) : TDlg(IDD_DIALOG_COMPRESS, _win) { lpParams = in; }
	
	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
	virtual BOOL	EventScroll(UINT uMsg, int nCode, int nPos, HWND scrollBar);
};

#define TEXT_TYPE_ANSI	0
#define TEXT_TYPE_UCS2	1
#define TEXT_TYPE_UTF8	2
#define TEXT_TYPE_RAW	3

#define	VIEW_TEXT_MAX_BUFFER	0x1400000	//20MB
//#define	VIEW_TEXT_MAX_BUFFER	200	//20MB
#define	VIEW_RAW_MAX_BUFFER		0x1000000	//16MB
//#define	VIEW_RAW_MAX_BUFFER		0x100000	//1MB

class TViewDlg : public TDlg
{
protected:
	char	**buf;
	char	*lpszTextShow;

	DWORD	dwSize;
	char	*lpszFile;

	int		textType;

	//BOOL	IsTextUTF8(char *inputStream, size_t size);
	int		DataType(const char *_s, size_t size);

	void	ShowRaw(LPBYTE lpbuf, size_t rawlength);

public:
	TViewDlg(char **_buf, DWORD &_dwSize, char *_lpszFile, TWin *_win);
	//TViewDlg(BOOL ((*_GetSingleFileData)(LPVOID, LPVOID, char *, size_t)), LPVOID _lpPckFileIndexToShow, DWORD _dwSize, char *_lpszFile, TWin *_win);


	~TViewDlg();

	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
};



class TAttrDlg : public TDlg
{
protected:
	void	*lpPckInfo;
	void	*lpRootInfo;

	char	*lpszPath;
	BOOL	isSearchMode;

	QWORD	qwPckFileSize;

public:
	TAttrDlg(void *_lpPckInfo, void *_lpRootInfo, QWORD _qwPckFileSize, char *_lpszPath, BOOL _isSearchMode, TWin *_win);
	//~TAttrDlg();

	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
};


////////////////////////////////////图像显示模块//////////////////////////////////////////

//#define DBG_DISABLE_ALPHA

class TPicDlg : public TDlg
{
protected:

	GdiplusStartupInput g_oGdiPlusStartupInput;
	ULONG_PTR	g_pGdiPlusToken;

	//窗口文字
	char szTitle[MAX_PATH];

	//image
	Graphics	*lpoGraph;
	Image		*lpmyImage;
	IStream*	s;
	HGLOBAL		hGlobal;

	//char		*bmpbufferWithoutBackground;

	//UINT32		dwDdsToBmpFileSize;
	UINT32		dwDdsToBmpImageDataSize;
	char		szDdsFormat[32];

	UINT		picWidth;
	UINT		picHeight;
	UINT		bytesPerLine;
	UINT		bytesPerPixel;
	UINT		stride;		//align 4 

	char		**buf;
	char		*cleanimage;
	//BOOL		hasAlpha;

	

	char		*lpszFile;
	UINT32		dwSize;
	int			iFormat;

	INT			x, y;
	POINT		pointMouse;
	BOOL		isMouseDown;


	//HBRUSH				hbEditBack;
	HDC			MemDC;
	HBITMAP		MemBitmap;
	//HBITMAP		pOldBit;


	BOOL		DetectDIBFormatAndAllocByFormat(UINT16 wWidth, UINT16 wHeight, ...);
	//BOOL		FormatBmpData(UINT16 wWidth, UINT16 wHeight, UINT32 &dwPixels);

	BOOL		CreateBmpBufferByDds();
	BOOL		CreateBmpBufferByTga();

//_inline VOID	CalcRgbs(CMYRGB cRGB[]);
//UINT32	MakeRgb(UINT32 dwBackColor, UINT32 dwSource, 
//						UINT32 dwABitMask, UINT32 dwRBitMask, UINT32 dwGBitMask, UINT32 dwBBitMask, 
//						BYTE abit, BYTE rbit, BYTE gbit, BYTE bbit, 
//						BYTE ashift, BYTE rshift, BYTE gshift, BYTE bshift,
//						BOOL hasAlpha);
//_inline UINT32	MakeRgbWithAlpha(UINT32 dwBackColor, UINT32 dwSrc);
//_inline UINT64	MakeRgbWithAlphaSSE2(UINT32 *dwBackColor, UINT32 *dwSrc);

	//void	MakeRgbWithAlphaAll(BOOL hasAlpha);

	BOOL	SaveFile(WCHAR * lpszFileName);
	//static VOID	SavePng(VOID *pParam);
	//BOOL	PrepareToSavePng(char *lpFile);

	//void	FlipBitmap();


	//DDS decoder
	PixelFormat	decodedDIBFormat;

	void	decode_dds_dxt1(BYTE *ddsimage);
	void	decode_dds_dxt3(BYTE *ddsimage);
	void	decode_dds_dxt5(BYTE *ddsimage);
	void	decode_dds_a8r8g8b8(BYTE *ddsimage);
	void	decode_dds_x8r8g8b8(BYTE *ddsimage);
	void	decode_dds_a8b8g8r8(BYTE *ddsimage);
	void	decode_dds_x8b8g8r8(BYTE *ddsimage);
	void	decode_dds_r8g8b8(BYTE *ddsimage);
	void	decode_dds_a1r5g5b5(BYTE *ddsimage);
	void	decode_dds_x1r5g5b5(BYTE *ddsimage);
	void	decode_dds_r5g6b5(BYTE *ddsimage);
	void	decode_dds_a4r4g4b4(BYTE *ddsimage);
	void	decode_dds_x4r4g4b4(BYTE *ddsimage);
	void	decode_dds_a8r3g3b2(BYTE *ddsimage);
	void	decode_dds_a2r10g10b10(BYTE *ddsimage);
	void	decode_dds_a2b10g10r10(BYTE *ddsimage);

	//TGA decoder
	BOOL	makeTgaColorMappedData(BYTE * &tgaimage, int fmt, char * &bufferOfColorMappedData, UINT16 wColorTableSize/*, int nTgaBitsPerPixel*/);
	void	decode_tga_ColorMapped8(BYTE *tgaimage, char *lpColorMappedData);
	void	decode_tga_ColorMapped16(BYTE *tgaimage, char *lpColorMappedData);

	void	decode_tga_RGB(BYTE *tgaimage);

	void	decode_tga_ColorMapped8REL(BYTE *tgaimage, char *lpColorMappedData);
	void	decode_tga_ColorMapped16REL(BYTE *tgaimage, char *lpColorMappedData);

	void	decode_tga_RGBREL(BYTE *tgaimage);


	//MEMDC
	void	DrawBlockOnDlg();


public:
	TPicDlg(char **_buf, UINT32 _dwSize, int _iFormat, char *_lpszFile, TWin *_win);
	~TPicDlg();

	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
	//virtual BOOL	EvDrawItem(UINT ctlID, DRAWITEMSTRUCT *lpDis);
	virtual BOOL	EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight);
	virtual BOOL	EventScroll(UINT uMsg, int nCode, int nPos, HWND scrollBar);
	virtual BOOL	EvMouseMove(UINT fwKeys, POINTS pos);
	virtual BOOL	EventButton(UINT uMsg, int nHitTest, POINTS pos);
	//virtual BOOL	EvTimer(WPARAM timerID, TIMERPROC proc);
	//virtual BOOL	EventCtlColor(UINT uMsg, HDC hDcCtl, HWND hWndCtl, HBRUSH *result);

	virtual BOOL	EvPaint(void);
	//virtual BOOL	EvClose();

};


#endif
