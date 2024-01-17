#include "CEdit.h"

void CEdit::Attach(int id, HWND hDlg)
{
	CWinControl::Attach(id, hDlg);
}

void CEdit::SetLimitText(int size)
{
	this->SendMsgA(EM_SETLIMITTEXT, size, 0);
}

int CEdit::GetLimitText()
{
	return this->SendMsgA(EM_GETLIMITTEXT, 0, 0);
}

void CEdit::SetText(const char* str)
{
	this->SetSelAll();
	this->SetSelText(str);
}

void CEdit::SetText(const wchar_t* str)
{
	this->SetSelAll();
	this->SetSelText(str);
}

/// <summary>
/// 
/// </summary>
/// <param name="line">从0开始</param>
/// <returns>第line行的第一个字符的索引</returns>
int CEdit::LineIndex(int line)
{
	return this->SendMsgA(EM_LINEINDEX, line, 0);
}

void CEdit::SetSel(int start, int end)
{
	this->SendMsgA(EM_SETSEL, start, end);
}

/// <summary>
/// 
/// </summary>
/// <param name="line">从0开始</param>
void CEdit::SetSelLine(int line)
{
	if (0 <= line) {
		int lineStart = this->LineIndex(line);//取第一行的第一个字符的索引
		int lineEnd = this->LineIndex(line + 1) /*- 1*/;//取第一行的最后一个字符的索引——用第二行的第一个索引减1来实现
		this->SetSel(lineStart, lineEnd);//选取第一行字符
	}
	else {
		this->SetSelEnd();
	}
}

void CEdit::SetSelLine(int start, int end)
{
	if (end < start)
		std::swap(start, end);

	int lineStart = this->LineIndex(start);//取第一行的第一个字符的索引
	int lineEnd = this->LineIndex(end + 1) /*- 1*/;//取第一行的最后一个字符的索引——用第二行的第一个索引减1来实现
	this->SetSel(lineStart, lineEnd);//选取第一行字符
}


void CEdit::SetSelText(const char* text)
{
	this->SendMsgA(EM_REPLACESEL, 0, (LPARAM)text);
}

void CEdit::SetSelText(const char8_t* text)
{
	this->SendMsgA(EM_REPLACESEL, 0, (LPARAM)text);
}

void CEdit::SetSelText(const wchar_t* text)
{
	this->SendMsgW(EM_REPLACESEL, 0, (LPARAM)text);
}

uint32_t CEdit::GetLineCount()
{
	return this->SendMsgA(EM_GETLINECOUNT, 0, 0);
}
#if 0
void CEdit::AppendText(const char* text)
{
	auto scroll = this->IsVScrollOnBottom();
	this->SetSelEnd();
	this->SetSelText(text);
	if(scroll)
		this->SendMsgA(WM_VSCROLL, SB_BOTTOM, 0);
}

void CEdit::AppendText(const wchar_t* text)
{
	auto scroll = this->IsVScrollOnBottom();
	this->SetSelEnd();
	this->SetSelText(text);
	if (scroll)
		this->SendMsgA(WM_VSCROLL, SB_BOTTOM, 0);
}
#endif

void CEdit::DeleteLine(int start, int end)
{
	this->SetSelLine(start, end);
	this->SetSelText("");
}

