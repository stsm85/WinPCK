#pragma once

#include "WinControl.h"
#include <Richedit.h>
#include "CEdit.h"
#include <memory>

typedef CHARFORMAT2W DefCharFormat;

class CRichEdit :
	public CEdit
{
public:
	CRichEdit() = default;
	virtual ~CRichEdit() = default;

	virtual void Attach(int id, HWND hDlg) override;

	//virtual void SetLimitText(int size = -1) override;

	void RichTextMode(bool isRichText);
	void SetSingleLevelUndo(bool SingleLevelUndo);
	void SetMultiCodepage(bool isMultiCodepage);

	void	GetScrollPos(POINT* point);
	virtual int		GetScrollPosX() override;
	virtual int		GetScrollPosY() override;

	//文本
	virtual void SetSelText(const char8_t* text) override;
	virtual void SetSelText(const wchar_t* text) override;

	//保存的最后一个字符的格式
	DefCharFormat m_default_format{ 0 };

	//格式
	std::unique_ptr<DefCharFormat> GetCharFormat(int start, int end);
	std::unique_ptr<DefCharFormat> GetCharFormat(int line);
	std::unique_ptr<DefCharFormat> GetCurrentCharFormat();
	std::unique_ptr<DefCharFormat> GetDefaultCharFormat();
	void GetDefaultCharFormat(DefCharFormat* cf);

	//默认修改最后一个字符的颜色
	int setForeground(COLORREF color);
	int setBackground(COLORREF color);
	int setTextAndBackColor(COLORREF textcolor, COLORREF backcolor);

	//设置某一行颜色
	int SetDefaultFormat();
	int SetDefaultFormat(const DefCharFormat* cf);
	int SetSelectionCharFormat(const DefCharFormat* cf);
	int SetLineFormat(const DefCharFormat* cf, int line = -1);

};
