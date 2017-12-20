//////////////////////////////////////////////////////////////////////
// PckClassReader.cpp: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 文件头、尾等结构体的读取
//
// 此程序由 李秋枫/stsm/liqf 编写，pck结构参考若水的pck结构.txt，并
// 参考了其易语言代码中并于读pck文件列表的部分
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2017.6.23
//////////////////////////////////////////////////////////////////////

#include "PckClass.h"

#pragma warning ( disable : 4244 )
#pragma warning ( disable : 4267 )

BOOL CPckClass::ReadPckFileIndexes()
{
	CMapViewFileRead *lpRead = new CMapViewFileRead();

	if(!OpenPckAndMappingRead(lpRead, m_PckAllInfo.szFilename, m_szMapNameRead))
	{
		PrintLogE(TEXT_OPENNAME_FAIL, m_PckAllInfo.szFilename, __FILE__, __FUNCTION__, __LINE__);
		delete lpRead;
		return FALSE;
	}

	if(!AllocIndexTableAndInit(m_lpPckIndexTable, m_PckAllInfo.dwFileCount)){
		delete lpRead;
		return FALSE;
	}

	//开始读文件
	BYTE	*lpFileBuffer;
	if(NULL == (lpFileBuffer = lpRead->View(m_PckAllInfo.dwAddressName, lpRead->GetFileSize() - m_PckAllInfo.dwAddressName)))
	{
		PrintLogE(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);

		delete lpRead;
		return FALSE;
	}
	
	LPPCKINDEXTABLE lpPckIndexTable = m_lpPckIndexTable;
	BOOL			isLevel0;
	DWORD			byteLevelKey;
	//存放每条文件索引的头部的两个DWORD压缩数据长度信息
	DWORD			dwFileIndexTableCryptedDataLength[2];
	DWORD			dwFileIndexTableClearDataLength = m_PckAllInfo.lpDetectedPckVerFunc->dwFileIndexSize;
	DWORD			IndexCompressedFilenameDataLengthCryptKey[2] = { \
		m_PckAllInfo.lpDetectedPckVerFunc->cPckXorKeys->IndexCompressedFilenameDataLengthCryptKey1, \
		m_PckAllInfo.lpDetectedPckVerFunc->cPckXorKeys->IndexCompressedFilenameDataLengthCryptKey2};

	//pck是压缩时，文件名的压缩长度不会超过0x100，所以当
	//开始一个字节，如果0x75，就没有压缩，如果是0x74就是压缩的	0x75->FILEINDEX_LEVEL0
	lpRead->SetFilePointer(m_PckAllInfo.dwAddressName, FILE_BEGIN);

	byteLevelKey = (*(DWORD*)lpFileBuffer) ^ IndexCompressedFilenameDataLengthCryptKey[0];
	isLevel0 = (m_PckAllInfo.lpDetectedPckVerFunc->dwFileIndexSize == byteLevelKey)/* ? TRUE : FALSE*/;

	if(isLevel0){

		for(DWORD i = 0;i<m_PckAllInfo.dwFileCount;i++){
			//先复制两个压缩数据长度信息
			memcpy(dwFileIndexTableCryptedDataLength, lpFileBuffer, 8);

			*(QWORD*)dwFileIndexTableCryptedDataLength ^= *(QWORD*)IndexCompressedFilenameDataLengthCryptKey;

			if( dwFileIndexTableCryptedDataLength[0] != dwFileIndexTableCryptedDataLength[1]){

				PrintLogE(TEXT_READ_INDEX_FAIL, __FILE__, __FUNCTION__, __LINE__);
				delete lpRead;
				return FALSE;
			}

			lpFileBuffer += 8;

			m_PckAllInfo.lpDetectedPckVerFunc->PickIndexData(&lpPckIndexTable->cFileIndex, lpFileBuffer);

			lpFileBuffer += dwFileIndexTableClearDataLength;
			++lpPckIndexTable;

		}
	}else{

		for(DWORD i = 0;i<m_PckAllInfo.dwFileCount;++i){

			memcpy(dwFileIndexTableCryptedDataLength, lpFileBuffer, 8);
			*(QWORD*)dwFileIndexTableCryptedDataLength ^= *(QWORD*)IndexCompressedFilenameDataLengthCryptKey;
			lpFileBuffer += 8;

			if( dwFileIndexTableCryptedDataLength[0] != dwFileIndexTableCryptedDataLength[1]){

				PrintLogE(TEXT_READ_INDEX_FAIL, __FILE__, __FUNCTION__, __LINE__);
				delete lpRead;
				return FALSE;
			}

			DWORD dwFileBytesRead = dwFileIndexTableClearDataLength;
			BYTE pckFileIndexBuf[MAX_INDEXTABLE_CLEARTEXT_LENGTH];

			decompress(pckFileIndexBuf, &dwFileBytesRead,
						lpFileBuffer, dwFileIndexTableCryptedDataLength[0]);


			m_PckAllInfo.lpDetectedPckVerFunc->PickIndexData(&lpPckIndexTable->cFileIndex, pckFileIndexBuf);

			lpFileBuffer += dwFileIndexTableCryptedDataLength[0];
			++lpPckIndexTable;

		}
	}

	delete lpRead;
	return TRUE;
}