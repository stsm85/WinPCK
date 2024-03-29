#pragma once
//#include "tlib.h"
//#include "resource.h"
//#include "ShowPictureWithZoom.h"
#include <ShowPictureInterface.h>
//#include "pck_handle.h"

class CPriviewInDlg
{
public:
	CPriviewInDlg();
	~CPriviewInDlg();

	BOOL Show(const PCK_UNIFIED_FILE_ENTRY* const lpPckFileIndexToShow, TWin *_win);

protected:

	//将要显示的对话框
	TDlg *	dlg;

	void *	m_buffer;
	DWORD	m_buffersize;

	PICFORMAT	GetPicFormatFromFilename(LPCWSTR lpszFilename);
	BOOL		AllocBuffer(PICFORMAT fmt, DWORD dwSize);
};


#pragma region TViewDlg

#define	VIEW_TEXT_MAX_BUFFER	(16*1024*1024)	//16MB
#define	VIEW_RAW_MAX_BUFFER		(16*1024*1024)	//16MB

class TViewDlg : public TDlg
{
protected:
	LPBYTE	*buf;
	const char	*lpszTextShow;

	DWORD	dwSize;
	const wchar_t	*lpszFile;

	TextEncoding::EncodingType textType;

	void	ShowRaw(LPBYTE lpbuf, size_t rawlength);

public:
	TViewDlg(LPBYTE *_buf, DWORD _dwSize, const wchar_t *_lpszFile, TWin *_win);
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
	std::wstring	m_szTitle;

	//待显示图片数据和大小
	LPBYTE			*buf;
	UINT32			dwSize;

	//文件名
	const wchar_t		*lpszFile;
	
	//格式
	PICFORMAT		iFormat;

	//记录当前鼠标位置
	//POINT			pointMouse;
	//BOOL			isMouseDown;

	std::unique_ptr<IShowPicture> lpShowPicture;

	bool	SaveFile();

	void	InitFixedShowPositionAndShowWindow();

	void	FreshWindowTitle();


public:
	TPicDlg(LPBYTE *_buf, UINT32 _dwSize, PICFORMAT _iFormat, const wchar_t *_lpszFile, TWin *_win);
	~TPicDlg();

	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
	virtual BOOL	EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight);
	virtual BOOL	EventButton(UINT uMsg, int nHitTest, POINTS pos);
	virtual BOOL	EvTimer(WPARAM timerID, TIMERPROC proc);

};
#pragma endregion