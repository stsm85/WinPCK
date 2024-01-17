#pragma once

#include "WinControl.h"
#include <memory>


class CEdit :
	public CWinControl
{
public:
	CEdit() = default;
	virtual ~CEdit() = default;

	virtual void Attach(int id, HWND hDlg) override;
	virtual void SetLimitText(int size = -1);
	virtual int GetLimitText();


	virtual void SetText(const char* str) override;
	virtual void SetText(const wchar_t* str) override;

	//索引
	
	/// <summary>
	/// 
	/// </summary>
	/// <param name="line">从0开始</param>
	/// <returns>第line行的第一个字符的索引</returns>
	virtual int LineIndex(int line);
	virtual void SetSel(int start, int end);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="line">从0开始</param>
	virtual void SetSelLine(int line);
	virtual void SetSelLine(int start, int end);

	virtual void SetSelEnd() {
		this->SetSel(-2, -1);
	}

	virtual void SetSelAll() {
		this->SetSel(0, -1);
	}

	virtual uint32_t GetLineCount();

	//文本
	virtual void SetSelText(const char* text);
	virtual void SetSelText(const char8_t* text);
	virtual void SetSelText(const wchar_t* text);

	template<typename T>
	void AppendText(const T* text)
	{
		auto scroll = this->IsVScrollOnBottom();
		this->SetSelEnd();
		this->SetSelText(text);
		if (scroll)
			this->SendMsgA(WM_VSCROLL, SB_BOTTOM, 0);
	}

	virtual void DeleteLine(int start, int end);

};
