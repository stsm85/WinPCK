
//////////////////////////////////////////////////////////////////////
// PckControlCenterInterface.cpp: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 头文件,界面与PCK类的数据交互，控制中心，日志功能
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2015.5.19
//////////////////////////////////////////////////////////////////////

#include "PckControlCenter.h"
#include "PckClass.h"

#pragma region 文件打开关闭信息界面交互

FeedbackCallback CPckControlCenter::pFeedbackCallBack = DefaultFeedbackCallback;
void* CPckControlCenter::pTag = NULL;

void CPckControlCenter::regMsgFeedback(void* _pTag, FeedbackCallback _FeedbackCallBack)
{
	pTag = _pTag;
	if(NULL != _FeedbackCallBack)
		pFeedbackCallBack = _FeedbackCallBack;
}

int CPckControlCenter::DefaultFeedbackCallback(void* pTag, int32_t eventId, size_t wParam, ssize_t lParam)
{
	wchar_t szTitle[MAX_PATH] = { 0 };

	switch (eventId)
	{
	case PCK_FILE_OPEN_SUCESS:

		GetConsoleTitleW(szTitle, MAX_PATH);
		wcscat_s(szTitle, L" - ");
		wcscat_s(szTitle, (const wchar_t*)lParam);
		SetConsoleTitleW(szTitle);

		break;

	case PCK_FILE_CLOSE:
		SetConsoleTitleA("WinPCK");

		break;

	default:
		break;
	}

	return 0;
}

#pragma endregion

#pragma region 查询及目录浏览

void CPckControlCenter::DefaultShowFilelistCallback(void* _in_param, int sn, LPCWSTR lpszFilename, int entry_type, uint64_t qwFileSize, uint64_t qwFileSizeCompressed, void* fileEntry)
{
	auto fix_print_str = [](int nTabs, LPCWSTR str) {
		
		wprintf(str);

		int nSubTabs = wcslen(str) / 8;

		if (nTabs < nSubTabs)
			nTabs = 0;
		else
			nTabs -= nSubTabs;
		for (int i = 0; i < nTabs; i++) {
			printf("\t");
		}
	};

	auto fix_print_qword = [](int nTabs, unsigned __int64 num) {

		wchar_t szQword2Str[32];
		swprintf_s(szQword2Str, L"%llu", num);

		int len = wcslen(szQword2Str);
		int nSubTabs = (len-1) / 8;
		int nModnum = 8 - len % 8;

		if (8 != nModnum) {
			memmove_s(szQword2Str + nModnum, sizeof(szQword2Str), szQword2Str, sizeof(wchar_t) * (len + 1));
			for (int i = 0; i < nModnum; i++) {
				szQword2Str[i] = ' ';
			}
		}

		if (nTabs < nSubTabs)
			nTabs = 0;
		else
			nTabs -= nSubTabs;
		for (int i = 0; i < nTabs; i++) {
			printf("\t");
		}

		wprintf(szQword2Str);
	};

	fix_print_str(4, lpszFilename);

	printf("%s\t", (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & entry_type)) ? "Folder" : "File");

	fix_print_qword(1, qwFileSize);
	fix_print_qword(1, qwFileSizeCompressed);

	printf("\r\n");
}

uint32_t CPckControlCenter::SearchByName(LPCWSTR lpszSearchString, void* _in_param, SHOW_LIST_CALLBACK _showListCallback)
{
	if (NULL == m_lpClassPck)
		return 0;

	SHOW_LIST_CALLBACK _showList = _showListCallback;

	if (NULL == _showListCallback) {
		_showList = DefaultShowFilelistCallback;
	}

	uint32_t	dwFileCount = GetPckFileCount(), dwFoundCount = 0;
	const PCKINDEXTABLE	* lpPckIndexTable = m_lpClassPck->GetPckIndexTable();

	//打印顶层
	_showList(_in_param,
		dwFoundCount++,
		L"<--",
		PCK_ENTRY_TYPE_DOTDOT,
		0,
		0,
		(void*)GetRootNode());

	for (uint32_t i = 0; i < dwFileCount; i++) {
		//while(PCK_ENTRY_TYPE_TAIL_INDEX != lpPckIndexTable->entryType){
		if (NULL != wcsstr(lpPckIndexTable->cFileIndex.szwFilename, lpszSearchString)) {

			_showList(_in_param,
				dwFoundCount,
				lpPckIndexTable->cFileIndex.szwFilename,
				lpPckIndexTable->entryType,
				lpPckIndexTable->cFileIndex.dwFileClearTextSize,
				lpPckIndexTable->cFileIndex.dwFileCipherTextSize,
				(void*)lpPckIndexTable);

			dwFoundCount++;
		}
		lpPckIndexTable++;
	}
	return dwFoundCount;
}

uint32_t CPckControlCenter::ListByNode(LPCENTRY lpFileEntry, void* _in_param, SHOW_LIST_CALLBACK _showListCallback)
{
	if (NULL == lpFileEntry)
		return 0;

	SHOW_LIST_CALLBACK _showList = _showListCallback;

	if (NULL == _showListCallback) {
		_showList = DefaultShowFilelistCallback;
	}

	//如果是..文件夹，就显示上一层，不是就显示下一层
	const PCK_PATH_NODE* lpNodeToShow = (LPPCK_PATH_NODE)lpFileEntry;

	int entry_type = lpFileEntry->entryType;
	//首先要是文件夹
	if (PCK_ENTRY_TYPE_FOLDER != (PCK_ENTRY_TYPE_FOLDER & entry_type)) {
#if PCK_DEBUG_OUTPUT
		printf("%s:is not a folder\n", __FUNCTION__);
#endif
		return 0;
	}

	//进入非..文件夹
	if (PCK_ENTRY_TYPE_DOTDOT != (PCK_ENTRY_TYPE_DOTDOT & entry_type)){

		lpNodeToShow = ((LPPCK_PATH_NODE)lpFileEntry)->child;

	}//剩下的是..文件夹
	else {
		lpNodeToShow = ((LPPCK_PATH_NODE)lpFileEntry)->parentfirst;
	}

	//lpNodeToShow是否为NULL
	if (NULL == lpNodeToShow) {
#if PCK_DEBUG_OUTPUT
		printf("%s:lpNodeToShow is NULL\n", __FUNCTION__);
#endif
		return 0;
	}

	uint32_t dwSerialNumber = 0;

	const PCK_PATH_NODE* lpNodeToShowPtr = lpNodeToShow;

	while (NULL != lpNodeToShowPtr && 0 != *lpNodeToShowPtr->szName) {
		//这里先显示文件夹
		int entryType = lpNodeToShowPtr->entryType;
		if ((PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & entryType))) {

			if (NULL != lpNodeToShowPtr->child) {
				_showList(_in_param,
					dwSerialNumber,
					lpNodeToShowPtr->szName,
					entryType,
					lpNodeToShowPtr->child->qdwDirClearTextSize,
					lpNodeToShowPtr->child->qdwDirCipherTextSize,
					(void*)lpNodeToShowPtr);
			}
			else {
				_showList(_in_param,
					dwSerialNumber,
					lpNodeToShowPtr->szName,
					entryType,
					0,
					0,
					(void*)lpNodeToShowPtr);
			}

			dwSerialNumber++;
		}
		lpNodeToShowPtr = lpNodeToShowPtr->next;
	}

	lpNodeToShowPtr = lpNodeToShow;
	while (NULL != lpNodeToShowPtr && 0 != *lpNodeToShowPtr->szName) {
		if ((PCK_ENTRY_TYPE_FOLDER != (PCK_ENTRY_TYPE_FOLDER & lpNodeToShowPtr->entryType))) {
			const PCKINDEXTABLE* lpPckIndexTable = lpNodeToShowPtr->lpPckIndexTable;

			_showList(_in_param,
				dwSerialNumber,
				lpNodeToShowPtr->szName,
				lpPckIndexTable->entryType,
				lpPckIndexTable->cFileIndex.dwFileClearTextSize,
				lpPckIndexTable->cFileIndex.dwFileCipherTextSize,
				(void*)lpNodeToShowPtr);

			dwSerialNumber++;
		}

		lpNodeToShowPtr = lpNodeToShowPtr->next;

	}
	return dwSerialNumber;
}

#pragma endregion
