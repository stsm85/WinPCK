#pragma once
#include "tlib.h"
#include "resource.h"
#include "CShowPictureWithZoom.h"
#include "PckHeader.h"


class CPriviewInDlg
{
public:
	CPriviewInDlg();
	~CPriviewInDlg();

	BOOL Show(LPCTSTR lpszFilename, DWORD dwSize, CPckControlCenter	*m_lpPckCenter, LPPCKINDEXTABLE lpPckFileIndexToShow, TWin *_win);

protected:

	//将要显示的对话框
	TDlg *	dlg;

	void *	m_buffer;
	DWORD	m_buffersize;

	PICFORMAT	GetPicFormatFromFilename(LPCTSTR lpszFilename);
	BOOL		AllocBuffer(PICFORMAT fmt, DWORD dwSize);
};


#pragma region TViewDlg

#define TEXT_TYPE_ANSI	0
#define TEXT_TYPE_UCS2	1
#define TEXT_TYPE_UTF8	2
#define TEXT_TYPE_RAW	3

#define	VIEW_TEXT_MAX_BUFFER	(16*1024*1024)	//16MB
#define	VIEW_RAW_MAX_BUFFER		(16*1024*1024)	//16MB

class TViewDlg : public TDlg
{
protected:
	LPBYTE	*buf;
	char	*lpszTextShow;

	DWORD	dwSize;
	const TCHAR	*lpszFile;

	int		textType;

	//BOOL	IsTextUTF8(char *inputStream, size_t size);
	int		DataType(const char *_s, size_t size);

	void	ShowRaw(LPBYTE lpbuf, size_t rawlength);

public:
	TViewDlg(LPBYTE *_buf, DWORD _dwSize, const TCHAR *_lpszFile, TWin *_win);
	~TViewDlg();

	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
};
#pragma endregion

#pragma region TPicDlg

//#define DBG_DISABLE_ALPHA

class TPicDlg : public TDlg
{
protected:

	//窗口标题
	TCHAR			m_szTitle[MAX_PATH];

	//待显示图片数据和大小
	LPBYTE			*buf;
	UINT32			dwSize;

	//文件名
	const TCHAR		*lpszFile;
	
	//格式
	PICFORMAT		iFormat;

	//记录当前鼠标位置
	POINT			pointMouse;
	BOOL			isMouseDown;

	CShowPictureWithZoom *lpShowPicture;

	BOOL	SaveFile();

	void	InitFixedShowPositionAndShowWindow();

	void	FreshWindowTitle();


public:
	TPicDlg(LPBYTE *_buf, UINT32 _dwSize, PICFORMAT _iFormat, const TCHAR *_lpszFile, TWin *_win);
	~TPicDlg();

	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
	//virtual BOOL	EvDrawItem(UINT ctlID, DRAWITEMSTRUCT *lpDis);
	virtual BOOL	EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight);
	//virtual BOOL	EventScroll(UINT uMsg, int nCode, int nPos, HWND scrollBar);
	virtual BOOL	EvMouseMove(UINT fwKeys, POINTS pos);
	virtual BOOL	EvMouseWheel(UINT nFlags, short zDelta, POINTS pos);
	virtual BOOL	EventButton(UINT uMsg, int nHitTest, POINTS pos);
	//virtual BOOL	EvTimer(WPARAM timerID, TIMERPROC proc);
	//virtual BOOL	EventCtlColor(UINT uMsg, HDC hDcCtl, HWND hWndCtl, HBRUSH *result);

	virtual BOOL	EvPaint(void);
	//virtual BOOL	EvClose();

};
#pragma endregion