#include "PckClassWriteOperator.h"

//重命名文件
BOOL CPckClassWriteOperator::RenameFilename()
{
	m_zlib.init_compressor(m_lpPckParams->dwCompressLevel);
	Logger.i(TEXT_LOG_RENAME);

	//以下是创建一个文件，用来保存压缩后的文件
	CMapViewFileMultiPckWrite cFileWrite(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.dwMaxSinglePckSize);

	if(!cFileWrite.OpenPck(m_PckAllInfo.szFilename, OPEN_EXISTING)) {

		Logger_el(UCSTEXT(TEXT_OPENWRITENAME_FAIL), m_PckAllInfo.szFilename);
		assert(FALSE);
		return FALSE;
	}

	QWORD dwFileSize = GetPckFilesizeRename(m_PckAllInfo.szFilename, cFileWrite.GetFileSize());

	if(!cFileWrite.Mapping(dwFileSize)) {

		Logger_el(UCSTEXT(TEXT_CREATEMAPNAME_FAIL), m_PckAllInfo.szFilename);
		assert(FALSE);
		return FALSE;
	}

	//写文件索引
	QWORD dwAddress = m_PckAllInfo.dwAddressOfFileEntry;

	WriteAllIndex(&cFileWrite, &m_PckAllInfo, dwAddress);
	
	WriteHeadAndTail(&cFileWrite, &m_PckAllInfo, dwAddress, FALSE);

	Logger.i(TEXT_LOG_WORKING_DONE);

	return TRUE;
}