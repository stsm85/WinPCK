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

#include "showimgpch.h"
#include "ShowPicture.h"

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0;
	UINT size = 0;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size == 0) {
		return -1;
	}

	auto pImageCodecInfo = std::make_unique<Gdiplus::ImageCodecInfo[]>(num);

	if(pImageCodecInfo == nullptr) {
		return -1;
	}

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo.get());

	for(UINT j = 0; j < num; ++j) {
		if(wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
			*pClsid = pImageCodecInfo[j].Clsid;
			return j;
		}
	}
	return -1;
}

bool CShowPicture::Save(const wchar_t *lpszFilename)
{
	CLSID pngClsid;
	GetEncoderClsid(L"image/png", &pngClsid);
	this->m_lpmyImage->Save(lpszFilename, &pngClsid, NULL);
	return true;
}