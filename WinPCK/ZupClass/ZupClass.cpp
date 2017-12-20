//////////////////////////////////////////////////////////////////////
// ZupClass.cpp: 用于解析完美世界公司的zup文件中的数据，并显示在List中
// 头文件
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.5.23
//////////////////////////////////////////////////////////////////////
//#include "zlib.h"
#include "ZupClass.h"


CZupClass::CZupClass(LPPCK_RUNTIME_PARAMS inout) : CPckClass(inout)
{
	lpPckParams = inout;

	m_lpZupIndexTable = NULL;
	//m_firstFile = NULL;
	memset(&m_RootNodeZup, 0, sizeof(PCK_PATH_NODE));

}

CZupClass::~CZupClass()
{
	DeAllocMultiNodes(m_RootNodeZup.child);
	if(NULL != m_lpZupIndexTable)free(m_lpZupIndexTable);	
}

CONST	LPPCKINDEXTABLE CZupClass::GetPckIndexTable()
{
	return m_lpZupIndexTable;
}

CONST	LPPCK_PATH_NODE CZupClass::GetPckPathNode()
{
	return &m_RootNodeZup;
}


void CZupClass::BuildDirTree()
{

	size_t	sizeFileIndex = sizeof(PCKINDEXTABLE);

	CMapViewFileRead	*lpcReadfile = new CMapViewFileRead();

	if(!OpenPckAndMappingRead(lpcReadfile, m_PckAllInfo.szFilename, m_szMapNameRead)){
		delete lpcReadfile;
		return;
	}

	//if(!lpcReadfile->Open(m_Filename))
	//{
	//	//swprintf(m_lastErrorString, 1024,
	//	//					TEXT_OPENNAME_FAIL, m_Filename);
	//	PrintLogE(TEXT_OPENNAME_FAIL, m_Filename, __FILE__, __FUNCTION__, __LINE__);
	//	delete lpcReadfile;
	//	return;
	//}

	//if(!lpcReadfile->Mapping(m_szMapNameRead))
	//{
	//	//swprintf(m_lastErrorString, 1024,
	//	//					TEXT_CREATEMAPNAME_FAIL, m_Filename);
	//	PrintLogE(TEXT_CREATEMAPNAME_FAIL, m_Filename, __FILE__, __FUNCTION__, __LINE__);

	//	delete lpcReadfile;
	//	return;

	//}

	LPPCKINDEXTABLE lpPckIndexTable = m_lpPckIndexTable;
	LPPCKINDEXTABLE lpZupIndexTable = m_lpZupIndexTable;

	for(DWORD i = 0;i<m_PckAllInfo.dwFileCount;i++)
	{

		//以element\开头的都需要解码
		//其他直接复制
		//"element\" = 0x6d656c65, 0x5c746e656d656c65
		if(0x6d656c65 == *(DWORD*)lpPckIndexTable->cFileIndex.szFilename){

			//解码文件名
			memcpy(lpZupIndexTable, lpPckIndexTable, sizeFileIndex);
			DecodeFilename(lpZupIndexTable->cFileIndex.szFilename, lpPckIndexTable->cFileIndex.szFilename);

			BYTE	*lpbuffer = lpcReadfile->ReView(lpZupIndexTable->cFileIndex.dwAddressOffset, lpZupIndexTable->cFileIndex.dwFileCipherTextSize);
			if(NULL == lpbuffer)
			{

				PrintLogE(TEXT_VIEWMAPNAME_FAIL, m_PckAllInfo.szFilename, __FILE__, __FUNCTION__, __LINE__);

				delete lpcReadfile;
				return ;
			}

			if(lpZupIndexTable->cFileIndex.dwFileCipherTextSize == lpZupIndexTable->cFileIndex.dwFileClearTextSize){

				lpZupIndexTable->cFileIndex.dwFileClearTextSize = *(DWORD*)lpbuffer;

				lpZupIndexTable->cFileIndex.dwAddressOffset += 4;
				lpZupIndexTable->cFileIndex.dwFileCipherTextSize -= 4;

			}else{

				DWORD	dwFileBytesRead = 4;
				//if(Z_OK != 
					decompress((BYTE*)&lpZupIndexTable->cFileIndex.dwFileClearTextSize, &dwFileBytesRead,
						lpbuffer, lpZupIndexTable->cFileIndex.dwFileCipherTextSize);

				lpZupIndexTable->cFileIndex.dwFileCipherTextSize = lpPckIndexTable->cFileIndex.dwFileClearTextSize;

			}


		}else{
			//直接复制
			memcpy(lpZupIndexTable, lpPckIndexTable, sizeFileIndex);

		}

		//建立目录
		AddFileToNode(&m_RootNodeZup, lpZupIndexTable);
		AddFileToNode(&m_RootNode, lpPckIndexTable);

		lpPckIndexTable++;
		lpZupIndexTable++;
	}
	
	delete lpcReadfile;

}

BOOL CZupClass::Init(LPCTSTR szFile)
{

	lstrcpy(m_PckAllInfo.szFilename, szFile);

	if(m_ReadCompleted = MountPckFile(m_PckAllInfo.szFilename))
	{

		if(!AllocIndexTableAndInit(m_lpZupIndexTable, m_PckAllInfo.dwFileCount)){
			return FALSE;
		}

		//第一遍循环，读取inc文件，建立字典
		m_lpDictHash = new CDictHash();
		if(BuildZupBaseDict()){

			//第二遍循环，解码文件名，建立目录树
			BuildDirTree();
		}

		//删除字典
		delete m_lpDictHash;

		return TRUE;

	}else{
		return FALSE;
	}

}

