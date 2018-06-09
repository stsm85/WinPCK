#pragma once
#include "PckClassBodyWriter.h"
#include "PckClassIndex.h"
#include <process.h>
#include "PckClassIndexDataAppend.h"


#define USE_OLD_QUEUE 1

#if !(USE_OLD_QUEUE)
#include <deque>
using namespace std;
#endif


/////////以下变量用于多线程pck压缩的测试
#define PCK_MODE_COMPRESS_CREATE	1
#define PCK_MODE_COMPRESS_ADD		2

//用户取消或发生错误时，保存当前已完成压缩的文件数
#define SET_PCK_FILE_COUNT_GLOBAL	mt_dwFileCountOfWriteTarget = lpPckParams->cVarParams.dwUIProgress;
#define SET_PCK_FILE_COUNT_AT_FAIL	lpPckParams->cVarParams.bThreadRunning = FALSE;SET_PCK_FILE_COUNT_GLOBAL


_inline char * __fastcall mystrcpy(char * dest, const char *src)
{
	while((*dest = *src))
		dest++, src++;
	return dest;
}

class CPckClassThreadHelper :
	public virtual CPckClassBodyWriter,
	public virtual CPckClassIndex
{
public:
	CPckClassThreadHelper();
	~CPckClassThreadHelper();

	//重压缩模式
	static	VOID CompressThreadRecompress(VOID* pParam);

	//PckClassThread.cpp
	static	VOID CompressThreadCreate(VOID* pParam);
	static	VOID WriteThread(VOID* pParam);

	//添加模式
	static	VOID CompressThreadAdd(VOID* pParam);

protected:


//private:
//函数
	BOOL BeforeMultiThreadProcess(CMapViewFileWrite* lpFileWriter);
	void AfterWriteThreadFailProcess(BOOL *lpbThreadRunning, HANDLE hevtAllCompressFinish, DWORD &dwFileCount, DWORD dwFileHasBeenWritten, QWORD &dwAddressFinal, QWORD dwAddress, BYTE **bufferPtrToWrite);

	void	MultiThreadInitialize(VOID CompressThread(VOID*), VOID WriteThread(VOID*), int threadnum);

	//压缩时内存使用
	void	detectMaxAndAddMemory(DWORD dwMallocSize);
	void	freeMaxAndSubtractMemory(DWORD dwMallocSize);

	//压缩数据队列
	BOOL	initCompressedDataQueue(int threadnum, DWORD dwFileCount, QWORD dwAddressStartAt);
	BOOL	putCompressedDataQueue(LPBYTE lpBuffer, LPPCKINDEXTABLE_COMPRESS lpPckIndexTable, LPPCKFILEINDEX lpPckFileIndexToCompress);
	BOOL	getCompressedDataQueue(LPBYTE &lpBuffer, LPPCKINDEXTABLE_COMPRESS &lpPckIndexTable);
	void	uninitCompressedDataQueue(int threadnum);

	char			m_szEventAllWriteFinish[16];
	char			m_szEventAllCompressFinish[16];
	char			m_szEventMaxMemory[16];

	//static vars
	static	CRITICAL_SECTION	g_cs;
	static	CRITICAL_SECTION	g_mt_threadID;
	static	CRITICAL_SECTION	g_mt_nMallocBlocked;
	static	CRITICAL_SECTION	g_mt_lpMaxMemory;
	static	CRITICAL_SECTION	g_dwCompressedflag;
	static	CRITICAL_SECTION	g_mt_pckCompressedDataPtrArrayPtr;

	static	int					mt_threadID;		//线程ID


	static	HANDLE				mt_evtAllWriteFinish;											//线程事件
	static	HANDLE				mt_evtAllCompressFinish;										//线程事件
	static	HANDLE				mt_evtMaxMemory;												//线程事件
	static	CMapViewFileRead	*mt_lpFileRead;													//全局读文件的句柄,目前仅用于重压缩
	static	CMapViewFileWrite	*mt_lpFileWrite;												//全局写文件的句柄
	static	QWORD				mt_CompressTotalFileSize;										//预计的压缩文件大小
	static	LPDWORD				mt_lpdwCount;													//从界面线程传过来的文件计数指针
	static	DWORD				mt_dwFileCount;													//从界面线程传过来的总文件数量指针
	static	DWORD				mt_dwFileCountOfWriteTarget;									//写入数据的目标数量,一般=mt_dwFileCount，添加时=重压缩时的有效文件数量

#if USE_OLD_QUEUE
	static	BYTE			**	mt_pckCompressedDataPtrArray;									//内存申请指针的数组
	static	LPPCKINDEXTABLE_COMPRESS	mt_lpPckIndexTable;										//压缩后的文件名索引
	static	DWORD				mt_dwMaxQueueLength;											//队列最大长度
#ifdef _DEBUG
	static	DWORD				mt_dwCurrentQueuePosPut, mt_dwCurrentQueuePosGet;				//当前队列位置
#endif
	static	DWORD				mt_dwCurrentQueueLength;										//当前队列可使用数据的个数
	static	BYTE			**	mt_pckCompressedDataPtrArrayGet, **mt_pckCompressedDataPtrArrayPut;
	static	LPPCKINDEXTABLE_COMPRESS	mt_lpPckIndexTablePut, mt_lpPckIndexTableGet;

#else
	static	deque<LPPCKINDEXTABLE_COMPRESS>	mt_CompressedIndexs;
	static	CPckClassIndexDataAppend *		mt_lpIndexData;
#endif
	static	QWORD				mt_dwAddressQueue;												//全局压缩过程的写文件的位置，只由Queue控制
	static	QWORD				mt_dwAddressNameQueue;											//读出的pck文件的压缩文件名索引起始位置

	static	BOOL				*mt_lpbThreadRunning;											//线程是否处于运行状态的值，当为false时线程退出
	static	DWORD				*mt_lpMaxMemory;												//压缩过程允许使用的最大内存，这是一个计数回显用的，引用从界面线程传来的数值并回显
	static	DWORD				mt_MaxMemory;													//压缩过程允许使用的最大内存
	static	int					mt_nMallocBlocked;												//因缓存用完被暂停的线程数


																						//添加时使用变量

	static	char				mt_szCurrentNodeString[MAX_PATH_PCK_260];						//（界面线程中当前显示的）节点对应的pck中的文件路径
	static	int					mt_nCurrentNodeStringLen;										//其长度

};

