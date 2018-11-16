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
#include "PckClassBaseFeatures.h"
#include "MapViewFileMultiPck.h"

BOOL CPckClassBaseFeatures::GetPckBasicInfo(LPRESTORE_INFOS lpRestoreInfo)
{

	BYTE	*lpFileBuffer;

	lpRestoreInfo->isValid = FALSE;
	lpRestoreInfo->sizeOfIndexTailBuffer = m_PckAllInfo.qwPckSize - m_PckAllInfo.dwAddressOfFilenameIndex;
	lpRestoreInfo->dwAddressOfFilenameIndex = m_PckAllInfo.dwAddressOfFilenameIndex;
	lpRestoreInfo->dwMaxSinglePckSize = m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.dwMaxSinglePckSize;

	_tcscpy_s(lpRestoreInfo->szFile, m_PckAllInfo.szFilename);
	
	if(NULL == (lpRestoreInfo->lpIndexTailBuffer = (LPBYTE) malloc (lpRestoreInfo->sizeOfIndexTailBuffer))){

		m_PckLog.PrintLogEL(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	CMapViewFileMultiPckRead	cReadfile;

	if(!cReadfile.OpenPckAndMappingRead(m_PckAllInfo.szFilename))
	{
		free(lpRestoreInfo->lpIndexTailBuffer);
		return FALSE;
	}

	if(NULL == (lpFileBuffer = cReadfile.View(m_PckAllInfo.dwAddressOfFilenameIndex, lpRestoreInfo->sizeOfIndexTailBuffer)))
	{
		m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
		free(lpRestoreInfo->lpIndexTailBuffer);
		return FALSE;
	}

	memcpy(lpRestoreInfo->lpIndexTailBuffer, lpFileBuffer, lpRestoreInfo->sizeOfIndexTailBuffer);
	memcpy(&lpRestoreInfo->Head, m_PckAllInfo.lpSaveAsPckVerFunc->FillHeadData(&m_PckAllInfo), sizeof(PCKHEAD_V2020));

	lpRestoreInfo->isValid = TRUE;
	return TRUE;

}

//当修改文件名等操作造成pck文件损坏无法读取时，恢复pck的索引数据
BOOL CPckClassBaseFeatures::SetPckBasicInfo(LPRESTORE_INFOS lpRestoreInfo)
{
	BYTE	*lpFileBuffer;

	CMapViewFileMultiPckWrite	cWritefile(lpRestoreInfo->dwMaxSinglePckSize);

	if(!cWritefile.OpenPckAndMappingWrite(lpRestoreInfo->szFile, OPEN_EXISTING, lpRestoreInfo->Head.dwPckSize)){
		return FALSE;
	}

	cWritefile.SetFilePointer(0, FILE_BEGIN);
	cWritefile.Write(&lpRestoreInfo->Head, sizeof(PCKHEAD_V2020));

	if(NULL == (lpFileBuffer = cWritefile.View(lpRestoreInfo->Head.dwPckSize - lpRestoreInfo->sizeOfIndexTailBuffer, lpRestoreInfo->sizeOfIndexTailBuffer)))
	{
		m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	memcpy(lpFileBuffer, lpRestoreInfo->lpIndexTailBuffer, lpRestoreInfo->sizeOfIndexTailBuffer);

	return TRUE;
}
