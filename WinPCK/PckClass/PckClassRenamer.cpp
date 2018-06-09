#include "PckClassRenamer.h"
#include "PckClassFileDisk.h"

CPckClassRenamer::CPckClassRenamer()
{}

CPckClassRenamer::~CPckClassRenamer()
{}

//重命名文件
BOOL CPckClassRenamer::RenameFilename()
{

	m_PckLog.PrintLogI(TEXT_LOG_RENAME);

	int			level = m_lpPckParams->dwCompressLevel;
	DWORD		IndexCompressedFilenameDataLengthCryptKey1 = m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->IndexCompressedFilenameDataLengthCryptKey1;
	DWORD		IndexCompressedFilenameDataLengthCryptKey2 = m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->IndexCompressedFilenameDataLengthCryptKey2;

	//以下是创建一个文件，用来保存压缩后的文件
	CMapViewFileWrite cFileWrite(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->dwMaxSinglePckSize);

	if(!cFileWrite.OpenPck(m_PckAllInfo.szFilename, OPEN_EXISTING)) {

		m_PckLog.PrintLogEL(TEXT_OPENWRITENAME_FAIL, m_PckAllInfo.szFilename, __FILE__, __FUNCTION__, __LINE__);
		assert(FALSE);
		return FALSE;
	}

	QWORD dwFileSize = GetPckFilesizeRename(m_PckAllInfo.szFilename, cFileWrite.GetFileSize());//cFileWrite.GetFileSize() + PCK_RENAME_EXPAND_ADD;

	if(!cFileWrite.Mapping(dwFileSize)) {

		m_PckLog.PrintLogEL(TEXT_CREATEMAPNAME_FAIL, m_PckAllInfo.szFilename, __FILE__, __FUNCTION__, __LINE__);
		assert(FALSE);
		return FALSE;
	}

	//写文件索引
	QWORD dwAddress = m_PckAllInfo.dwAddressOfFilenameIndex;

	//窗口中以显示的文件进度，初始化，显示写索引进度mt_hFileToWrite
	m_lpPckParams->cVarParams.dwUIProgress = 0;
	m_lpPckParams->cVarParams.dwUIProgressUpper = m_PckAllInfo.dwFileCount;

	//写原来的文件
	LPPCKINDEXTABLE	lpPckIndexTableSrc = m_PckAllInfo.lpPckIndexTable;

	//写入索引
	for(DWORD i = 0; i < m_lpPckParams->cVarParams.dwUIProgressUpper; ++i) {

		PCKINDEXTABLE_COMPRESS	pckIndexTableTemp;

		if(lpPckIndexTableSrc->bSelected) {
			--m_PckAllInfo.dwFileCount;
			++lpPckIndexTableSrc;

			//窗口中以显示的文件进度
			++m_lpPckParams->cVarParams.dwUIProgress;
			continue;
		}

		FillAndCompressIndexData(&pckIndexTableTemp, &lpPckIndexTableSrc->cFileIndex);

		WritePckIndex(&cFileWrite, &pckIndexTableTemp, dwAddress);

		++lpPckIndexTableSrc;

	}

	AfterProcess(&cFileWrite, m_PckAllInfo, dwAddress, FALSE);

	m_PckLog.PrintLogI(TEXT_LOG_WORKING_DONE);

	return TRUE;
}