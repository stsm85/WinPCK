#include "CRichEdit.h"

void CRichEdit::Attach(int id, HWND hDlg)
{
	CEdit::Attach(id, hDlg);
	auto cf = this->GetDefaultCharFormat();

	memcpy(&this->m_default_format, cf.get(), cf->cbSize);
	wcscpy(this->m_default_format.szFaceName, L"ËÎÌו");
	this->m_default_format.yHeight = 180;
	this->SetDefaultFormat();
}

void CRichEdit::RichTextMode(bool isRichText)
{
	if (isRichText)
		this->SendMsgA(EM_SETTEXTMODE, TM_RICHTEXT, 0);
	else
		this->SendMsgA(EM_SETTEXTMODE, TM_PLAINTEXT, 0);
}

void CRichEdit::SetSingleLevelUndo(bool SingleLevelUndo)
{
	if (SingleLevelUndo)
		this->SendMsgA(EM_SETTEXTMODE, TM_SINGLELEVELUNDO, 0);
	else
		this->SendMsgA(EM_SETTEXTMODE, TM_MULTILEVELUNDO, 0);
}

void CRichEdit::SetMultiCodepage(bool isMultiCodepage)
{
	if (isMultiCodepage)
		this->SendMsgA(EM_SETTEXTMODE, TM_MULTICODEPAGE, 0);
	else
		this->SendMsgA(EM_SETTEXTMODE, TM_SINGLECODEPAGE, 0);
}

//void CRichEdit::SetLimitText(int size)
//{
//	this->SendMsgA(EM_EXLIMITTEXT, size, 0);
//}

void CRichEdit::GetScrollPos(POINT* point)
{
	this->SendMsgA(EM_GETSCROLLPOS, 0, (LPARAM)point);
}

int CRichEdit::GetScrollPosX()
{
	POINT p{ 0 };
	this->GetScrollPos(&p);
	return p.x;
}

int CRichEdit::GetScrollPosY()
{
	POINT p{ 0 };
	this->GetScrollPos(&p);
	return p.y;
}

void CRichEdit::SetSelText(const char8_t* text)
{
	SETTEXTEX SetTxtEx{ .flags = ST_SELECTION | ST_UNICODE,.codepage = CP_UTF8 };
	this->SendMsgW(EM_SETTEXTEX, (WPARAM)&SetTxtEx, (LPARAM)text);
}

void CRichEdit::SetSelText(const wchar_t* text)
{
	SETTEXTEX SetTxtEx {.flags= ST_SELECTION| ST_UNICODE,.codepage= 1200/*CP_UTF8*/ };
	this->SendMsgW(EM_SETTEXTEX, (WPARAM)&SetTxtEx, (LPARAM)text);
}

std::unique_ptr<DefCharFormat> CRichEdit::GetCharFormat(int start, int end)
{
	this->SetSel(start, end);
	return this->GetCurrentCharFormat();
}


std::unique_ptr<DefCharFormat> CRichEdit::GetCharFormat(int line)
{
	this->SetSelLine(line);
	return this->GetCurrentCharFormat();
}

std::unique_ptr<DefCharFormat> CRichEdit::GetCurrentCharFormat()
{
	auto cf = std::make_unique<DefCharFormat>();
	cf->cbSize = sizeof(DefCharFormat);
	cf->dwMask = CFM_ALL;
	this->SendMsgW(EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)cf.get());
	return std::move(cf);
}

std::unique_ptr<DefCharFormat> CRichEdit::GetDefaultCharFormat()
{
	auto cf = std::make_unique<DefCharFormat>();
	cf->cbSize = sizeof(DefCharFormat);
	cf->dwMask = CFM_ALL;
	this->SendMsgW(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)cf.get());
	return cf;
}

void CRichEdit::GetDefaultCharFormat(DefCharFormat* cf)
{
	if(nullptr != cf)
		memcpy(cf, &this->m_default_format, this->m_default_format.cbSize);
}

int CRichEdit::setForeground(COLORREF color)
{
	//this->m_last_format->dwMask = CFM_COLOR;
	//this->m_last_format->crTextColor = color;

	DefCharFormat cf{ 0 };
	cf.cbSize = sizeof(DefCharFormat);
	cf.dwMask = CFM_COLOR;
	cf.crTextColor = color;

	this->SetSelEnd();
	return this->SetSelectionCharFormat(&cf);
}

int CRichEdit::setBackground(COLORREF color)
{
#if 0
	this->m_last_format->dwMask = CFM_BACKCOLOR;
	this->m_last_format->crBackColor = color;
#endif
	DefCharFormat cf{ 0 };
	cf.cbSize = sizeof(DefCharFormat);
	cf.dwMask = CFM_BACKCOLOR;
	cf.crBackColor = color;
	this->SetSelEnd();
	return this->SetSelectionCharFormat(&cf);
}

int CRichEdit::setTextAndBackColor(COLORREF textcolor, COLORREF backcolor)
{
	DefCharFormat cf{ 0 };
	cf.cbSize = sizeof(DefCharFormat);
	cf.dwMask = CFM_BACKCOLOR | CFM_COLOR;
	cf.crTextColor = textcolor;
	cf.crBackColor = backcolor;
	this->SetSelEnd();
	return this->SetSelectionCharFormat(&cf);
}

int CRichEdit::SetDefaultFormat()
{
	//DefCharFormat cf{ this->m_default_format };
	//cf.cbSize = sizeof(DefCharFormat);
	//cf.dwMask = CFM_ALL;
	
	return this->SetDefaultFormat(&this->m_default_format);
}

int CRichEdit::SetDefaultFormat(const DefCharFormat* cf)
{
	return this->SendMsgW(EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)cf);
}

int CRichEdit::SetSelectionCharFormat(const DefCharFormat* cf)
{
	return this->SendMsgW(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)cf);
}

int CRichEdit::SetLineFormat(const DefCharFormat* cf, int line)
{
	this->SetSelLine(line);
	return this->SetSelectionCharFormat(cf);
}


