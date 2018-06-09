
#include "PckClassBodyWriter.h"

CPckClassBodyWriter::CPckClassBodyWriter()
{}

CPckClassBodyWriter::~CPckClassBodyWriter()
{}

//获取数据压缩后的大小，如果源大小小于一定值就不压缩
DWORD CPckClassBodyWriter::GetCompressBoundSizeByFileSize(LPPCKFILEINDEX	lpPckFileIndex, DWORD dwFileSize)
{
	if(PCK_BEGINCOMPRESS_SIZE < dwFileSize) {
		lpPckFileIndex->dwFileClearTextSize = dwFileSize;
		lpPckFileIndex->dwFileCipherTextSize = compressBound(dwFileSize);
	} else {
		lpPckFileIndex->dwFileCipherTextSize = lpPckFileIndex->dwFileClearTextSize = dwFileSize;
	}

	return lpPckFileIndex->dwFileCipherTextSize;
}