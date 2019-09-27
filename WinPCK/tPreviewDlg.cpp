
#include "tPreviewDlg.h"
#include <tchar.h>
#include "DdsTgaDecoderDefine.h"


CPriviewInDlg::CPriviewInDlg() : m_buffer(NULL), dlg(NULL) {};
CPriviewInDlg::~CPriviewInDlg()
{}

#pragma region 为buffer申请内存
BOOL CPriviewInDlg::AllocBuffer(PICFORMAT fmt, DWORD dwSize)
{
	if(0 != dwSize) {
		if(FMT_UNKNOWN == fmt) {
			if(VIEW_TEXT_MAX_BUFFER < dwSize)
				dwSize = VIEW_TEXT_MAX_BUFFER;
			m_buffersize = dwSize;
			m_buffer = (char*)malloc((dwSize) + 2);
		} else {
			m_buffer = (char*)malloc(m_buffersize = dwSize);
		}

	} else {
		m_buffer = NULL;
		return TRUE;
	}

	return (NULL != m_buffer);
}
#pragma endregion


#pragma region 获取PICFORMAT
PICFORMAT CPriviewInDlg::GetPicFormatFromFilename(LPCWSTR lpszFilename)
{
	LPCWSTR		lpszFileExt = wcsrchr(lpszFilename, L'.');
	PICFORMAT	picFormat = FMT_UNKNOWN;

	if(NULL != lpszFileExt) {
		wchar_t szExt[8] = { 0 };
		//_tcscpy_s(szExt, lpszFileExt);
		wcsncpy(szExt, lpszFileExt, sizeof(szExt)/sizeof(wchar_t) - 1);

		//转化lpszFileExt为小写,当strlen(lpszFileExt) > 6 时报错，修改
		if(4 == wcslen(szExt)) {
			_wcslwr_s(szExt, 6);

			if(NULL != wcsstr(L".dds.tga.bmp.jpg.png.gif.jpeg.tif.tiff.emf", szExt)) {
				if(0 == wcscmp(szExt, L".dds"))
					picFormat = FMT_DDS;
				else if(0 == wcscmp(szExt, L".tga"))
					picFormat = FMT_TGA;
				else
					picFormat = FMT_RAW;
			}
		}
	}
	return picFormat;
}
#pragma endregion

BOOL CPriviewInDlg::Show(const PCK_UNIFIED_FILE_ENTRY* const lpPckFileIndexToShow, TWin *_win)
{
#pragma region get FileTitle

	LPCWSTR lpszFilename = lpPckFileIndexToShow->szName;
	DWORD dwSize = pck_getFileSizeInEntry(lpPckFileIndexToShow);
	LPCWSTR lpszFileTitle;

	if(NULL == (lpszFileTitle = wcsrchr(lpszFilename, L'\\')))
		if(NULL == (lpszFileTitle = wcsrchr(lpszFilename, L'/')))
			lpszFileTitle = lpszFilename - 1;

	//把前面的'\\'或'/'去掉
	lpszFileTitle++;
#pragma endregion

	switch(PICFORMAT fmt = GetPicFormatFromFilename(lpszFilename)) {

	case FMT_DDS:
	case FMT_TGA:
	case FMT_RAW:
		if(AllocBuffer(fmt, dwSize))
			dlg = new TPicDlg((LPBYTE*)&m_buffer, m_buffersize, fmt, lpszFileTitle, _win);
		break;
	case FMT_UNKNOWN:
		if(AllocBuffer(fmt, dwSize))
			dlg = new TViewDlg((LPBYTE*)&m_buffer, m_buffersize, lpszFileTitle, _win);
		break;
	default:
		break;
	}

	if(NULL == dlg)
		return FALSE;


	if(NULL == m_buffer) {

		if(0 != dwSize)
			pck_logEA(TEXT_MALLOC_FAIL);

		delete dlg;
		return FALSE;
	}

	if((WINPCK_OK == pck_GetSingleFileData(lpPckFileIndexToShow, (char*)m_buffer, m_buffersize)))
		dlg->Exec();

	delete dlg;

	if(NULL != m_buffer)
		free(m_buffer);

	return TRUE;
}