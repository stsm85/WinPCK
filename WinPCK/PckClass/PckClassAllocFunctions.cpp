//////////////////////////////////////////////////////////////////////
// PckClassAllocFunctions.cpp: PCK文件功能过程中的内存申请与释放子过程 
//
// 此程序由 李秋枫/stsm/liqf 编写，pck结构参考若水的pck结构.txt，并
// 参考了其易语言代码中并于读pck文件列表的部分
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2015.5.19
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4996 )

#include "PckClass.h"

void* CPckClass::AllocNodes(size_t	sizeStuct)
{
	void*		lpMallocNode;

	if(NULL == (lpMallocNode = /*(LPPCK_PATH_NODE)*/malloc(sizeStuct)))
	{
		PrintLogE(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
		return lpMallocNode;
	}
	//初始化内存
	memset(lpMallocNode, 0, sizeStuct);

	return lpMallocNode;

}

BOOL CPckClass::AllocIndexTableAndInit(LPPCKINDEXTABLE &lpPckIndexTable, DWORD dwFileCount)
{
#ifdef _DEBUG
	char szDebug[64];
	DWORD take = GetTickCount();
#endif
	//申请空间
	if(NULL == (lpPckIndexTable = (LPPCKINDEXTABLE) malloc (sizeof(PCKINDEXTABLE) * dwFileCount)))
	{
		PrintLogE(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	memset(lpPckIndexTable, 0, sizeof(PCKINDEXTABLE) * dwFileCount);

#ifdef _DEBUG
	sprintf_s(szDebug, "%s:%d", __FUNCTION__, GetTickCount() - take);
	PrintLogD(szDebug);
#endif
	return TRUE;

}

VOID CPckClass::DeallocateFileinfo()
{
	LPFILES_TO_COMPRESS Fileinfo;

	while(m_firstFile != NULL){
		Fileinfo = m_firstFile->next;
		free(m_firstFile);
		m_firstFile = Fileinfo;
	}
	//m_firstFile = NULL;
}

LPFILES_TO_COMPRESS CPckClass::AllocateFileinfo()
{
	LPFILES_TO_COMPRESS pFileToCompress;

	pFileToCompress = (LPFILES_TO_COMPRESS) malloc(sizeof(FILES_TO_COMPRESS));

	if(pFileToCompress != NULL){
		memset(pFileToCompress, 0, sizeof(FILES_TO_COMPRESS));
	}

	return pFileToCompress;
}

VOID CPckClass::DeAllocMultiNodes(LPPCK_PATH_NODE lpThisNode)
{

	LPPCK_PATH_NODE		lpThisNodePtr = lpThisNode;
	//BOOL				bFinish = FALSE;

	while(NULL != lpThisNode)
	{

		lpThisNodePtr = lpThisNode;
		if(NULL != lpThisNodePtr->child)
		{
			DeAllocMultiNodes(lpThisNodePtr->child);
		}
		lpThisNode = lpThisNodePtr->next;

		free(lpThisNodePtr);

	}

}

VOID CPckClass::DeleteNode(LPPCK_PATH_NODE lpNode)
{
	lpNode = lpNode->child->next;

	while(NULL != lpNode)
	{
		if(NULL == lpNode->child)
		{
			lpNode->lpPckIndexTable->bSelected = TRUE;
		}else{
			DeleteNode(lpNode);
		}

		lpNode = lpNode->next;
	}

}
