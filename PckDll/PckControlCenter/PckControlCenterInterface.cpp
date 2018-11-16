
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

int CPckControlCenter::DefaultFeedbackCallback(void* pTag, int eventId, WPARAM wParam, LPARAM lParam)
{
	wchar_t szTitle[MAX_PATH] = { 0 };
	char szInput;

	switch (eventId)
	{
	case PCK_FILE_OPEN_SUCESS:

		GetConsoleTitleW(szTitle, MAX_PATH);
		wcscat_s(szTitle, (const wchar_t*)lParam);
		SetConsoleTitleW(szTitle);

		break;

	case PCK_FILE_CLOSE:
		SetConsoleTitleA("WinPCK");

		break;

	case PCK_FILE_NEED_RESTORE:
		
		printf("fail to open file\r\n"
			"Maybe the last operation resulted in the damage of the file.\r\nAre you trying to restore to the last open state? (y/n)");
		do {
			scanf("%c\n", &szInput);
		} while (NULL != strchr("yYnN", szInput));

		if (('y' == szInput) || ('Y' == szInput))
			return PCK_FEEDBACK_YES;
		else
			return PCK_FEEDBACK_NO;

		break;
	}

	return PCK_FEEDBACK_YES;
}

#pragma endregion

#pragma region 查询及目录浏览

void CPckControlCenter::DefaultShowFilelistCallback(void* _in_param, int sn, const wchar_t *lpszFilename, int entry_type, unsigned __int64 qwFileSize, unsigned __int64 qwFileSizeCompressed, void* fileEntry)
{
	wprintf(L"%s\t%s\t%llu\t%llu\r\n", lpszFilename, (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & entry_type)) ? L"Folder" : L"File", qwFileSize, qwFileSizeCompressed);
}

DWORD CPckControlCenter::SearchByName(const wchar_t* lpszSearchString, void* _in_param, SHOW_LIST_CALLBACK _showListCallback)
{
	if (NULL == m_lpClassPck)
		return 0;

	SHOW_LIST_CALLBACK _showList = _showListCallback;

	if (NULL == _showListCallback) {
		_showList = DefaultShowFilelistCallback;
	}

	DWORD	dwFileCount = GetPckFileCount(), dwFoundCount = 0;
	const PCKINDEXTABLE	* lpPckIndexTable = m_lpClassPck->GetPckIndexTable();

	//打印顶层
	_showList(_in_param,
		dwFoundCount++,
		L"<--",
		PCK_ENTRY_TYPE_DOTDOT,
		0,
		0,
		(void*)GetRootNode());

	for (DWORD i = 0; i < dwFileCount; i++) {
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

DWORD CPckControlCenter::ListByNode(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry, void* _in_param, SHOW_LIST_CALLBACK _showListCallback)
{
	if (NULL == lpFileEntry)
		return 0;

	SHOW_LIST_CALLBACK _showList = _showListCallback;

	if (NULL == _showListCallback) {
		_showList = DefaultShowFilelistCallback;
	}

	const PCK_PATH_NODE* lpNodeToShow = (LPPCK_PATH_NODE)lpFileEntry;

	DWORD dwSerialNumber = 0;

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

const PCK_UNIFIED_FILE_ENTRY* CPckControlCenter::GetUpwardEntry(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry)
{
	if (NULL == lpFileEntry)
		return 0;

	if (PCK_ENTRY_TYPE_FIRSTDOT != (PCK_ENTRY_TYPE_FIRSTDOT & lpFileEntry->entryType)) {
		LPPCK_PATH_NODE lpNode = (LPPCK_PATH_NODE)lpFileEntry;
		return (LPPCK_UNIFIED_FILE_ENTRY)lpNode->parentfirst;
	}
	return lpFileEntry;
}

const PCK_UNIFIED_FILE_ENTRY* CPckControlCenter::GetDownwardEntry(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry)
{
	if (NULL == lpFileEntry)
		return 0;

	if (PCK_ENTRY_TYPE_INDEX != lpFileEntry->entryType) {
		LPPCK_PATH_NODE lpNode = (LPPCK_PATH_NODE)lpFileEntry;
		return (LPPCK_UNIFIED_FILE_ENTRY)lpNode->child;
	}
	return lpFileEntry;
}

#pragma endregion
