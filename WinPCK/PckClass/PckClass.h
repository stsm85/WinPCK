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

#include "MapViewFile.h"
#include "PckHeader.h"
#include <stdio.h>
#ifdef _DEBUG
#include <assert.h>
#endif

#if !defined(_PCKCLASS_H_)
#define _PCKCLASS_H_

#ifdef _DEBUG
int logOutput(const char *file, const char *text);
char *formatString(const char *format, ...);
#endif

class CPckClass  
{
//函数
public:
	void	CPckClassInit();
	//CPckClass();
	CPckClass(LPPCK_RUNTIME_PARAMS inout);
	virtual ~CPckClass();

	virtual BOOL	Init(LPCTSTR szFile);

	virtual CONST	LPPCKINDEXTABLE		GetPckIndexTable();
	virtual CONST	LPPCK_PATH_NODE		GetPckPathNode();
	//CONST	LPPCKHEAD			GetPckHead();

	//设置版本
	const	PCK_KEYS*	GetPckVersion();
	void	SetPckVersion(int verID);
	LPCTSTR	GetSaveDlgFilterString();

	//文件大小
	QWORD	GetPckSize();

	//获取文件数
	DWORD	GetPckFileCount();

	//数据区大小
	QWORD	GetPckDataAreaSize();

	//数据区冗余数据大小
	QWORD	GetPckRedundancyDataSize();
	
	//解压文件
	BOOL	ExtractFiles(LPPCKINDEXTABLE *lpIndexToExtract, int nFileCount);
	BOOL	ExtractFiles(LPPCK_PATH_NODE *lpNodeToExtract, int nFileCount);
	
	//设置附加信息
	char*	GetAdditionalInfo();
	BOOL	SetAdditionalInfo();

	//新建pck文件
	virtual BOOL	CreatePckFile(LPTSTR szPckFile, LPTSTR szPath);

	//重建pck文件
	virtual BOOL	RebuildPckFile(LPTSTR szRebuildPckFile);
	virtual BOOL	RecompressPckFile(LPTSTR szRecompressPckFile);

	//更新pck文件
	virtual BOOL	UpdatePckFile(LPTSTR szPckFile, TCHAR (*lpszFilePath)[MAX_PATH], int nFileCount, LPPCK_PATH_NODE lpNodeToInsert);

	//重命名文件
	virtual BOOL	RenameFilename();

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
	virtual BOOL	GetSingleFileData(LPVOID lpvoidFileRead, LPPCKINDEXTABLE lpPckFileIndexTable, char *buffer, size_t sizeOfBuffer = 0);

	//获取node路径
	BOOL	GetCurrentNodeString(char*szCurrentNodePathString, LPPCK_PATH_NODE lpNode);

	//获取基本信息，用于写入文件后造成的文件损坏的恢复
	#include "PckClassExceptionRestore.h"

protected:

	//PckClass.cpp
	BOOL	MountPckFile(LPCTSTR	szFile);

	//PckClassFunction.cpp
	char*	UCS2toA(LPCWSTR src, int max_len = -1);
	BOOL	OpenPckAndMappingRead(CMapViewFileRead *lpRead, LPCSTR lpFileName, LPCSTR lpszMapNamespace);
	BOOL	OpenPckAndMappingRead(CMapViewFileRead *lpRead, LPCWSTR lpFileName, LPCSTR lpszMapNamespace);
	BOOL	OpenPckAndMappingWrite(CMapViewFileWrite *lpWrite, LPCTSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap);
	//重建时重新计算文件数量，除去无效的和文件名重复的
	DWORD	ReCountFiles();
	BOOL	IsNeedExpandWritingFile(
		CMapViewFileWrite *lpWrite,
		QWORD dwAddress,
		QWORD dwFileSize,
		QWORD &dwCompressTotalFileSize
	);
	DWORD GetCompressBoundSizeByFileSize(LPPCKFILEINDEX	lpPckFileIndex, DWORD dwFileSize);

	//函数
	BOOL BeforeSingleOrMultiThreadProcess(LPPCK_ALL_INFOS lpPckAllInfo, CMapViewFileWrite* &lpWrite, LPTSTR szPckFile, DWORD dwCreationDisposition, QWORD qdwSizeToMap, int threadnum);

	void MultiThreadInitialize(VOID CompressThread(VOID*), VOID WriteThread(VOID*), int threadnum);
	BOOL WritePckIndex(CMapViewFileWrite *lpWrite, LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr, QWORD &dwAddress);
	void AfterProcess(CMapViewFileWrite *lpWrite, PCK_ALL_INFOS &PckAllInfo, QWORD &dwAddress, BOOL isRenewAddtional = TRUE);
	BOOL WritePckIndexTable(CMapViewFileWrite *lpWrite, LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr, DWORD &dwFileCount, QWORD &dwAddress);
	void AfterWriteThreadFailProcess(BOOL *lpbThreadRunning, HANDLE hevtAllCompressFinish, DWORD &dwFileCount, DWORD dwFileHasBeenWritten, QWORD &dwAddressFinal, QWORD dwAddress, BYTE **bufferPtrToWrite);
	LPBYTE OpenMappingAndViewAllRead(CMapViewFileRead *lpRead, LPCSTR lpFileName, LPCSTR lpszMapNamespace);
	LPPCKINDEXTABLE_COMPRESS FillAndCompressIndexData(LPPCKINDEXTABLE_COMPRESS lpPckIndexTableComped, LPPCKFILEINDEX lpPckFileIndexToCompress);


	//PckClassFunction.cpp

	BOOL	AllocIndexTableAndInit(LPPCKINDEXTABLE &lpPckIndexTable, DWORD dwFileCount);

	virtual void	BuildDirTree();
	void* AllocNodes(size_t	sizeStuct);
	VOID	DeAllocMultiNodes(LPPCK_PATH_NODE lpThisNode);

	LPFILES_TO_COMPRESS AllocateFileinfo();
	VOID	DeallocateFileinfo();

	BOOL	AddFileToNode(LPPCK_PATH_NODE lpRootNode, LPPCKINDEXTABLE	lpPckIndexNode);
	LPPCK_PATH_NODE	FindFileNode(LPPCK_PATH_NODE lpBaseNode, char* lpszFile);

	//BOOL	EnumNode(LPPCK_PATH_NODE lpNodeToExtract, LPVOID lpvoidFileRead, LPVOID lpvoidFileWrite, LPPCKINDEXTABLE_COMPRESS &lpPckIndexTablePtr, QWORD &dwAddress);
	VOID	EnumFile(LPSTR szFilename, BOOL IsPatition, DWORD &dwFileCount, LPFILES_TO_COMPRESS &pFileinfo, QWORD &qwTotalFileSize, size_t nLen);

	//PckClassExtract.cpp

	BOOL	StartExtract(LPPCK_PATH_NODE lpNodeToExtract, LPVOID lpMapAddress);
	BOOL	DecompressFile(char	*lpszFilename, LPPCKINDEXTABLE lpPckFileIndexTable, LPVOID lpvoidFileRead);


	//PckClassThread.cpp

	/////////以下过程用于多线程pck压缩的测试
	//单线程版
#if PCK_COMPRESS_NEED_ST
	BOOL	CreatePckFileSingleThread(QWORD &dwAddress);
	BOOL	UpdatePckFileSingleThread(QWORD &dwAddress);
	BOOL	RecompressPckFileSingleThread(CMapViewFileRead	*lpFileRead, CMapViewFileWrite *lpFileWrite, DWORD dwFileCount, QWORD &dwAddress, LPPCKINDEXTABLE_COMPRESS	&lpPckIndexTable);
#endif

	static	VOID CompressThreadCreate(VOID* pParam);
	static	VOID WriteThread(VOID* pParam);

	//添加模式

	static	VOID CompressThreadAdd(VOID* pParam);

	//重压缩模式
	static	VOID CompressThreadRecompress(VOID* pParam);

	//压缩数据队列
	void	detectMaxAndAddMemory(DWORD dwMallocSize);
	void	freeMaxAndSubtractMemory(DWORD dwMallocSize);

	BOOL	initCompressedDataQueue(int threadnum, DWORD dwFileCount, QWORD dwAddressStartAt);
	BOOL	putCompressedDataQueue(LPBYTE lpBuffer, LPPCKINDEXTABLE_COMPRESS lpPckIndexTable, LPPCKFILEINDEX lpPckFileIndexToCompress);
	BOOL	getCompressedDataQueue(LPBYTE &lpBuffer, LPPCKINDEXTABLE_COMPRESS &lpPckIndexTable);
	void	uninitCompressedDataQueue(int threadnum);


	///代码测试
	//virtual void test();

	//打印日志
	void PrintLogI(const char *_text);
	void PrintLogW(const char *_text);
	void PrintLogE(const char *_text);
	void PrintLogD(const char *_text);
	void PrintLogN(const char *_text);

	void PrintLogI(const wchar_t *_text);
	void PrintLogW(const wchar_t *_text);
	void PrintLogE(const wchar_t *_text);
	void PrintLogD(const wchar_t *_text);
	void PrintLogN(const wchar_t *_text);

	void PrintLogE(const char *_maintext, const char *_file, const char *_func, const long _line);
	void PrintLogE(const wchar_t *_maintext, const char *_file, const char *_func, const long _line);
	void PrintLogE(const char *_fmt, const char *_maintext, const char *_file, const char *_func, const long _line);
	void PrintLogE(const char *_fmt, const wchar_t *_maintext, const char *_file, const char *_func, const long _line);

	void PrintLog(const char chLevel, const char *_maintext);
	void PrintLog(const char chLevel, const wchar_t *_maintext);
	void PrintLog(const char chLevel, const  char *_fmt, const char *_maintext);
	void PrintLog(const char chLevel, const char *_fmt, const wchar_t *_maintext);

	//
	#include "PckClassVersionDetect.h"

	// 文件头、尾等结构体的读取
	#include "PckClassReader.h"

	//数据压缩算法
	#include "PckClassCompress.h"

//变量
public:

	//TCHAR	m_lastErrorString[1024];

protected:

	LPPCK_RUNTIME_PARAMS	lpPckParams;

	BOOL					m_ReadCompleted;
	PCK_ALL_INFOS			m_PckAllInfo;
	LPPCKINDEXTABLE			m_lpPckIndexTable;
	PCK_PATH_NODE			m_RootNode;

	LPFILES_TO_COMPRESS		m_firstFile;
	
	char			m_szEventAllWriteFinish[16];
	char			m_szEventAllCompressFinish[16];
	char			m_szEventMaxMemory[16];

	char			m_szMapNameRead[16];
	char			m_szMapNameWrite[16];
};

#endif