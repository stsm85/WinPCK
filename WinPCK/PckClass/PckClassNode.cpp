
#include <Windows.h>
#include "PckStructs.h"
#include "PckClassNode.h"

#include "CharsCodeConv.h"


template <typename T>
_inline int __fastcall strpathcmp(const T * src, T * &dst)
{
	int ret = 0;

	while((!(ret = (int)(*src - *dst))) && *src)
		++src, ++dst;

	if(0 != ret && 0 == *src && ('\\' == *dst || '/' == *dst))return 0;
	return(ret);
}

template <typename T>
_inline void __fastcall strpathcpy(T * dst, T * &src)
{
	while((*dst++ = *src) && '\\' != *++src && '/' != *src)
		;
}
#pragma region 类函数

CPckClassNode::CPckClassNode() : 
	m_cRootNode({ 0 }),
	m_lpPckIndexTable(NULL)
{}

CPckClassNode::~CPckClassNode()
{
	DeAllocMultiNodes(m_cRootNode.child);

	if(NULL != m_lpPckIndexTable)
		free(m_lpPckIndexTable);
}

VOID CPckClassNode::DeAllocMultiNodes(LPPCK_PATH_NODE lpThisNode)
{

	LPPCK_PATH_NODE		lpThisNodePtr;

	while(NULL != lpThisNode) {

		lpThisNodePtr = lpThisNode;
		if(NULL != lpThisNodePtr->child) {
			DeAllocMultiNodes(lpThisNodePtr->child);
		}
		lpThisNode = lpThisNodePtr->next;

		free(lpThisNodePtr);
	}
	//memset(&m_cRootNode, 0, sizeof(m_cRootNode));
}
LPPCK_PATH_NODE	CPckClassNode::GetRootNode()
{
	return &m_cRootNode;
}
#pragma endregion
#pragma region ParseIndexTableToNode

LPPCK_PATH_NODE	CPckClassNode::ParseIndexTableToNode()
{
	LPPCKINDEXTABLE lpPckIndexTable = m_lpPckIndexTable;

	for(DWORD i = 0;i < m_dwFileCount;++i) {
		//建立目录

		AddFileToNode(lpPckIndexTable);
		++lpPckIndexTable;
	}

	return &m_cRootNode;
}

/********************************
*
*将文件名按目录(\或/）拆分，添加到目录节点中去
*
*
**********************************/

BOOL CPckClassNode::AddFileToNode(LPPCKINDEXTABLE lpPckIndexTable)
{
	LPPCK_PATH_NODE lpChildNode = &m_cRootNode;
	LPPCK_PATH_NODE	lpFirstNode = NULL;

	size_t			sizePckPathNode = sizeof(PCK_PATH_NODE);

	wchar_t			*lpszFilename = lpPckIndexTable->cFileIndex.szwFilename;
	wchar_t			*lpszToFind;

	do {
		//此节点下还没有文件（是一个新产生的节点），首先添加".."
		if(NULL == (lpChildNode->child)) {
			lpChildNode->child = (LPPCK_PATH_NODE)AllocMemory(sizePckPathNode);
			lpChildNode->child->parent = lpChildNode;
			lpChildNode->child->parentfirst = lpFirstNode;

			//添加..目录
			//strcpy(lpChildNode->child->szName, "..");
			//*(DWORD*)lpChildNode->child->szName = 0x2e2e;	//".."
			memcpy(lpChildNode->child->szName, L"..", wcslen(L"..") * sizeof(wchar_t));
		}

		lpFirstNode = lpChildNode = lpChildNode->child;

		do {
			lpszToFind = lpszFilename;

			if((NULL == lpChildNode->lpPckIndexTable) || (!lpChildNode->lpPckIndexTable->isInvalid)) {

				if(0 == strpathcmp(lpChildNode->szName, lpszToFind)) {
					//存在这个文件（夹）了
					lpszFilename = lpszToFind;

					//存在重复的文件名，则前一个重复的文件为无效
					if(0 == *lpszToFind)
						lpChildNode->lpPckIndexTable->isInvalid = TRUE;

					break;
				}

				if(NULL == lpChildNode->next) {

					//添加文件（夹）
					lpChildNode->next = (LPPCK_PATH_NODE)AllocMemory(sizePckPathNode);
					lpChildNode = lpChildNode->next;

					strpathcpy(lpChildNode->szName, lpszFilename);

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
			lpChildNode = lpChildNode->next;

		} while(1);

		++(lpFirstNode->dwFilesCount);
		lpFirstNode->qdwDirCipherTextSize += lpPckIndexTable->cFileIndex.dwFileCipherTextSize;
		lpFirstNode->qdwDirClearTextSize += lpPckIndexTable->cFileIndex.dwFileClearTextSize;

		if(TEXT('\\') == *lpszFilename || TEXT('/') == *lpszFilename)
			++lpszFilename;

	} while(*lpszFilename);

	lpChildNode->lpPckIndexTable = lpPckIndexTable;

	return TRUE;

}
#pragma endregion

#pragma region FindFileNode
LPPCK_PATH_NODE CPckClassNode::FindFileNode(LPPCK_PATH_NODE lpBaseNode, char* lpszFile)
{
	LPPCK_PATH_NODE lpChildNode = lpBaseNode;

	CAnsi2Ucs		cA2U;
	wchar_t			szFilename[MAX_PATH];
	cA2U.GetString(lpszFile, szFilename, MAX_PATH);

	wchar_t			*lpszFilename = szFilename;
	wchar_t			*lpszToFind;


	if(NULL == lpChildNode->szName)
		return NULL;

	do {
		do {
			lpszToFind = lpszFilename;

			if(0 == strpathcmp(lpChildNode->szName, lpszToFind)) {
				lpszFilename = lpszToFind;

				if(NULL == lpChildNode->child && 0 == *lpszFilename)return lpChildNode;

				if((NULL == lpChildNode->child && (TEXT('\\') == *lpszFilename || TEXT('/') == *lpszFilename)) || (NULL != lpChildNode->child && 0 == *lpszFilename)) {
					return (LPPCK_PATH_NODE)-1;
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

VOID CPckClassNode::DeleteNode(LPPCK_PATH_NODE lpNode)
{
	lpNode = lpNode->child->next;

	while(NULL != lpNode) {
		if(NULL == lpNode->child) {
			lpNode->lpPckIndexTable->bSelected = TRUE;
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
	if(lenrs >= (MAX_PATH_PCK_260 - strlen(lpNode->lpPckIndexTable->cFileIndex.szFilename + lenNodeRes - 2)))return FALSE;
	char	szTemp[MAX_PATH_PCK_260] = { 0 };
	char	*lpszReplacePos = lpNode->lpPckIndexTable->cFileIndex.szFilename + lenrp - lenNodeRes;

	//DebugA("lpszReplaceString = %s \r\nlenNodeRes = %d\r\nlenrs = %d\r\nlenrp = %d\r\n===============================\r\n",
	//		lpszReplaceString, lenNodeRes, lenrs, lenrp);
	memcpy(szTemp, lpNode->lpPckIndexTable->cFileIndex.szFilename + lenrp, MAX_PATH_PCK_260 - lenrp);
	memcpy(lpszReplacePos, lpszReplaceString, lenrs);
	memcpy(lpszReplacePos + lenrs, szTemp, MAX_PATH_PCK_260 - lenrp - lenrs + lenNodeRes);

	return TRUE;
}


BOOL CPckClassNode::RenameNode(LPPCK_PATH_NODE lpNode, char* lpszReplaceString)
{
	//假设文件名为a\b\c\d.exe
	//本节点为c
	size_t lenNodeRes, lenrs, lenrp;
	char	lpReplacePos[MAX_PATH_PCK_260];

	CUcs2Ansi		cU2A;
	lenNodeRes = cU2A.GetStrlen(lpNode->szName);

	lenrs = strlen(lpszReplaceString);

	//取到"a\b\"
	GetCurrentNodeString(lpReplacePos, lpNode->child);

	//lpReplacePos = "models\z1\"
	lenrp = strlen(lpReplacePos) - 1;

	return RenameNodeEnum(lpNode, lenNodeRes, lpszReplaceString, lenrs, lenrp);

}


VOID CPckClassNode::RenameIndex(LPPCK_PATH_NODE lpNode, char* lpszReplaceString)
{
	size_t nBytesReadayToWrite;
	char	*lpszPosToWrite;

	CUcs2Ansi		cU2A;
	nBytesReadayToWrite = strlen(lpNode->lpPckIndexTable->cFileIndex.szFilename) - cU2A.GetStrlen(lpNode->szName);

	lpszPosToWrite = lpNode->lpPckIndexTable->cFileIndex.szFilename + nBytesReadayToWrite;
	//nBytesReadayToWrite = MAX_PATH_PCK - nBytesReadayToWrite;

	memset(lpszPosToWrite, 0, MAX_PATH_PCK_260 - nBytesReadayToWrite);
	strcpy(lpszPosToWrite, lpszReplaceString);

}

VOID CPckClassNode::RenameIndex(LPPCKINDEXTABLE lpIndex, char* lpszReplaceString)
{
	memset(lpIndex->cFileIndex.szFilename, 0, MAX_PATH_PCK_260);
	strcpy(lpIndex->cFileIndex.szFilename, lpszReplaceString);

}

BOOL CPckClassNode::GetCurrentNodeString(char *szCurrentNodePathString, LPPCK_PATH_NODE lpNode)
{

	if((NULL == lpNode) || (NULL == lpNode->parentfirst)) {
		*szCurrentNodePathString = 0;
		//return TRUE;
	} else {

		GetCurrentNodeString(szCurrentNodePathString, lpNode->parentfirst);

		CUcs2Ansi		cU2A;
		strcat_s(szCurrentNodePathString, MAX_PATH_PCK_260, cU2A.GetString(lpNode->parent->szName));

		strcat_s(szCurrentNodePathString, MAX_PATH_PCK_260, "\\");
	}

	return TRUE;
}

#pragma endregion

#pragma region MyRegion

void* CPckClassNode::AllocMemory(size_t	sizeStuct)
{
	void*		lpMallocNode;

	if(NULL == (lpMallocNode = malloc(sizeStuct))) {
		return lpMallocNode;
	}
	//初始化内存
	memset(lpMallocNode, 0, sizeStuct);
	return lpMallocNode;
}

LPPCKINDEXTABLE	CPckClassNode::AllocPckIndexTableByFileCount(DWORD dwFileCount)
{
	m_dwFileCount = dwFileCount;
	return m_lpPckIndexTable = (LPPCKINDEXTABLE)AllocMemory(sizeof(PCKINDEXTABLE) * dwFileCount);
}

void CPckClassNode::GenerateUnicodeStringToIndex()
{
	LPPCKINDEXTABLE lpPckIndexTable = m_lpPckIndexTable;

	for(DWORD i = 0;i < m_dwFileCount;++i) {
		//建立目录

		CAnsi2Ucs cA2U;
		cA2U.GetString(lpPckIndexTable->cFileIndex.szFilename, lpPckIndexTable->cFileIndex.szwFilename, sizeof(lpPckIndexTable->cFileIndex.szwFilename) / sizeof(wchar_t));

		++lpPckIndexTable;
	}
}
#pragma endregion
