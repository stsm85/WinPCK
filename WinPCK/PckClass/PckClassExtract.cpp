//////////////////////////////////////////////////////////////////////
// PckClassExtract.cpp: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 解压功能
//
// 此程序由 李秋枫/stsm/liqf 编写，pck结构参考若水的pck结构.txt，并
// 参考了其易语言代码中并于读pck文件列表的部分
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

//#include "zlib.h"
#include "PckClass.h"


BOOL CPckClass::GetSingleFileData(LPVOID lpvoidFileRead, LPPCKINDEXTABLE lpPckFileIndexTable, char *buffer, size_t sizeOfBuffer)
{

	CMapViewFileRead	*lpFileRead = (CMapViewFileRead*)lpvoidFileRead;

	LPPCKFILEINDEX lpPckFileIndex = &lpPckFileIndexTable->cFileIndex;

	if(NULL == lpvoidFileRead)
	{
		lpFileRead = new CMapViewFileRead();

		if(!OpenPckAndMappingRead(lpFileRead, m_PckAllInfo.szFilename, m_szMapNameRead)){
			delete lpFileRead;
			return FALSE;
		}

	}

	BYTE	*lpMapAddress;
	if(NULL == (lpMapAddress = lpFileRead->View(lpPckFileIndex->dwAddressOffset, lpPckFileIndex->dwFileCipherTextSize)))
	{

		PrintLogE(TEXT_VIEWMAPNAME_FAIL, m_PckAllInfo.szFilename, __FILE__, __FUNCTION__, __LINE__);

		if(NULL == lpvoidFileRead)
			delete lpFileRead;
		return FALSE;
	}

	DWORD	dwFileLengthToWrite = lpPckFileIndex->dwFileClearTextSize;

	if(0 != sizeOfBuffer && sizeOfBuffer < dwFileLengthToWrite)
		dwFileLengthToWrite = sizeOfBuffer;


	if(PCK_BEGINCOMPRESS_SIZE < lpPckFileIndex->dwFileClearTextSize)
	{

		if(!decompress_part((BYTE*)buffer, &dwFileLengthToWrite,
					lpMapAddress, lpPckFileIndex->dwFileCipherTextSize, lpPckFileIndex->dwFileClearTextSize))
		{
			if(lpPckFileIndex->dwFileClearTextSize == lpPckFileIndex->dwFileCipherTextSize)
				memcpy(buffer, lpMapAddress, dwFileLengthToWrite);
			else
			{

				PrintLogE(TEXT_UNCOMPRESSDATA_FAIL, lpPckFileIndex->szFilename, __FILE__, __FUNCTION__, __LINE__);

				lpFileRead->UnmapView();

				if(NULL == lpvoidFileRead)
					delete lpFileRead;

				return FALSE;
			}
		}
	}else{
		memcpy(buffer, lpMapAddress, dwFileLengthToWrite);
	}

	lpFileRead->UnmapView();

	if(NULL == lpvoidFileRead)
		delete lpFileRead;



	return TRUE;

}

BOOL CPckClass::ExtractFiles(LPPCKINDEXTABLE *lpIndexToExtract, int nFileCount)
{

	PrintLogI(TEXT_LOG_EXTRACT);

	CMapViewFileRead	*lpFileRead = new CMapViewFileRead();

	if(!OpenPckAndMappingRead(lpFileRead, m_PckAllInfo.szFilename, m_szMapNameRead)){
		delete lpFileRead;
		return FALSE;
	}
	
	BOOL	ret = TRUE;

	char	szFilename[MAX_PATH_PCK_260], *szStrchr;

	LPPCKINDEXTABLE *lpIndexToExtractPtr = lpIndexToExtract;

	for(int i=0;i<nFileCount;i++)
	{
		if(!lpPckParams->cVarParams.bThreadRunning)
		{
			PrintLogW(TEXT_USERCANCLE);

			delete lpFileRead;
			return FALSE;
		}

		memcpy(szFilename, (*lpIndexToExtractPtr)->cFileIndex.szFilename, MAX_PATH_PCK_260);
		szStrchr = szFilename;
		for(int j=0;j<MAX_PATH_PCK_260;j++)
		{
			if('\\' == *szStrchr)*szStrchr = '_';
			else if('/' == *szStrchr)*szStrchr = '_';
			//else if(0 == *szStrchr)break;
			++szStrchr;
		}

		//解压文件
		if(!(DecompressFile(szFilename, *lpIndexToExtractPtr, lpFileRead)))
		{
			PrintLogE(TEXT_UNCOMP_FAIL, __FILE__, __FUNCTION__, __LINE__);

			delete lpFileRead;
			return FALSE;
		}else{
			//dwCount++;
			++(lpPckParams->cVarParams.dwUIProgress);
		}

		++lpIndexToExtractPtr;
	}

	delete lpFileRead;

	PrintLogI(TEXT_LOG_WORKING_DONE);

	return	ret;
}

BOOL CPckClass::ExtractFiles(LPPCK_PATH_NODE *lpNodeToExtract, int nFileCount)
{

	PrintLogI(TEXT_LOG_EXTRACT);

	CMapViewFileRead	*lpFileRead = new CMapViewFileRead();

	if(!OpenPckAndMappingRead(lpFileRead, m_PckAllInfo.szFilename, m_szMapNameRead)){
		delete lpFileRead;
		return FALSE;
	}

	BOOL	ret = TRUE;

	LPPCK_PATH_NODE *lpNodeToExtractPtr = lpNodeToExtract;

	for(int i=0;i<nFileCount;i++)
	{
		if(!lpPckParams->cVarParams.bThreadRunning)
		{
			PrintLogW(TEXT_USERCANCLE);

			delete lpFileRead;
			return FALSE;
		}

		if(NULL == (*lpNodeToExtractPtr)->child)
		{

			//解压文件
			if(!(DecompressFile((*lpNodeToExtractPtr)->szName, (*lpNodeToExtractPtr)->lpPckIndexTable, lpFileRead)))
			{
				PrintLogE(TEXT_UNCOMP_FAIL, __FILE__, __FUNCTION__, __LINE__);

				delete lpFileRead;
				return FALSE;
			}else{
				//dwCount++;
				++(lpPckParams->cVarParams.dwUIProgress);
			}

		}else{

			CreateDirectoryA((*lpNodeToExtractPtr)->szName, NULL);
			SetCurrentDirectoryA((*lpNodeToExtractPtr)->szName);
			ret = StartExtract((*lpNodeToExtractPtr)->child->next, lpFileRead);
			SetCurrentDirectoryA("..\\");
		}

		lpNodeToExtractPtr++;
	}

	delete lpFileRead;

	PrintLogI(TEXT_LOG_WORKING_DONE);

	return	ret;
}
BOOL CPckClass::StartExtract(LPPCK_PATH_NODE lpNodeToExtract, /*DWORD	&dwCount, */LPVOID lpvoidFileRead/*, BOOL	&bThreadRunning*/)
{

	CMapViewFileRead	*lpFileRead = (CMapViewFileRead*)lpvoidFileRead;

	do
	{
		if(!lpPckParams->cVarParams.bThreadRunning)
		{
			PrintLogW(TEXT_USERCANCLE);
			return FALSE;
		}

		//lpThisNodePtr = lpNodeToExtract;
		if(NULL != lpNodeToExtract->child)
		{
			
			CreateDirectoryA(lpNodeToExtract->szName, NULL);
			SetCurrentDirectoryA(lpNodeToExtract->szName);
			
			StartExtract(lpNodeToExtract->child->next, lpFileRead);

			SetCurrentDirectoryA("..\\");

		}else{
		
			//解压文件
			if(!DecompressFile(lpNodeToExtract->szName, lpNodeToExtract->lpPckIndexTable, lpFileRead))
			{
				PrintLogE(TEXT_UNCOMP_FAIL, __FILE__, __FUNCTION__, __LINE__);

				return FALSE;
			}else{
				//dwCount++;
				++(lpPckParams->cVarParams.dwUIProgress);
			}
		}

		lpNodeToExtract = lpNodeToExtract->next;

	}while(NULL != lpNodeToExtract);

	return TRUE;
}

BOOL CPckClass::DecompressFile(char	*lpszFilename, LPPCKINDEXTABLE lpPckFileIndexTable, LPVOID lpvoidFileRead)
{
	LPPCKFILEINDEX lpPckFileIndex = &lpPckFileIndexTable->cFileIndex;

	CMapViewFileWrite	*lpFileWrite = new CMapViewFileWrite(0xffffffff);

	LPBYTE	lpMapAddressToWrite;
	DWORD	dwFileLengthToWrite;

	//if (0 == strcmp(lpszFilename, "剑仙男技能攻击1.gfx")){
	//	dwFileLengthToWrite = 1;
	//}

	dwFileLengthToWrite = lpPckFileIndex->dwFileClearTextSize;

	//以下是创建一个文件，用来保存解压缩后的文件
	if(!lpFileWrite->Open(lpszFilename, CREATE_ALWAYS))
	{
		PrintLogE(TEXT_OPENWRITENAME_FAIL, lpszFilename, __FILE__, __FUNCTION__, __LINE__);
		delete lpFileWrite;
		return FALSE;
	}

	if(!lpFileWrite->Mapping(m_szMapNameWrite, dwFileLengthToWrite))
	{
		delete lpFileWrite;
		if(0 == dwFileLengthToWrite)return TRUE;
		PrintLogE(TEXT_CREATEMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	if(NULL == (lpMapAddressToWrite = lpFileWrite->View(0, 0)))
	{
		PrintLogE(TEXT_VIEWMAPNAME_FAIL, lpszFilename, __FILE__, __FUNCTION__, __LINE__);

		delete lpFileWrite;
		return FALSE;
	}


	BOOL rtn = GetSingleFileData(lpvoidFileRead, lpPckFileIndexTable, (char*)lpMapAddressToWrite);

	lpFileWrite->SetFilePointer(dwFileLengthToWrite ,FILE_BEGIN);
	lpFileWrite->SetEndOfFile();
	delete lpFileWrite;

	return rtn;
}

