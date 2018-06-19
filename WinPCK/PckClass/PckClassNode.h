#pragma once
#include "PckClassIndex.h"

#define INVALID_NODE	( -1 )

class CPckClassNode :
	protected virtual CPckClassIndex
{
public:
	CPckClassNode();
	~CPckClassNode();

	//将PckIndex文件进行路径分析后放入Node
	void			ParseIndexTableToNode(LPPCKINDEXTABLE lpMainIndexTable);
	//查找相同的节点
	LPPCK_PATH_NODE	FindFileNode(const LPPCK_PATH_NODE lpBaseNode, char* lpszFile);

	//删除一个节点
	virtual VOID	DeleteNode(LPPCK_PATH_NODE lpNode);
	virtual VOID	DeleteNode(LPPCKINDEXTABLE lpIndex);

	//重命名一个节点
	virtual	BOOL	RenameNode(LPPCK_PATH_NODE lpNode, char* lpszReplaceString);
protected:
	BOOL	RenameNodeEnum(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp);
	BOOL	RenameNode(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp);
public:
	virtual	BOOL	RenameIndex(LPPCK_PATH_NODE lpNode, char* lpszReplaceString);
	virtual	BOOL	RenameIndex(LPPCKINDEXTABLE lpIndex, char* lpszReplaceString);

	//获取node路径
	BOOL	GetCurrentNodeString(char*szCurrentNodePathString, LPPCK_PATH_NODE lpNode);

protected:
	BOOL	FindDuplicateNodeFromFileList(const LPPCK_PATH_NODE lpNodeToInsertPtr, DWORD &_in_out_FileCount);

private:

	LPPCK_PATH_NODE		m_lpRootNode;		//PCK文件节点的根节点

	//将PckIndex文件进行路径分析后放入Node
	BOOL	AddFileToNode(LPPCKINDEXTABLE	lpPckIndexNode);

	//释放内存
	VOID	DeAllocMultiNodes(LPPCK_PATH_NODE lpThisNode);
};

