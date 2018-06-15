#include "PckClassWriteOperator.h"

//重命名文件
BOOL CPckClassWriteOperator::RenameFilename()
{
	init_compressor();
	m_PckLog.PrintLogI(TEXT_LOG_RENAME);

	//以下是创建一个文件，用来保存压缩后的文件
	CMapViewFileWrite cFileWrite(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->dwMaxSinglePckSize);

	if(!cFileWrite.OpenPck(m_PckAllInfo.szFilename, OPEN_EXISTING)) {

		m_PckLog.PrintLogEL(TEXT_OPENWRITENAME_FAIL, m_PckAllInfo.szFilename, __FILE__, __FUNCTION__, __LINE__);
		assert(FALSE);
		return FALSE;
	}

	QWORD dwFileSize = GetPckFilesizeRename(m_PckAllInfo.szFilename, cFileWrite.GetFileSize());

	if(!cFileWrite.Mapping(dwFileSize)) {

		m_PckLog.PrintLogEL(TEXT_CREATEMAPNAME_FAIL, m_PckAllInfo.szFilename, __FILE__, __FUNCTION__, __LINE__);
		assert(FALSE);
		return FALSE;
	}

	//写文件索引
	QWORD dwAddress = m_PckAllInfo.dwAddressOfFilenameIndex;

	WriteAllIndex(&cFileWrite, &m_PckAllInfo, dwAddress);
	
	AfterProcess(&cFileWrite, m_PckAllInfo, dwAddress, FALSE);

	m_PckLog.PrintLogI(TEXT_LOG_WORKING_DONE);

	return TRUE;
}