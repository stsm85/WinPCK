#pragma once

class CPckClassNode
{
public:
	CPckClassNode();
	~CPckClassNode();


	LPPCK_PATH_NODE		GetRootNode();
	//将PckIndex文件进行路径分析后放入Node
	LPPCK_PATH_NODE		ParseIndexTableToNode();
	//查找相同的节点
	LPPCK_PATH_NODE		FindFileNode(LPPCK_PATH_NODE lpBaseNode, char* lpszFile);

	//删除一个节点
	VOID	DeleteNode(LPPCK_PATH_NODE lpNode);

	//重命名一个节点
	BOOL	RenameNode(LPPCK_PATH_NODE lpNode, char* lpszReplaceString);
protected:
	BOOL	RenameNodeEnum(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp);
	BOOL	RenameNode(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp);
public:
	VOID	RenameIndex(LPPCK_PATH_NODE lpNode, char* lpszReplaceString);
	VOID	RenameIndex(LPPCKINDEXTABLE lpIndex, char* lpszReplaceString);

	//获取node路径
	BOOL	GetCurrentNodeString(char*szCurrentNodePathString, LPPCK_PATH_NODE lpNode);

	//Indexes
	LPPCKINDEXTABLE		AllocPckIndexTableByFileCount(DWORD dwFileCount);
	//将lpPckIndexTable->cFileIndex.szFilename 中的Ansi字符转换志Unicode并写到lpPckIndexTable->cFileIndex.szwFilename中
	void				GenerateUnicodeStringToIndex();

private:

	LPPCKINDEXTABLE		m_lpPckIndexTable;	//PCK文件的索引
	PCK_PATH_NODE		m_cRootNode;		//PCK文件节点的根节点
	DWORD				m_dwFileCount;		//文件数量

	//将PckIndex文件进行路径分析后放入Node
	BOOL	AddFileToNode(LPPCKINDEXTABLE	lpPckIndexNode);

	void*	AllocMemory(size_t	sizeStuct);

	//释放内存
	VOID	DeAllocMultiNodes(LPPCK_PATH_NODE lpThisNode);
};

