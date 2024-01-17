//////////////////////////////////////////////////////////////////////
// tPicDlg.cpp: WinPCK 界面线程部分
// 对话框代码
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2017.12.26
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4018 )
#include "guipch.h"

#include "tPreviewDlg.h"

#define MIN_CX	640
#define MIN_CY	480

#define TIMER_TITLE (WM_USER + 1)

TPicDlg::TPicDlg(LPBYTE *_buf, UINT32 _dwSize, PICFORMAT _iFormat, const wchar_t *_lpszFile, TWin *_win) :
	TDlg(IDD_DIALOG_PIC, _win),
	buf(_buf),
	dwSize(_dwSize),
	lpszFile(_lpszFile),
	iFormat(_iFormat)
{ }

TPicDlg::~TPicDlg()
{}

BOOL TPicDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	switch(wID) {
	case IDOK:
	case IDCANCEL:

		this->KillTimer(TIMER_TITLE);

		EndDialog(wID);
		return	TRUE;

	}
	return	FALSE;
}

bool TPicDlg::SaveFile()
{
	//导出
	if(FMT_RAW != this->iFormat) {

		auto filepath = fs::current_path() / this->lpszFile;
		filepath.replace_extension(".png");		

		auto ext = ::MakeFileExtFilter<wchar_t>({ { L"PNG文件(*.png)", L"*.png" } });
		auto szFilename = filepath.wstring();

		try {
			::SaveFile<wchar_t>(this->hWnd, szFilename, std::wstring(L"png"), ext);
			return lpShowPicture->Save(szFilename.c_str());
		}
		catch (std::exception)
		{
			return true;
		}
	}
	
	return true;
}

void TPicDlg::InitFixedShowPositionAndShowWindow()
{

	int64_t cx, cy;
	BOOL bNeedShowMax = this->lpShowPicture->CalcFixedRatioAndSizeOnInit(cx, cy, MIN_CX, MIN_CY);

	this->FreshWindowTitle();

	if(bNeedShowMax) {

		this->ShowWindow(SW_SHOWMAXIMIZED);
	} else {

		::SetWindowPos(hWnd, NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE);
		//显示窗口
		this->Show();
	}
}

void TPicDlg::FreshWindowTitle()
{
	this->SetWindowTextW(std::format(L"{} ({}%)", this->m_szTitle, (int)(lpShowPicture->GetZoomRatio() * 100.0 + 0.5)).c_str());
}

BOOL TPicDlg::EvCreate(LPARAM lParam)
{

	this->lpShowPicture = MakeZoomShowPictureInstance();

	if(!this->lpShowPicture->Attach(this->GetDlgItem(IDC_PIC), *buf, dwSize, lpszFile, iFormat))
		return FALSE;

	this->lpShowPicture->SetDbClickFunc(std::bind(&TPicDlg::SaveFile, this));

	this->m_szTitle = this->lpShowPicture->GetWindowTitle();

	this->InitFixedShowPositionAndShowWindow();

	this->SetTimer(TIMER_TITLE, 200, nullptr);

	return	FALSE;
}

BOOL TPicDlg::EvTimer(WPARAM timerID, TIMERPROC proc)
{
	switch (timerID)
	{
	case TIMER_TITLE:
		this->FreshWindowTitle();
		break;
	}
	return	FALSE;
}

BOOL TPicDlg::EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight)
{
	if (nullptr != this->lpShowPicture) {
		
		::MoveWindow(this->GetDlgItem(IDC_PIC), 0, 0, nWidth, nHeight, TRUE);
	}

#ifdef _DEBUG
	//::OutputDebugStringA(std::format("func:{}, nWidth={}, nHeight={}\r\n", __FUNCTION__, nWidth, nHeight).c_str());
#endif

	return	FALSE;
}

BOOL TPicDlg::EventButton(UINT uMsg, int nHitTest, POINTS pos)
{
	switch(uMsg) {
	case WM_LBUTTONDBLCLK:
		//导出
		this->SaveFile();
		break;

	}
	return FALSE;
}
