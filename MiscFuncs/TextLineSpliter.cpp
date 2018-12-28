#include "TextLineSpliter.h"

#define MAX_LINE_LENGTH (MAX_PATH + 10)

CTextLineSpliter::CTextLineSpliter()
{
}


CTextLineSpliter::~CTextLineSpliter()
{
}

void CTextLineSpliter::SetBufPointer(SCRIPTBUFFER *sfvBuf, int pos)
{
	sfvBuf->dwPos += pos;
	sfvBuf->bufpos += pos;
}

BOOL CTextLineSpliter::ReadFromBuffer(void *_dstbuf, size_t size, SCRIPTBUFFER *sfvBuf)
{
	if ((sfvBuf->dwPos + size) > sfvBuf->dwSize)
		return FALSE;

	memcpy(_dstbuf, sfvBuf->bufpos, size * sizeof(wchar_t));
	SetBufPointer(sfvBuf, size);
	return TRUE;
}

#define CHAR_IS_CRLF(ch) ((ch == L'\n') || (ch == L'\r'))

void CTextLineSpliter::GetNextLine(SCRIPTBUFFER *sfvBuf, wchar_t * szLineUnicode, const UINT uiLengthLine,
	UINT * puiStringLength, BOOL * pbErrorOccured, BOOL * pbEndOfFile)
{
	wchar_t myChar;

	UINT uiCount;
	BOOL bSuccess;

	// 至少需要一个字节存放 \0
	if (uiLengthLine <= 1) {
		(*pbErrorOccured) = TRUE;
		return;
	}

	ZeroMemory(szLineUnicode, uiLengthLine * sizeof(wchar_t));
	uiCount = 0;

	while (TRUE) {
		//在下次循环中写入szLineAnsi[uiCount]并在 szLineAnsi[uiCount+1] 中写入\0
		if (uiCount >= uiLengthLine - 1) {
			(*pbErrorOccured) = TRUE;
			return;
		}
		bSuccess = ReadFromBuffer(&myChar, 1/*sizeof(myChar)*/, sfvBuf);

		if (!bSuccess) {
			(*pbErrorOccured) = FALSE;
			(*puiStringLength) = uiCount;
			(*pbEndOfFile) = TRUE;
			return;
		}

		if (CHAR_IS_CRLF(myChar)) {
			do {
				bSuccess = ReadFromBuffer(&myChar, 1/*sizeof(myChar)*/, sfvBuf);

			} while ((bSuccess) && (CHAR_IS_CRLF(myChar)));

			if (bSuccess && !(CHAR_IS_CRLF(myChar)))
				SetBufPointer(sfvBuf, -1);

			(*pbErrorOccured) = FALSE;
			(*puiStringLength) = uiCount;
			(*pbEndOfFile) = FALSE;
			return;
		}
		else {
			szLineUnicode[uiCount] = myChar;
			uiCount += 1;
		}
	}
	return;
}
#undef CHAR_IS_CRLF

void CTextLineSpliter::SplitText(const wchar_t* _src, const UINT _len, vector<wstring>&splitedLine, int flag)
{
	wchar_t	szLineUnicode[MAX_LINE_LENGTH];
	UINT	uiStringLength;
	BOOL	bErrorOccured, bEndOfFile;
	SCRIPTBUFFER sfvBuf = { 0 };

	sfvBuf.buffer = _src;
	sfvBuf.dwSize = _len;

	splitedLine.clear();

	//处理读取的内容
	sfvBuf.bufpos = sfvBuf.buffer;

	do {

		GetNextLine(&sfvBuf, szLineUnicode, MAX_LINE_LENGTH, &uiStringLength, &bErrorOccured, &bEndOfFile);
		if (bErrorOccured) {
			return;
		}

		wchar_t* szLineResult = szLineUnicode;

		if (flag & LINE_TRIM_LEFT) {
			TrimLeft(szLineResult, uiStringLength);
		}

		if (flag & LINE_TRIM_RIGHT) {
			TrimRight(szLineResult, uiStringLength);
		}

		if (flag & LINE_EMPTY_DELETE) {
			if (!*szLineResult)
				continue;
		}

		splitedLine.push_back(szLineResult);

	} while (!(bEndOfFile && uiStringLength == 0));

}

void CTextLineSpliter::TrimLeft(wchar_t* &lpszText, UINT &uiStringLength)
{
	while((' ' == *lpszText ) && (*lpszText)) {
		lpszText++;
		uiStringLength--;
	}
}

void CTextLineSpliter::TrimRight(wchar_t* lpszText, UINT &uiStringLength)
{
	while ((lpszText[uiStringLength - 1] == ' ') && (uiStringLength > 0)) {
		lpszText[uiStringLength - 1] = 0;
		uiStringLength--;
	}
}