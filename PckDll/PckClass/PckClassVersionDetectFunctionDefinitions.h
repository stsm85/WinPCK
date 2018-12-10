#pragma once

#define define_get_pckAllInfo_by_version(_ver, _id)	\
		PCKTAIL_V##_ver PckTail_v##_ver; \
		cRead.SetFilePointer(qwPckSizeInHeader - ((QWORD)(sizeof(PCKTAIL_V##_ver))), FILE_BEGIN); \
		if (!cRead.Read(&PckTail_v##_ver, sizeof(PCKTAIL_V##_ver))) { \
			m_PckLog.PrintLogEL(TEXT_READFILE_FAIL, __FILE__, __FUNCTION__, __LINE__); \
			goto dect_err; \
		} \
		if (cPckVersionFunc[(_id)].cPckXorKeys.TailVerifyKey1 == PckTail_v##_ver.dwIndexTableCheckHead) { \
			isFoundVer = TRUE; \
			m_PckAllInfo.qwPckSize = ((LPPCKHEAD_V##_ver)&cPckHead)->dwPckSize; \
			m_PckAllInfo.dwFinalFileCount = m_PckAllInfo.dwFileCountOld = m_PckAllInfo.dwFileCount = dwTailVals[2];\
			m_PckAllInfo.dwAddressOfFilenameIndex = PckTail_v##_ver.dwCryptedFileIndexesAddr ^ cPckVersionFunc[(_id)].cPckXorKeys.IndexesEntryAddressCryptKey;\
			memcpy(m_PckAllInfo.szAdditionalInfo, PckTail_v##_ver.szAdditionalInfo, PCK_ADDITIONAL_INFO_SIZE);\
			_tcscpy_s(m_PckAllInfo.szFilename, lpszPckFile); \
			break; \
		} \


/*
#define define_get_pckAllInfo_by_version(_ver, _id)	\
		PCKTAIL_V##_ver PckTail_v##_ver; \
		cRead.SetFilePointer(-((QWORD)(sizeof(PCKTAIL_V##_ver))), FILE_END); \
		if (!cRead.Read(&PckTail_v##_ver, sizeof(PCKTAIL_V##_ver))) { \
			m_PckLog.PrintLogEL(TEXT_READFILE_FAIL, __FILE__, __FUNCTION__, __LINE__); \
			goto dect_err; \
		} \
		for (int i = 0; i < PCK_VERSION_ALL_NUMS; ++i) { \
			if (cPckKeys[i].VersionId == ver) { \
				if (cPckKeys[i].TailVerifyKey1 == PckTail_v##_ver.dwIndexTableCheckHead) { \
					isFoundVer = TRUE; \
					(_id) = i; \
					m_PckAllInfo.qwPckSize = ((LPPCKHEAD_V##_ver)&cPckHead)->dwPckSize; \
					m_PckAllInfo.dwFinalFileCount = m_PckAllInfo.dwFileCountOld = m_PckAllInfo.dwFileCount = dwTailVals[2];\
					m_PckAllInfo.dwAddressOfFilenameIndex = PckTail_v##_ver.dwCryptedFileIndexesAddr ^ cPckKeys[(_id)].IndexesEntryAddressCryptKey;\
					memcpy(m_PckAllInfo.szAdditionalInfo, PckTail_v##_ver.szAdditionalInfo, PCK_ADDITIONAL_INFO_SIZE);\
					_tcscpy_s(m_PckAllInfo.szFilename, lpszPckFile); \
					break; \
				} \
			} \
		}
*/

#define define_one_FillIndexData_by_version(_ver)	\
		LPVOID CPckClassVersionDetect::FillIndexData_V##_ver(void *param, void *pckFileIndexBuf) \
		{ \
			LPPCKFILEINDEX lpFileIndex = (LPPCKFILEINDEX)param; \
			LPPCKFILEINDEX_V##_ver lpPckIndexTableClear = (LPPCKFILEINDEX_V##_ver)pckFileIndexBuf; \
			memcpy(lpPckIndexTableClear->szFilename, lpFileIndex->szFilename, sizeof(lpPckIndexTableClear->szFilename)); \
			lpPckIndexTableClear->dwUnknown1 = lpPckIndexTableClear->dwUnknown2 = 0; \
			lpPckIndexTableClear->dwAddressOffset = lpFileIndex->dwAddressOffset; \
			lpPckIndexTableClear->dwFileCipherTextSize = lpFileIndex->dwFileCipherTextSize; \
			lpPckIndexTableClear->dwFileClearTextSize = lpFileIndex->dwFileClearTextSize; \
			return pckFileIndexBuf; \
		}


#define define_one_PickIndexData_by_version(_ver)	\
		BOOL CPckClassVersionDetect::PickIndexData_V##_ver(void *param, void* lpIndex) \
		{ \
			LPPCKFILEINDEX lpFileIndex = (LPPCKFILEINDEX)param; \
			LPPCKFILEINDEX_V##_ver lpPckIndexTableClear = (LPPCKFILEINDEX_V##_ver)lpIndex; \
			lpFileIndex->dwAddressOffset = lpPckIndexTableClear->dwAddressOffset; \
			lpFileIndex->dwFileCipherTextSize = lpPckIndexTableClear->dwFileCipherTextSize; \
			lpFileIndex->dwFileClearTextSize = lpPckIndexTableClear->dwFileClearTextSize; \
			memcpy(lpFileIndex->szFilename, lpPckIndexTableClear->szFilename, sizeof(lpPckIndexTableClear->szFilename)); \
			return TRUE; \
		}

#define define_one_FillHeadData_by_version(_ver)	\
		LPVOID CPckClassVersionDetect::FillHeadData_V##_ver(void *param) \
		{ \
			static DWORD headbuf[3]; \
			LPPCK_ALL_INFOS lpPckAllInfo = (LPPCK_ALL_INFOS)param; \
			LPPCKHEAD_V##_ver	lpHead = (LPPCKHEAD_V##_ver)headbuf; \
			lpHead->dwHeadCheckHead = lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.HeadVerifyKey1; \
			if (lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.HeadVerifyKey2) { \
				lpHead->dwHeadCheckTail = lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.HeadVerifyKey2; \
				lpHead->dwPckSize = (DWORD)lpPckAllInfo->qwPckSize; \
			} else { \
				lpHead->dwPckSize = lpPckAllInfo->qwPckSize; \
			} \
			return headbuf; \
		}

#define define_one_FillTailData_by_version(_ver)	\
		LPVOID CPckClassVersionDetect::FillTailData_V##_ver(void *param) \
		{ \
			static BYTE tailbuf[MAX_TAIL_LENGTH]; \
			LPPCK_ALL_INFOS lpPckAllInfo = (LPPCK_ALL_INFOS)param; \
			LPPCKTAIL_V##_ver lpTail = (LPPCKTAIL_V##_ver)tailbuf; \
			lpTail->dwIndexTableCheckHead = lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.TailVerifyKey1; \
			lpTail->dwVersion0 = lpTail->dwVersion = lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.Version; \
			lpTail->dwCryptedFileIndexesAddr = lpPckAllInfo->dwAddressOfFilenameIndex ^ lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.IndexesEntryAddressCryptKey; \
			lpTail->dwNullDword = 0; \
			memcpy(lpTail->szAdditionalInfo, lpPckAllInfo->szAdditionalInfo, sizeof(lpTail->szAdditionalInfo)); \
			lpTail->dwIndexTableCheckTail = lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.TailVerifyKey2; \
			lpTail->dwFileCount = lpPckAllInfo->dwFinalFileCount; \
			return tailbuf; \
		}
