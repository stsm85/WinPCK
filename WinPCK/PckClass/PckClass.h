//////////////////////////////////////////////////////////////////////
// PckClass.h: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 头文件
//
// 此程序由 李秋枫/stsm/liqf 编写，pck结构参考若水的pck结构.txt，并
// 参考了其易语言代码中并于读pck文件列表的部分
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#include <assert.h>
#include "CharsCodeConv.h"

#include "PckClassVersionDetect.h"
//#include "PckClassNode.h"
//#include "PckClassHeadTail.h"

#include "PckClassAppendFiles.h"
#include "PckClassCreateNew.h"
#include "PckClassRebuild.h"
#include "PckClassRenamer.h"

#if !defined(_PCKCLASS_H_)
#define _PCKCLASS_H_

#ifdef _DEBUG
int logOutput(const char *file, const char *text);
char *formatString(const char *format, ...);
#endif

class CPckClass : 
	public CPckClassRenamer,
	public CPckClassRebuild,
	public CPckClassAppendFiles,
	public CPckClassVersionDetect
{
//函数
public:
	CPckClass(LPPCK_RUNTIME_PARAMS inout);
	virtual ~CPckClass();

	virtual BOOL	Init(LPCTSTR szFile);
#if 0
	virtual CONST	LPPCKINDEXTABLE		GetPckIndexTable();
	virtual CONST	LPPCK_PATH_NODE		GetPckPathNode();

	//文件大小
	QWORD	GetPckSize();

	//获取文件数
	DWORD	GetPckFileCount();

	//数据区大小
	QWORD	GetPckDataAreaSize();

	//数据区冗余数据大小
	QWORD	GetPckRedundancyDataSize();
#endif
	//解压文件
	BOOL	ExtractFiles(LPPCKINDEXTABLE *lpIndexToExtract, int nFileCount);
	BOOL	ExtractFiles(LPPCK_PATH_NODE *lpNodeToExtract, int nFileCount);
#if 0
	//新建pck文件
	virtual BOOL	CreatePckFile(LPTSTR szPckFile, LPTSTR szPath);
#endif
#if 0
	//重建pck文件
	virtual BOOL	RebuildPckFile(LPCTSTR szRebuildPckFile, BOOL bUseRecompress);
protected:
	BOOL	RebuildPckFile(LPCTSTR szRebuildPckFile);
	BOOL	RecompressPckFile(LPCTSTR szRecompressPckFile);
#endif
public:
#if 0
	//更新pck文件
	virtual BOOL	UpdatePckFile(LPTSTR szPckFile, TCHAR (*lpszFilePath)[MAX_PATH], int nFileCount, LPPCK_PATH_NODE lpNodeToInsert);
#endif
#if 0
	//重命名文件
	virtual BOOL	RenameFilename();
#endif
	//预览文件
	virtual BOOL	GetSingleFileData(LPVOID lpvoidFileRead, LPPCKINDEXTABLE lpPckFileIndexTable, char *buffer, size_t sizeOfBuffer = 0);

	//获取基本信息，用于写入文件后造成的文件损坏的恢复
	#include "PckClassExceptionRestore.h"

protected:

	//PckClass.cpp
	BOOL	MountPckFile(LPCTSTR	szFile);
	virtual void	BuildDirTree();

	//PckClassFunction.cpp

#if 0
	//重建时重新计算文件数量，除去无效的和文件名重复的
	DWORD	ReCountFiles();

#endif
#if 0
	DWORD GetCompressBoundSizeByFileSize(LPPCKFILEINDEX	lpPckFileIndex, DWORD dwFileSize);
#endif
#if 0
	//函数
	BOOL BeforeSingleOrMultiThreadProcess(LPPCK_ALL_INFOS lpPckAllInfo, CMapViewFileWrite* &lpWrite, LPCTSTR szPckFile, DWORD dwCreationDisposition, QWORD qdwSizeToMap, int threadnum);
#endif
#if 0
	void MultiThreadInitialize(VOID CompressThread(VOID*), VOID WriteThread(VOID*), int threadnum);
#endif
#if 0
	BOOL WritePckIndex(CMapViewFileWrite *lpWrite, LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr, QWORD &dwAddress);
	BOOL WritePckIndexTable(CMapViewFileWrite *lpWrite, LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr, DWORD &dwFileCount, QWORD &dwAddress);
#endif
#if 0
	void AfterProcess(CMapViewFileWrite *lpWrite, PCK_ALL_INFOS &PckAllInfo, QWORD &dwAddress, BOOL isRenewAddtional = TRUE);

	void AfterWriteThreadFailProcess(BOOL *lpbThreadRunning, HANDLE hevtAllCompressFinish, DWORD &dwFileCount, DWORD dwFileHasBeenWritten, QWORD &dwAddressFinal, QWORD dwAddress, BYTE **bufferPtrToWrite);
#endif
#if 0
	LPPCKINDEXTABLE_COMPRESS FillAndCompressIndexData(LPPCKINDEXTABLE_COMPRESS lpPckIndexTableComped, LPPCKFILEINDEX lpPckFileIndexToCompress);
#endif

	//PckClassExtract.cpp
	BOOL	StartExtract(LPPCK_PATH_NODE lpNodeToExtract, LPVOID lpMapAddress);
	BOOL	DecompressFile(LPCWSTR lpszFilename, LPPCKINDEXTABLE lpPckFileIndexTable, LPVOID lpvoidFileRead);

#if 0
	//PckClassThread.cpp
	static	VOID CompressThreadCreate(VOID* pParam);
	static	VOID WriteThread(VOID* pParam);

	//添加模式
	static	VOID CompressThreadAdd(VOID* pParam);

#endif
#if 0
	//重压缩模式
	static	VOID CompressThreadRecompress(VOID* pParam);
#endif
#if 0
	//压缩时内存使用
	void	detectMaxAndAddMemory(DWORD dwMallocSize);
	void	freeMaxAndSubtractMemory(DWORD dwMallocSize);
#endif
#if 0
	//压缩数据队列
	BOOL	initCompressedDataQueue(int threadnum, DWORD dwFileCount, QWORD dwAddressStartAt);
	BOOL	putCompressedDataQueue(LPBYTE lpBuffer, LPPCKINDEXTABLE_COMPRESS lpPckIndexTable, LPPCKFILEINDEX lpPckFileIndexToCompress);
	BOOL	getCompressedDataQueue(LPBYTE &lpBuffer, LPPCKINDEXTABLE_COMPRESS &lpPckIndexTable);
	void	uninitCompressedDataQueue(int threadnum);
#endif

	///代码测试
	//virtual void test();
#if 0
public:
	void	init_compressor();
#endif
	//PckClassRebuildFilter.cpp
public:
	BOOL	ParseScript(LPCTSTR lpszScriptFile);

protected:

	
	
#if 0
	char			m_szEventAllWriteFinish[16];
	char			m_szEventAllCompressFinish[16];
	char			m_szEventMaxMemory[16];
#endif
};

#endif