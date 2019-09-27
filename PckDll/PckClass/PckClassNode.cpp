#include "PckClassNode.h"

template <typename T>
_inline void strpathcpy(T * dst, T * &src)
{
	while((*dst++ = *src) && '\\' != *++src && '/' != *src)
		;
}
#pragma region 类函数

CPckClassNode::CPckClassNode()
{
	m_lpRootNode = &m_PckAllInfo.cRootNode;
}

CPckClassNode::~CPckClassNode()
{
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

#pragma endregion
#pragma region ParseIndexTableToNode

void CPckClassNode::ParseIndexTableToNode(LPPCKINDEXTABLE lpMainIndexTable)
{
	LPPCKINDEXTABLE lpPckIndexTable = lpMainIndexTable;

	for(DWORD i = 0;i < m_PckAllInfo.dwFileCount;++i) {
		//添加类型标识
		lpPckIndexTable->entryType = PCK_ENTRY_TYPE_INDEX;
		//建立目录
		AddFileToNode(lpPckIndexTable);
		++lpPckIndexTable;
	}
	
	//将第一个root节点下的entryType加上标签以示区别
	if(NULL != m_PckAllInfo.cRootNode.child)
		m_PckAllInfo.cRootNode.child->entryType |= PCK_ENTRY_TYPE_ROOT;
}

/********************************
*
*将文件名按目录(\或/）拆分，添加到目录节点中去
*
*
**********************************/

BOOL CPckClassNode::AddFileToNode(LPPCKINDEXTABLE lpPckIndexTable)
{
	LPPCK_PATH_NODE lpChildNode = &m_PckAllInfo.cRootNode;
	LPPCK_PATH_NODE	lpFirstNode = NULL;

	wchar_t			*lpszFilename = lpPckIndexTable->cFileIndex.szwFilename;

	do {
		//此节点下还没有文件（是一个新产生的节点），首先添加".."
		if(NULL == (lpChildNode->child)) {

			lpChildNode->child = (LPPCK_PATH_NODE)m_NodeMemPool.Alloc(sizeof(PCK_PATH_NODE));

			lpChildNode->child->parent = lpChildNode;
			lpChildNode->child->parentfirst = lpFirstNode;

			//使用nNameSizeAnsi 在..目录记录本目录路径（如gfx\下的..目录）的长度（ansi）
			if(NULL != lpFirstNode)
				lpChildNode->child->nNameSizeAnsi = (lpFirstNode->nNameSizeAnsi + lpChildNode->nNameSizeAnsi + 1);

			//添加..目录
			memcpy(lpChildNode->child->szName, L"..", wcslen(L"..") * sizeof(wchar_t));

			lpChildNode->child->entryType = PCK_ENTRY_TYPE_NODE | PCK_ENTRY_TYPE_FOLDER | PCK_ENTRY_TYPE_DOTDOT;
		}

		lpFirstNode = lpChildNode = lpChildNode->child;

		//解析目录层（分组\\）

		wchar_t	szToFind[MAX_PATH_PCK_260] = { 0 };
		strpathcpy(szToFind, lpszFilename);

		do {
			//下一级目录(next '\\')
			//此级目录是新节点或此节点是有效目录节点
			if((NULL == lpChildNode->lpPckIndexTable) || (!lpChildNode->lpPckIndexTable->isInvalid)) {

				if (0 == wcscmp(lpChildNode->szName, szToFind)) {
					//存在这个文件（夹）了

					//存在重复的文件名，则前一个重复的文件为无效
					if(0 == *lpszFilename)
						lpChildNode->lpPckIndexTable->isInvalid = TRUE;

					break;
				}

				//此级节点已遍历完，没有重复的，添加新节点
				if(NULL == lpChildNode->next) {

					//添加文件（夹）
					lpChildNode->next = (LPPCK_PATH_NODE)m_NodeMemPool.Alloc(sizeof(PCK_PATH_NODE));
					lpChildNode = lpChildNode->next;

					lpChildNode->parent = lpFirstNode->parent;

					lpChildNode->entryType = PCK_ENTRY_TYPE_NODE | PCK_ENTRY_TYPE_FOLDER;

					wcscpy(lpChildNode->szName, szToFind);
					lpChildNode->nNameSizeAnsi = CPckClassCodepage::PckFilenameCode2Ansi(lpChildNode->szName, NULL, 0);
					lpChildNode->nMaxNameSizeAnsi = MAX_PATH_PCK_256;

					//统计各文件夹的子文件夹数
					if(0 != *lpszFilename) {
						LPPCK_PATH_NODE	lpAddDirCount = lpFirstNode;
						do {
							++(lpAddDirCount->dwDirsCount);
							lpAddDirCount = lpAddDirCount->parentfirst;

						} while(NULL != lpAddDirCount);

					}

					break;
				}
			}
			else {
				throw MyException("理论上不存在无效目录节点!");
				//出错了
			}

			//同级目录的下一个节点
			lpChildNode = lpChildNode->next;

		} while(1);

		//名文件夹数据统计
		++(lpFirstNode->dwFilesCount);
		lpFirstNode->qdwDirCipherTextSize += lpPckIndexTable->cFileIndex.dwFileCipherTextSize;
		lpFirstNode->qdwDirClearTextSize += lpPckIndexTable->cFileIndex.dwFileClearTextSize;

		if (lpChildNode->nMaxNameSizeAnsi > lpPckIndexTable->nFilelenLeftBytes) {
			lpChildNode->nMaxNameSizeAnsi = lpPckIndexTable->nFilelenLeftBytes;
		}

		if(TEXT('\\') == *lpszFilename || TEXT('/') == *lpszFilename)
			++lpszFilename;

	} while(*lpszFilename);

	lpChildNode->lpPckIndexTable = lpPckIndexTable;
	lpChildNode->entryType = PCK_ENTRY_TYPE_NODE;

	return TRUE;

}
#pragma endregion

#pragma region FindFileNode
const PCK_PATH_NODE* CPckClassNode::FindFileNode(const PCK_PATH_NODE* lpBaseNode, wchar_t* lpszFile)
{
	if ((NULL == lpBaseNode) || (NULL == lpBaseNode->child))
		return NULL;

	const PCK_PATH_NODE* lpChildNode = lpBaseNode->child;

	wchar_t			szFilename[MAX_PATH];
	wcscpy_s(szFilename, lpszFile);

	wchar_t			*lpszFilename = szFilename;

	if(NULL == lpChildNode->szName)
		return NULL;

	do {
		wchar_t	szToFind[MAX_PATH_PCK_260] = { 0 };
		strpathcpy(szToFind, lpszFilename);

		do {

			if(0 == wcscmp(lpChildNode->szName, szToFind)) {

				if(NULL == lpChildNode->child && 0 == *lpszFilename)return lpChildNode;

				if((NULL == lpChildNode->child && (TEXT('\\') == *lpszFilename || TEXT('/') == *lpszFilename)) || (NULL != lpChildNode->child && 0 == *lpszFilename)) {
					return (LPPCK_PATH_NODE)INVALID_NODE;
				}

				break;
			}

			if(NULL == lpChildNode->next) {
				return NULL;
			}
			lpChildNode = lpChildNode->next;

		} while(1);

		lpChildNode = lpChildNode->child;

		if(TEXT('\\') == *lpszFilename || TEXT('/') == *lpszFilename)
			++lpszFilename;

	} while(*lpszFilename);

	return NULL;

}

#pragma endregion
#pragma region DeleteNode

VOID CPckClassNode::DeleteNode(LPPCKINDEXTABLE lpIndex)
{
	lpIndex->isInvalid = TRUE;
}

VOID CPckClassNode::DeleteNode(LPPCK_PATH_NODE lpNode)
{
	lpNode = lpNode->child->next;

	while(NULL != lpNode) {
		if(NULL == lpNode->child) {
			DeleteNode(lpNode->lpPckIndexTable);
		} else {
			DeleteNode(lpNode);
		}

		lpNode = lpNode->next;
	}
}

#pragma endregion

#pragma region RenameNode

BOOL CPckClassNode::RenameNodeEnum(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp)
{

	lpNode = lpNode->child->next;

	while(NULL != lpNode) {

		if(NULL == lpNode->child) {

			if(!RenameNode(lpNode, lenNodeRes, lpszReplaceString, lenrs, lenrp))
				return FALSE;
		} else {
			if(!RenameNodeEnum(lpNode, lenNodeRes, lpszReplaceString, lenrs, lenrp))
				return FALSE;
		}

		lpNode = lpNode->next;
	}
	return TRUE;
}

BOOL CPckClassNode::RenameNode(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp)
{
	//if(lenrs >= (MAX_PATH_PCK_260 - strlen(lpNode->lpPckIndexTable->cFileIndex.szFilename + lenNodeRes - 2)))return FALSE;
	char	szTemp[MAX_PATH_PCK_260] = { 0 };
	char	*lpszReplacePos = lpNode->lpPckIndexTable->cFileIndex.szFilename + lenrp - lenNodeRes;

	//DebugA("lpszReplaceString = %s \r\nlenNodeRes = %d\r\nlenrs = %d\r\nlenrp = %d\r\n===============================\r\n",
	//		lpszReplaceString, lenNodeRes, lenrs, lenrp);
	memcpy(szTemp, lpNode->lpPckIndexTable->cFileIndex.szFilename + lenrp, MAX_PATH_PCK_260 - lenrp);
	memcpy(lpszReplacePos, lpszReplaceString, lenrs);
	memcpy(lpszReplacePos + lenrs, szTemp, MAX_PATH_PCK_260 - lenrp - lenrs + lenNodeRes);

	return TRUE;
}

//public
BOOL CPckClassNode::RenameNode(LPPCK_PATH_NODE lpNode, const wchar_t* lpszReplaceString)
{
	//假设文件名为a\b\c\d.exe
	//本节点为c
	size_t	lenNodeRes, lenrs, lenrp;
	char	lpszReplaceStringAnsi[MAX_PATH_PCK_260];

	lenrs = CPckClassCodepage::PckFilenameCode2Ansi(lpszReplaceString, lpszReplaceStringAnsi, sizeof(lpszReplaceStringAnsi));

	lenNodeRes = lpNode->nNameSizeAnsi;

	size_t nAllowedMaxSize = lpNode->nMaxNameSizeAnsi + lenNodeRes;

	if (nAllowedMaxSize < lenrs)
		return FALSE;

	//取到"a\b\"
	lenrp = lpNode->child->nNameSizeAnsi - 1;

	return RenameNodeEnum(lpNode, lenNodeRes, lpszReplaceStringAnsi, lenrs, lenrp);

}


BOOL CPckClassNode::RenameIndex(LPPCK_PATH_NODE lpNode, const wchar_t* lpszReplaceString)
{
	//针对目录浏览模式下的文件进行修改
	int		nBytesReadayToWrite;
	char	*lpszPosToWrite;	//文件名地址，又名lpszFileTitle

	lpszPosToWrite = lpNode->lpPckIndexTable->cFileIndex.szFilename + lpNode->lpPckIndexTable->nFilelenBytes - lpNode->nNameSizeAnsi;
	nBytesReadayToWrite = lpNode->nMaxNameSizeAnsi + lpNode->nNameSizeAnsi;

	//转换为ansi，检查长度
	char szReplaceStringAnsi[MAX_PATH_PCK_260];

	size_t nLenOfReplaceString = CPckClassCodepage::PckFilenameCode2Ansi(lpszReplaceString, szReplaceStringAnsi, MAX_PATH_PCK_260);

	if (nBytesReadayToWrite < nLenOfReplaceString)
		return FALSE;

	memset(lpszPosToWrite, 0, nBytesReadayToWrite);
	strcpy(lpszPosToWrite, szReplaceStringAnsi);
	return TRUE;
}

BOOL CPckClassNode::RenameIndex(LPPCKINDEXTABLE lpIndex, const wchar_t* lpszReplaceString)
{
	//转换为ansi，检查长度
	char szReplaceStringAnsi[MAX_PATH_PCK_260];

	size_t nLenOfReplaceString = CPckClassCodepage::PckFilenameCode2Ansi(lpszReplaceString, szReplaceStringAnsi, MAX_PATH_PCK_260);

	if (MAX_PATH_PCK_256 < nLenOfReplaceString)
		return FALSE;

	memset(lpIndex->cFileIndex.szFilename, 0, MAX_PATH_PCK_260);
	strcpy(lpIndex->cFileIndex.szFilename, szReplaceStringAnsi);
	return TRUE;
}

BOOL CPckClassNode::GetCurrentNodeString(wchar_t *szCurrentNodePathString, const PCK_PATH_NODE* lpNode)
{
	if ((NULL == lpNode) || (PCK_ENTRY_TYPE_ROOT == (PCK_ENTRY_TYPE_ROOT & lpNode->entryType))) {
		*szCurrentNodePathString = 0;
		return TRUE;
	}

	int entry_type = lpNode->entryType;

	if (PCK_ENTRY_TYPE_DOTDOT == (PCK_ENTRY_TYPE_DOTDOT & entry_type)) {

		if ((NULL == lpNode->parentfirst) || (NULL == lpNode->parentfirst->parent)) {
			*szCurrentNodePathString = 0;
			return TRUE;
		}

		lpNode = lpNode->parentfirst->parent;

	}
	entry_type = lpNode->entryType;

	if (PCK_ENTRY_TYPE_ROOT == (PCK_ENTRY_TYPE_ROOT & lpNode->entryType)) {
		*szCurrentNodePathString = 0;
		return TRUE;
	}

	GetCurrentNodeString(szCurrentNodePathString, lpNode->parent);
	wcscat_s(szCurrentNodePathString, MAX_PATH_PCK_260, lpNode->szName);

	if (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & entry_type))
		wcscat_s(szCurrentNodePathString, MAX_PATH_PCK_260, L"\\");

	return TRUE;
}

#pragma endregion


BOOL CPckClassNode::FindDuplicateNodeFromFileList(const PCK_PATH_NODE* lpNodeToInsertPtr, DWORD &_in_out_DuplicateFileCount)
{
	vector<FILES_TO_COMPRESS> *lpFilesList = m_PckAllInfo.lpFilesToBeAdded;
	size_t sizeOfFileList = lpFilesList->size();

	for(size_t i=0;i<sizeOfFileList;i++){

		FILES_TO_COMPRESS *lpfirstFile = &(*lpFilesList)[i];
		const PCK_PATH_NODE* lpDuplicateNode = FindFileNode(lpNodeToInsertPtr, lpfirstFile->szwFilename + lpfirstFile->nFileTitleLen);

		if(INVALID_NODE == (int)lpDuplicateNode) {
			Logger.w(TEXT_ERROR_DUP_FOLDER_FILE);
			assert(FALSE);
			return FALSE;
		}

		if(NULL != lpDuplicateNode) {
			lpfirstFile->samePtr = lpDuplicateNode->lpPckIndexTable;
			lpDuplicateNode->lpPckIndexTable->isInvalid = TRUE;
			++_in_out_DuplicateFileCount;
		}

	}
	return TRUE;
}