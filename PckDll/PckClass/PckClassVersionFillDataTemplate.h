#pragma once

//文件头、尾的数据填写和数据写入
template<typename T>
void* FillHeadData(void* param)
{
	LPPCK_ALL_INFOS lpPckAllInfo = (LPPCK_ALL_INFOS)param;
	static DWORD headbuf[3];
	T* lpHead = (T*)headbuf;
	lpHead->dwHeadCheckHead = lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.HeadVerifyKey1;
	if (lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.HeadVerifyKey2) {
		lpHead->dwHeadCheckTail = lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.HeadVerifyKey2;
		lpHead->dwPckSize = (DWORD)lpPckAllInfo->qwPckSize;
	}
	else {
		lpHead->dwPckSize = lpPckAllInfo->qwPckSize;
	}
	return headbuf;
}

template<typename T>
void* FillTailData(void* param)
{
	LPPCK_ALL_INFOS lpPckAllInfo = (LPPCK_ALL_INFOS)param;
	static BYTE tailbuf[MAX_TAIL_LENGTH] = { 0 };
	T* lpTail = (T*)tailbuf;
	lpTail->dwIndexTableCheckHead = lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.TailVerifyKey1;
	lpTail->dwVersion0 = lpTail->dwVersion = lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.Version;
	lpTail->dwEntryOffset = lpPckAllInfo->dwAddressOfFileEntry ^ lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.IndexesEntryAddressCryptKey;
	//lpTail->dwFlags = 0;
	memcpy(lpTail->szAdditionalInfo, lpPckAllInfo->szAdditionalInfo, sizeof(lpTail->szAdditionalInfo));
	lpTail->dwIndexTableCheckTail = lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.TailVerifyKey2;
	lpTail->dwFileCount = lpPckAllInfo->dwFinalFileCount;
	return tailbuf;
}

//数据从param -> lpTail
template<typename T>
void* FillIndexData(void* lpFileIndexParam, void* lpPckIndexTableClearParam)
{
	LPPCKFILEINDEX lpFileIndex = (LPPCKFILEINDEX)lpFileIndexParam;
	T* lpPckIndexTableClear = (T*)lpPckIndexTableClearParam;
	memcpy(lpPckIndexTableClear->szFilename, lpFileIndex->szFilename, sizeof(lpPckIndexTableClear->szFilename));
	lpPckIndexTableClear->dwUnknown1 = lpPckIndexTableClear->dwUnknown2 = 0;
	lpPckIndexTableClear->dwAddressOffset = lpFileIndex->dwAddressOffset;
	lpPckIndexTableClear->dwFileCipherTextSize = lpFileIndex->dwFileCipherTextSize;
	lpPckIndexTableClear->dwFileClearTextSize = lpFileIndex->dwFileClearTextSize;
	return (void*)lpPckIndexTableClear;
}

//数据从lpIndex -> param
template<typename T>
BOOL PickIndexData(void* lpFileIndexParam, void* lpPckIndexTableClearParam)
{
	LPPCKFILEINDEX lpFileIndex = (LPPCKFILEINDEX)lpFileIndexParam;
	T* lpPckIndexTableClear = (T*)lpPckIndexTableClearParam;
	lpFileIndex->dwAddressOffset = lpPckIndexTableClear->dwAddressOffset;
	lpFileIndex->dwFileCipherTextSize = lpPckIndexTableClear->dwFileCipherTextSize;
	lpFileIndex->dwFileClearTextSize = lpPckIndexTableClear->dwFileClearTextSize;
	memcpy(lpFileIndex->szFilename, lpPckIndexTableClear->szFilename, sizeof(lpPckIndexTableClear->szFilename));
	return TRUE;
}

//template
//void* FillIndexData<LPPCKFILEINDEX_V2020>(void *param, LPPCKFILEINDEX_V2020 lpPckIndexTableClear);
