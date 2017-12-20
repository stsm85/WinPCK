//////////////////////////////////////////////////////////////////////
// PckClassExceptionRestore.cpp: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 用于写入文件后造成的文件损坏的恢复
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2017.6.27
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4244 )
//#pragma warning ( disable : 4311 )

#include "PckClass.h"

BOOL CPckClass::GetPckBasicInfo(LPTSTR lpszFile, PCKHEAD_V2020 *lpHead, LPBYTE &lpFileIndexData, DWORD &dwPckFileIndexDataSize)
{

	BYTE	*lpFileBuffer;

	dwPckFileIndexDataSize = m_PckAllInfo.qwPckSize - m_PckAllInfo.dwAddressName;

	//lstrcpy(lpszFile, m_Filename);
#ifdef UNICODE
	memcpy(lpszFile, m_PckAllInfo.szFilename, MAX_PATH * 2);
#else
	memcpy(lpszFile, m_PckAllInfo.szFilename, MAX_PATH);
#endif
	
	if(NULL == (lpFileIndexData = (LPBYTE) malloc (dwPckFileIndexDataSize))){

		PrintLogE(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	CMapViewFileRead	*lpcReadfile = new CMapViewFileRead();

	if(!OpenPckAndMappingRead(lpcReadfile, m_PckAllInfo.szFilename, m_szMapNameRead))
	{
		free(lpFileIndexData);
		delete lpcReadfile;
		return FALSE;
	}

	if(NULL == (lpFileBuffer = lpcReadfile->View(m_PckAllInfo.dwAddressName, dwPckFileIndexDataSize)))
	{
		PrintLogE(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);

		free(lpFileIndexData);
		delete lpcReadfile;
		
		return FALSE;
	}

	memcpy(lpFileIndexData, lpFileBuffer, dwPckFileIndexDataSize);

	delete lpcReadfile;

	memcpy(lpHead, m_PckAllInfo.lpSaveAsPckVerFunc->FillHeadData(&m_PckAllInfo), sizeof(PCKHEAD_V2020));

	return TRUE;

}

//当修改文件名等操作造成pck文件损坏无法读取时，恢复pck的索引数据
BOOL CPckClass::SetPckBasicInfo(PCKHEAD_V2020 *lpHead, LPBYTE lpFileIndexData, DWORD &dwPckFileIndexDataSize)
{
	BYTE	*lpFileBuffer;

	CMapViewFileWrite	*lpcWritefile = new CMapViewFileWrite(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->dwMaxSinglePckSize);

	if(!OpenPckAndMappingWrite(lpcWritefile, m_PckAllInfo.szFilename, OPEN_EXISTING, lpHead->dwPckSize)){

		delete lpcWritefile;
		return FALSE;
	}

	lpcWritefile->SetFilePointer(0, FILE_BEGIN);
	lpcWritefile->Write(lpHead, sizeof(PCKHEAD_V2020));

	if(NULL == (lpFileBuffer = lpcWritefile->View(lpHead->dwPckSize - dwPckFileIndexDataSize, dwPckFileIndexDataSize)))
	{

		PrintLogE(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);

		delete lpcWritefile;
		return FALSE;
	}

	memcpy(lpFileBuffer, lpFileIndexData, dwPckFileIndexDataSize);

	delete lpcWritefile;
	return TRUE;

}
