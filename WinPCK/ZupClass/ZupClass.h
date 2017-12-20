//////////////////////////////////////////////////////////////////////
// ZupClass.h: 用于解析完美世界公司的zup文件中的数据，并显示在List中
// 头文件
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.5.23
//////////////////////////////////////////////////////////////////////

#include "PckClass.h"
#include "ZupHeader.h"
#include "DictHash.h"

#if !defined(_ZUPCLASS_H_)
#define _ZUPCLASS_H_

class CZupClass : public CPckClass
{
public:

	//CZupClass();
	CZupClass(LPPCK_RUNTIME_PARAMS inout);
	virtual ~CZupClass();

	BOOL	Init(LPCTSTR	szFile);

	CONST	LPPCKINDEXTABLE		GetPckIndexTable();
	CONST	LPPCK_PATH_NODE		GetPckPathNode();

	//解压文件
	//virtual BOOL	ExtractFiles(LPPCKINDEXTABLE *lpIndexToExtract, int nFileCount, DWORD &dwCount, BOOL	&bThreadRunning);
	//virtual BOOL	ExtractFiles(LPPCK_PATH_NODE *lpNodeToExtract, int nFileCount, DWORD &dwCount, BOOL	&bThreadRunning);
	
	//设置附加信息
	//char*	GetAdditionalInfo();
	//BOOL	SetAdditionalInfo();

	//新建pck文件
	virtual BOOL	CreatePckFile(LPTSTR szPckFile, LPTSTR szPath/*, int level, int threadnum, int &nMaxMemory, DWORD &dwCount, DWORD &dwAllCount, BOOL	&bThreadRunning*/);

	//重建pck文件
	virtual BOOL	RebuildPckFile(LPTSTR szRebuildPckFile);
	virtual BOOL	RecompressPckFile(LPTSTR szRecompressPckFile);

	//更新pck文件//有bug
	virtual BOOL	UpdatePckFile(LPTSTR szPckFile, TCHAR (*lpszFilePath)[MAX_PATH], int nFileCount, LPPCK_PATH_NODE lpNodeToInsert);

	//重命名文件
	virtual BOOL	RenameFilename(/*int level, DWORD &dwCount, DWORD &dwAllCount*/);

	//删除一个节点
	virtual VOID	DeleteNode(LPPCK_PATH_NODE lpNode);

	//重命名一个节点
	virtual BOOL	RenameNode(LPPCK_PATH_NODE lpNode, char* lpszReplaceString);
protected:
	virtual BOOL	RenameNodeEnum(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp);
	virtual BOOL	RenameNode(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp);
public:
	virtual VOID	RenameIndex(LPPCK_PATH_NODE lpNode, char* lpszReplaceString);
	virtual VOID	RenameIndex(LPPCKINDEXTABLE lpIndex, char* lpszReplaceString);

	//预览文件
	BOOL	GetSingleFileData(LPVOID lpvoidFileRead, LPPCKINDEXTABLE lpZupFileIndexTable, char *buffer, size_t sizeOfBuffer = 0);


protected:


protected:

	PCK_PATH_NODE				m_RootNodeZup;
	LPPCKINDEXTABLE				m_lpZupIndexTable;

	//LPZUP_FILENAME_DICT			m_lpZupDict;
	CDictHash					*m_lpDictHash;


protected:

	virtual void	BuildDirTree();
	BOOL	BuildZupBaseDict();
	void	DecodeFilename(char *_dst, char *_src);

	_inline void	DecodeDict(LPZUP_FILENAME_DICT lpZupDict);
	VOID	AddDict(char *&lpszStringToAdd);
	//void	DeleteDictBuffer(LPZUP_FILENAME_DICT lpDictHead);
	VOID	EnumAndDecodeNode(LPPCK_PATH_NODE lpNodeToDecodeZup, LPPCK_PATH_NODE lpNodeToDecodePck);

	LPPCKINDEXTABLE GetBaseFileIndex(LPPCKINDEXTABLE lpIndex, LPPCKINDEXTABLE lpZeroBaseIndex);

};




#endif