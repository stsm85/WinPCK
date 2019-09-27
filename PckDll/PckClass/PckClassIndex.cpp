
#include "PckClassIndex.h"
#include <thread>
#include <vector>

CPckClassIndex::CPckClassIndex()
{}

CPckClassIndex::~CPckClassIndex()
{
	if(NULL != m_PckAllInfo.lpPckIndexTable)
		free(m_PckAllInfo.lpPckIndexTable);
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

void* CPckClassIndex::AllocMemory(size_t	sizeStuct)
{
	void*		lpMallocNode;

	if(NULL == (lpMallocNode = malloc(sizeStuct))) {
		SetErrMsgFlag(PCK_ERR_MALLOC);
		return NULL;
	}
	//初始化内存
	memset(lpMallocNode, 0, sizeStuct);
	return lpMallocNode;
}

LPPCKINDEXTABLE	CPckClassIndex::AllocPckIndexTableByFileCount()
{
	//这里给m_PckAllInfo.dwFileCount + 1，最后一个1用来做为标记index的结束，其entryType置为PCK_ENTRY_TYPE_TAIL_INDEX
	//PCK_ENTRY_TYPE_TAIL_INDEX，此index不存放数据，仅为标记结束作用
	return (LPPCKINDEXTABLE)AllocMemory(sizeof(PCKINDEXTABLE) * (m_PckAllInfo.dwFileCount + 1));
}

void CPckClassIndex::GenerateUnicodeStringToIndex()
{
	LPPCKINDEXTABLE lpPckIndexTable = m_PckAllInfo.lpPckIndexTable;

	for(DWORD i = 0;i < m_PckAllInfo.dwFileCount;++i) {
		//文件名长度
		lpPckIndexTable->nFilelenBytes = strlen(lpPckIndexTable->cFileIndex.szFilename);
		//文件名剩余空间,不占用最后的\0
		lpPckIndexTable->nFilelenLeftBytes = MAX_PATH_PCK_256 - lpPckIndexTable->nFilelenBytes - 1;
		//pck ansi -> unicode
		CPckClassCodepage::PckFilenameCode2UCS(lpPckIndexTable->cFileIndex.szFilename, lpPckIndexTable->cFileIndex.szwFilename, sizeof(lpPckIndexTable->cFileIndex.szwFilename) / sizeof(wchar_t));
		++lpPckIndexTable;
	}

}

//重建时计算有效文件数量，排除重复的文件
DWORD CPckClassIndex::ReCountFiles()
{
	DWORD deNewFileCount = 0;
	LPPCKINDEXTABLE lpPckIndexTableSource = m_PckAllInfo.lpPckIndexTable;

	for(DWORD i = 0; i < m_PckAllInfo.dwFileCount; ++i) {

		if(!lpPckIndexTableSource->isInvalid) {
			++deNewFileCount;
		}
		++lpPckIndexTableSource;
	}
	return deNewFileCount;
}

//按修改后的索引数据按版本填入到结构体中并压缩好
LPPCKINDEXTABLE_COMPRESS CPckClassIndex::FillAndCompressIndexData(LPPCKINDEXTABLE_COMPRESS lpPckIndexTableComped, LPPCKFILEINDEX lpPckFileIndexToCompress)
{
	BYTE pckFileIndexBuf[MAX_INDEXTABLE_CLEARTEXT_LENGTH];
	lpPckIndexTableComped->dwIndexDataLength = MAX_INDEXTABLE_CLEARTEXT_LENGTH;

	m_zlib.compress(lpPckIndexTableComped->buffer, &lpPckIndexTableComped->dwIndexDataLength,
		m_PckAllInfo.lpSaveAsPckVerFunc->FillIndexData(lpPckFileIndexToCompress, pckFileIndexBuf), m_PckAllInfo.lpSaveAsPckVerFunc->dwFileIndexSize);
	//将获取的
	lpPckIndexTableComped->dwIndexValueHead = lpPckIndexTableComped->dwIndexDataLength ^ m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.IndexCompressedFilenameDataLengthCryptKey1;
	lpPckIndexTableComped->dwIndexValueTail = lpPckIndexTableComped->dwIndexDataLength ^ m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.IndexCompressedFilenameDataLengthCryptKey2;

	return lpPckIndexTableComped;
}