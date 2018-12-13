#include "PckClassIndex.h"
#include "PckClassZlib.h"


BOOL CPckClassIndex::ReadPckFileIndexes()
{
	CMapViewFileMultiPckRead cRead;

	if(!cRead.OpenPckAndMappingRead(m_PckAllInfo.szFilename)) {
		m_PckLog.PrintLogEL(TEXT_OPENNAME_FAIL, m_PckAllInfo.szFilename, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	if(!(NULL != (m_PckAllInfo.lpPckIndexTable = AllocPckIndexTableByFileCount()))) {
		return FALSE;
	}

	//开始读文件
	BYTE	*lpFileBuffer;
	if(NULL == (lpFileBuffer = cRead.View(m_PckAllInfo.dwAddressOfFilenameIndex, cRead.GetFileSize() - m_PckAllInfo.dwAddressOfFilenameIndex))) {
		m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	LPPCKINDEXTABLE lpPckIndexTable = m_PckAllInfo.lpPckIndexTable;
	BOOL			isLevel0;
	DWORD			byteLevelKey;
	//存放每条文件索引的头部的两个DWORD压缩数据长度信息
	DWORD			dwFileIndexTableCryptedDataLength[2];
	DWORD			dwFileIndexTableClearDataLength = m_PckAllInfo.lpDetectedPckVerFunc->dwFileIndexSize;
	DWORD			IndexCompressedFilenameDataLengthCryptKey[2] = { \
		m_PckAllInfo.lpDetectedPckVerFunc->cPckXorKeys.IndexCompressedFilenameDataLengthCryptKey1, \
		m_PckAllInfo.lpDetectedPckVerFunc->cPckXorKeys.IndexCompressedFilenameDataLengthCryptKey2 };

	//pck是压缩时，文件名的压缩长度不会超过0x100，所以当
	//开始一个字节，如果0x75，就没有压缩，如果是0x74就是压缩的	0x75->FILEINDEX_LEVEL0
	//cRead.SetFilePointer(m_PckAllInfo.dwAddressOfFilenameIndex, FILE_BEGIN);

	byteLevelKey = (*(DWORD*)lpFileBuffer) ^ IndexCompressedFilenameDataLengthCryptKey[0];
	isLevel0 = (m_PckAllInfo.lpDetectedPckVerFunc->dwFileIndexSize == byteLevelKey)/* ? TRUE : FALSE*/;

	if(isLevel0) {

		for(DWORD i = 0;i < m_PckAllInfo.dwFileCount;i++) {
			//先复制两个压缩数据长度信息
			memcpy(dwFileIndexTableCryptedDataLength, lpFileBuffer, 8);

			*(QWORD*)dwFileIndexTableCryptedDataLength ^= *(QWORD*)IndexCompressedFilenameDataLengthCryptKey;

			if(dwFileIndexTableCryptedDataLength[0] != dwFileIndexTableCryptedDataLength[1]) {

				m_PckLog.PrintLogEL(TEXT_READ_INDEX_FAIL, __FILE__, __FUNCTION__, __LINE__);
				return FALSE;
			}

			lpFileBuffer += 8;

			m_PckAllInfo.lpDetectedPckVerFunc->PickIndexData(&lpPckIndexTable->cFileIndex, lpFileBuffer);
			lpFileBuffer += dwFileIndexTableClearDataLength;
			++lpPckIndexTable;

		}
	} else {

		for(DWORD i = 0;i < m_PckAllInfo.dwFileCount;++i) {

			memcpy(dwFileIndexTableCryptedDataLength, lpFileBuffer, 8);
			*(QWORD*)dwFileIndexTableCryptedDataLength ^= *(QWORD*)IndexCompressedFilenameDataLengthCryptKey;
			lpFileBuffer += 8;

			if(dwFileIndexTableCryptedDataLength[0] != dwFileIndexTableCryptedDataLength[1]) {

				m_PckLog.PrintLogEL(TEXT_READ_INDEX_FAIL, __FILE__, __FUNCTION__, __LINE__);
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

	return TRUE;
}