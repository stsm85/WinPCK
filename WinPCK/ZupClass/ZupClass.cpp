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

CZupClass::CZupClass(LPPCK_RUNTIME_PARAMS inout) : 
	CPckClass(inout),
	m_lpZupIndexTable(NULL)
{
	m_lpRootNodeZup = m_classNodeZup.GetRootNode();
}

CZupClass::~CZupClass()
{}

CONST	LPPCKINDEXTABLE CZupClass::GetPckIndexTable()
{
	return m_lpZupIndexTable;
}

CONST	LPPCK_PATH_NODE CZupClass::GetPckPathNode()
{
	return m_lpRootNodeZup;
}


void CZupClass::BuildDirTree()
{
	CMapViewFileRead	cReadfile;

	if(!cReadfile.OpenPckAndMappingRead(m_PckAllInfo.szFilename))
		return;

	LPPCKINDEXTABLE lpPckIndexTable = m_PckAllInfo.lpPckIndexTable;
	LPPCKINDEXTABLE lpZupIndexTable = m_lpZupIndexTable;

	for(DWORD i = 0;i < m_PckAllInfo.dwFileCount;i++) {

		//以element\开头的都需要解码
		//其他直接复制
		//"element\" = 0x6d656c65, 0x5c746e656d656c65
		if(0x6d656c65 == *(DWORD*)lpPckIndexTable->cFileIndex.szFilename) {

			//解码文件名
			memcpy(lpZupIndexTable, lpPckIndexTable, sizeof(PCKINDEXTABLE));
			DecodeFilename(lpZupIndexTable->cFileIndex.szFilename, lpZupIndexTable->cFileIndex.szwFilename, lpPckIndexTable->cFileIndex.szFilename);

			BYTE	*lpbuffer = cReadfile.ReView(lpZupIndexTable->cFileIndex.dwAddressOffset, lpZupIndexTable->cFileIndex.dwFileCipherTextSize);
			if(NULL == lpbuffer) {

				m_PckLog.PrintLogEL(TEXT_VIEWMAPNAME_FAIL, m_PckAllInfo.szFilename, __FILE__, __FUNCTION__, __LINE__);
				return;
			}

			if(lpZupIndexTable->cFileIndex.dwFileCipherTextSize == lpZupIndexTable->cFileIndex.dwFileClearTextSize) {

				lpZupIndexTable->cFileIndex.dwFileClearTextSize = *(DWORD*)lpbuffer;

				lpZupIndexTable->cFileIndex.dwAddressOffset += 4;
				lpZupIndexTable->cFileIndex.dwFileCipherTextSize -= 4;

			} else {

				if(check_zlib_header(lpbuffer)) {
					DWORD	dwFileBytesRead = 4;
					decompress_part((BYTE*)&lpZupIndexTable->cFileIndex.dwFileClearTextSize, &dwFileBytesRead,
						lpbuffer, lpZupIndexTable->cFileIndex.dwFileCipherTextSize, lpZupIndexTable->cFileIndex.dwFileCipherTextSize);
				} else {
					lpZupIndexTable->cFileIndex.dwFileClearTextSize = *(DWORD*)lpbuffer;
				}

				lpZupIndexTable->cFileIndex.dwFileCipherTextSize = lpPckIndexTable->cFileIndex.dwFileClearTextSize;
			}
		} else {
			//直接复制
			memcpy(lpZupIndexTable, lpPckIndexTable, sizeof(PCKINDEXTABLE));
			CAnsi2Ucs cA2U;
			cA2U.GetString(lpZupIndexTable->cFileIndex.szFilename, lpZupIndexTable->cFileIndex.szwFilename, sizeof(lpZupIndexTable->cFileIndex.szwFilename) / sizeof(wchar_t));

		}

#if 0
		CAnsi2Ucs cA2U;
		cA2U.GetString(lpZupIndexTable->cFileIndex.szFilename, lpZupIndexTable->cFileIndex.szwFilename, sizeof(lpZupIndexTable->cFileIndex.szwFilename) / sizeof(wchar_t));
#endif

		lpPckIndexTable++;
		lpZupIndexTable++;
	}

	//建立目录
	m_classNodeZup.ParseIndexTableToNode();
	//m_classNode.ParseIndexTableToNode();

}

BOOL CZupClass::Init(LPCTSTR szFile)
{
	_tcscpy(m_PckAllInfo.szFilename, szFile);
	GetFileTitle(m_PckAllInfo.szFilename, m_PckAllInfo.szFileTitle, MAX_PATH);

	if(m_ReadCompleted = MountPckFile(m_PckAllInfo.szFilename)) {

		if(NULL == (m_lpZupIndexTable = (m_classNodeZup.AllocPckIndexTableByFileCount(m_PckAllInfo.dwFileCount)))) {
			return FALSE;
		}

		//第一遍循环，读取inc文件，建立字典
		m_lpDictHash = new CDictHash();
		if(BuildZupBaseDict()) {

			//第二遍循环，解码文件名，建立目录树
			BuildDirTree();
		}

		//删除字典
		delete m_lpDictHash;
		return TRUE;
	} else {
		return FALSE;
	}
}

