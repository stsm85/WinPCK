//////////////////////////////////////////////////////////////////////
// DrawPicture.cpp: 显示dds、tga图像
// 解码dds、tga并显示
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2018.5.29
//////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <assert.h>
#include "ShowPicture.h"
#include "DdsTgaDecoder.h"


BOOL CShowPicture::BuildImage()
{
	//读dds header
	CDdsTgaDecoder cDdsTgaDecoder;
	switch(m_picFormat) {
	case FMT_DDS:

		if(!cDdsTgaDecoder.DecodeDDS(m_resBufferInClass, m_bufsize, m_DecodedDIBData, m_picWidth, m_picHeight, m_szPictureFormat)) {
			MessageBoxA(NULL, TEXT_SHOWPIC_ERROR, NULL, MB_OK | MB_ICONERROR);
			assert(FALSE);
			return FALSE;
		}
		{
			free(m_resBufferInClass);
			m_resBufferInClass = NULL;
			m_lpmyImage = new Bitmap(m_picWidth, m_picHeight, cDdsTgaDecoder.GetStride(), cDdsTgaDecoder.GetPixelFormat(), m_DecodedDIBData);
		}
		break;

	case FMT_TGA:

		if(!cDdsTgaDecoder.DecodeTGA(m_resBufferInClass, m_bufsize, m_DecodedDIBData, m_picWidth, m_picHeight, m_szPictureFormat)) {
			MessageBoxA(NULL, TEXT_SHOWPIC_ERROR, NULL, MB_OK | MB_ICONERROR);
			assert(FALSE);
			return FALSE;
		}

		free(m_resBufferInClass);
		m_resBufferInClass = NULL;

		m_lpmyImage = new Bitmap(m_picWidth, m_picHeight, cDdsTgaDecoder.GetStride(), cDdsTgaDecoder.GetPixelFormat(), m_DecodedDIBData);
		break;

	case FMT_RAW:	//bmp,jpg,png..

		CreateStreamOnHGlobal(m_hGlobal, FALSE, &m_stream);
		if(!m_stream) {
			MessageBoxA(NULL, TEXT_SHOWPIC_ERROR, NULL, MB_OK | MB_ICONERROR);
			assert(FALSE);
			return FALSE;
		}

		//show picture
		m_lpmyImage = new Image(m_stream);

		//看看图片大小和显示窗口大小
		m_picWidth = m_lpmyImage->GetWidth();
		m_picHeight = m_lpmyImage->GetHeight();

		break;

	default:

		MessageBoxA(NULL, TEXT_NOT_DEFINE, NULL, MB_OK | MB_ICONERROR);
		return FALSE;
		break;
	}

	return TRUE;
}

BOOL CShowPicture::DrawPictureOnDC()
{
	DrawBlockOnDlg();

	m_lpoGraph = new Graphics(m_MemDC);

	m_lpoGraph->DrawImage(m_lpmyImage, 0, 0, m_picWidth, m_picHeight);

	if(NULL != m_lpoGraph)
		delete m_lpoGraph;
	m_lpoGraph = NULL;

	//以下在源代码中是当FMT_RAW == iFormat时执行的
	if (FMT_RAW == m_picFormat) {

		//不可以现在释放m_lpmyImage，保存为png时会用到
		if(NULL != m_lpmyImage)
			delete m_lpmyImage;
		m_lpmyImage = NULL;

		if (NULL != m_stream)
			m_stream->Release();
		m_stream = NULL;

		if (NULL != m_hGlobal)
			GlobalFree(m_hGlobal);
		m_hGlobal = NULL;
	}

	return TRUE;
}