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

#include "ZupClass.h"

CZupClass::CZupClass(LPPCK_RUNTIME_PARAMS inout) : 
	CPckClass(inout),
	m_lpZupIndexTable(NULL)
{}

CZupClass::~CZupClass()
{
	if(NULL != m_lpZupIndexTable)
		free(m_lpZupIndexTable);
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

CONST	LPPCKINDEXTABLE CZupClass::GetPckIndexTable()
{
	return m_lpZupIndexTable;
}

void CZupClass::BuildDirTree()
{
	CMapViewFileMultiPckRead	cReadfile;

	if(!cReadfile.OpenPckAndMappingRead(m_PckAllInfo.szFilename))
		return;

	LPPCKINDEXTABLE lpPckIndexTable = m_PckAllInfo.lpPckIndexTable;
	LPPCKINDEXTABLE lpZupIndexTable = m_lpZupIndexTable;

	for(uint32_t i = 0;i < m_PckAllInfo.dwFileCount;i++) {

		//以element\开头的都需要解码
		//其他直接复制
		//"element\" = 0x6d656c65, 0x5c746e656d656c65
		if(0x6d656c65 == *(uint32_t*)lpPckIndexTable->cFileIndex.szFilename) {

			//解码文件名
			memcpy(lpZupIndexTable, lpPckIndexTable, sizeof(PCKINDEXTABLE));
			DecodeFilename(lpZupIndexTable->cFileIndex.szFilename, lpZupIndexTable->cFileIndex.szwFilename, lpPckIndexTable->cFileIndex.szFilename);

			uint8_t	*lpbuffer = cReadfile.View(lpZupIndexTable->cFileIndex.dwAddressOffset, lpZupIndexTable->cFileIndex.dwFileCipherTextSize);
			if(NULL == lpbuffer) {

				Logger_el(TEXT_VIEWMAPNAME_FAIL, m_PckAllInfo.szFilename);
				return;
			}

			if(lpZupIndexTable->cFileIndex.dwFileCipherTextSize == lpZupIndexTable->cFileIndex.dwFileClearTextSize) {

				lpZupIndexTable->cFileIndex.dwFileClearTextSize = *(uint32_t*)lpbuffer;

				lpZupIndexTable->cFileIndex.dwAddressOffset += 4;
				lpZupIndexTable->cFileIndex.dwFileCipherTextSize -= 4;

			} else {

				if(m_zlib.check_zlib_header(lpbuffer)) {
					ulong_t	dwFileBytesRead = 4;
					m_zlib.decompress_part((uint8_t*)&lpZupIndexTable->cFileIndex.dwFileClearTextSize, &dwFileBytesRead,
						lpbuffer, lpZupIndexTable->cFileIndex.dwFileCipherTextSize, lpZupIndexTable->cFileIndex.dwFileCipherTextSize);
				} else {
					lpZupIndexTable->cFileIndex.dwFileClearTextSize = *(uint32_t*)lpbuffer;
				}

				lpZupIndexTable->cFileIndex.dwFileCipherTextSize = lpPckIndexTable->cFileIndex.dwFileClearTextSize;
			}

			cReadfile.UnmapViewAll();
		} else {
			//直接复制
			memcpy(lpZupIndexTable, lpPckIndexTable, sizeof(PCKINDEXTABLE));
			CPckClassCodepage::PckFilenameCode2UCS(lpZupIndexTable->cFileIndex.szFilename, lpZupIndexTable->cFileIndex.szwFilename, sizeof(lpZupIndexTable->cFileIndex.szwFilename) / sizeof(wchar_t));
		}

		lpPckIndexTable++;
		lpZupIndexTable++;
	}

	//建立目录
	ParseIndexTableToNode(m_lpZupIndexTable);

}

BOOL CZupClass::Init(LPCWSTR szFile)
{
	wcscpy_s(m_PckAllInfo.szFilename, szFile);
	GetFileTitleW(m_PckAllInfo.szFilename, m_PckAllInfo.szFileTitle, MAX_PATH);

	if(MountPckFile(m_PckAllInfo.szFilename)) {

		if(NULL == (m_lpZupIndexTable = AllocPckIndexTableByFileCount())) {
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
		return (m_PckAllInfo.isPckFileLoaded = TRUE);
	} else {
		ResetPckInfos();
		return FALSE;
	}
}

