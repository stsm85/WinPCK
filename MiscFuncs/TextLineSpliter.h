#pragma once
#include <string>
#include <vector>
using namespace std;

#include <Windows.h>

#define LINE_TRIM_LEFT		1
#define LINE_TRIM_RIGHT		2
#define LINE_EMPTY_DELETE	4

class CTextLineSpliter
{


public:
	CTextLineSpliter();
	~CTextLineSpliter();

	void SplitText(const wchar_t* _src, const UINT _len, vector<wstring>&splitedLine, int flag);

private:
	typedef struct
	{
		const wchar_t*	buffer;
		const wchar_t*	bufpos;
		unsigned long	dwSize;
		unsigned long	dwPos;

	}SCRIPTBUFFER;

	void	GetNextLine(SCRIPTBUFFER *sfvBuf, wchar_t* szLineUnicode, const UINT uiLengthLine, UINT * puiStringLength, BOOL * pbErrorOccured, BOOL * pbEndOfFile);
	BOOL	ReadFromBuffer(void *_dstbuf, size_t size, SCRIPTBUFFER *sfvBuf);
	void	SetBufPointer(SCRIPTBUFFER *sfvBuf, int pos);

	void	TrimLeft(wchar_t* &lpszText, UINT &uiStringLength);
	void	TrimRight(wchar_t* lpszText, UINT &uiStringLength);
};

