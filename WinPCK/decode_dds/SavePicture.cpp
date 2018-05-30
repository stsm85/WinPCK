//////////////////////////////////////////////////////////////////////
// SavePicture.cpp: 构建DIB数据
// 将解码的dds、tga数据保存为文件
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2018.5.29
//////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include "ShowPicture.h"

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0;
	UINT size = 0;

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0) {
		return -1;
	}
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL) {
		return -1;
	}

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j) {
		if(wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}

	free(pImageCodecInfo);
	return -1;
}

BOOL CShowPicture::Save(const WCHAR *lpszFilename)
{
	CLSID pngClsid;
	GetEncoderClsid(L"image/png", &pngClsid);
	m_lpmyImage->Save(lpszFilename, &pngClsid, NULL);
	return TRUE;
}