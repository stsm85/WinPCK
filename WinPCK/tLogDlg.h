#pragma once

#include "tlib.h"
#include "resource.h"

class TLogDlg : public TDlg
{
private:

	HWND	hWndList;
	wchar_t	szExePath[MAX_PATH];

	int			m_LogListCount = 0;

	wchar_t*	pszLogFileName();
	wchar_t*	pszTargetListLog(int iItem);

	int		m_iCurrentHotItem;
	const char	m_szLogPrefix[6] = { 'N', 'I', 'W', 'E', 'D', ' ' };

	int		log_level_char_to_int(const char level);
	void	_InsertLogIntoList(const char, const wchar_t *);

public:
	TLogDlg(TWin *_win) : TDlg(IDD_DIALOG_LOG, _win) { }

	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvCreate(LPARAM lParam);
	virtual BOOL	EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight);
	virtual BOOL	EvClose();
	virtual BOOL	EvNotify(UINT ctlID, NMHDR *pNmHdr);

	void	InsertLogToList(const char, const wchar_t *);
};
