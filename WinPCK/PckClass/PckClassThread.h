#pragma once
#include "PckClassFileDisk.h"
#include "PckClassIndex.h"
#include <process.h>
#include <deque>

//获取文件返回值
typedef enum { FD_OK, FD_END, FD_ERR ,FD_CANCEL} FETCHDATA_RET;

class CPckClassThreadWorker;

typedef struct _DataFetchMethod
{
	vector<FILES_TO_COMPRESS>::const_iterator ciFilesList;
	vector<FILES_TO_COMPRESS>::const_iterator ciFilesListEnd;

	CMapViewFileMultiPckRead		*lpFileReadPCK;
	LPPCKINDEXTABLE			lpPckIndexTablePtrSrc;
	DWORD					dwProcessIndex;
	DWORD					dwTotalIndexCount;

	char					szCurrentNodeString[MAX_PATH_PCK_260];						//（界面线程中当前显示的）节点对应的pck中的文件路径
	int						nCurrentNodeStringLen;										//其长度

}DATA_FETCH_METHOD, *LPDATA_FETCH_METHOD;

typedef struct _ThreadParams
{
	void					*pThis;
	FETCHDATA_RET(*GetUncompressedData)(CPckClassThreadWorker *pThis, LPDATA_FETCH_METHOD lpDataFetchMethod, PCKINDEXTABLE &pckFileIndex);
	DATA_FETCH_METHOD		cDataFetchMethod;
}THREAD_PARAMS, *LPTHREAD_PARAMS;

#define MALLOCED_EMPTY_DATA			(1)

_inline char * __fastcall mystrcpy(char * dest, const char *src)
{
	while((*dest = *src))
		dest++, src++;
	return dest;
}

class CPckClassThreadWorker :
	protected virtual CPckClassFileDisk,
	public virtual CPckClassIndex
{
public:
	CPckClassThreadWorker();
	~CPckClassThreadWorker();

private:
	static	VOID WriteThread(VOID* pParam);
	static	VOID CompressThread(VOID* pParam);

protected:

//函数
	BOOL initMultiThreadVars(CMapViewFileMultiPckWrite* lpFileWriter);
protected:
	void	ExecuteMainThreadGroup(PCK_ALL_INFOS &pckAllInfo, int threadnum, void* threadparams);

private:
	//压缩时内存使用
	FETCHDATA_RET	detectMaxToAddMemory(DWORD dwMallocSize);
	FETCHDATA_RET	detectMaxAndAddMemory(LPBYTE &_out_buffer, DWORD dwMallocSize);
	void	freeMaxToSubtractMemory(DWORD dwMallocSize);
	void	freeMaxAndSubtractMemory(LPBYTE &_out_buffer, DWORD dwMallocSize);
	void	SetThreadWorkerCanceled();
	BOOL	isThreadWorkerStillWorking();

	//压缩数据队列
protected:
	BOOL	initCompressedDataQueue(DWORD dwFileCount, QWORD dwAddressStartAt);
private:
	BOOL	putCompressedDataQueue(PCKINDEXTABLE &lpPckFileIndexToCompress);
	BOOL	getCompressedDataQueue(LPBYTE &lpBuffer, PCKINDEXTABLE_COMPRESS &lpPckIndexTable);
protected:
	void	uninitCompressedDataQueue(int threadnum);

private:
	char			m_szEventAllWriteFinish[16];

	static	SRWLOCK				g_mt_LockCompressedflag;
	static	SRWLOCK				g_mt_LockThreadID;
	static	SRWLOCK				g_mt_LockReadFileMap;

	static	int					mt_threadID;		//线程ID
	static	HANDLE				mt_evtAllWriteFinish;											//线程事件

protected:

	static	CMapViewFileMultiPckWrite	*mt_lpFileWrite;												//全局写文件的句柄
	static	QWORD				mt_CompressTotalFileSize;										//预计的压缩文件大小
	static	DWORD				mt_dwFileCountOfWriteTarget;									//写入数据的目标数量,一般=mt_dwFileCount，添加时=重压缩时的有效文件数量


private:
#ifdef _DEBUG
	static	DWORD				mt_dwCurrentQueuePosPut, mt_dwCurrentQueuePosGet;				//当前队列位置
#endif

protected:
	static	QWORD				mt_dwAddressQueue;												//全局压缩过程的写文件的位置，只由Queue控制
	static	QWORD				mt_dwAddressNameQueue;											//读出的pck文件的压缩文件名索引起始位置


//private:

	//在多线程运算中获取压缩好的源数据
	static FETCHDATA_RET		GetUncompressedDataFromFile(CPckClassThreadWorker *pThis, LPDATA_FETCH_METHOD lpDataFetchMethod, PCKINDEXTABLE &pckFileIndex/*, LPBYTE &lpCompressedBuffer, DWORD &dwMallocSize*/);
	static FETCHDATA_RET		GetUncompressedDataFromPCK(CPckClassThreadWorker *pThis, LPDATA_FETCH_METHOD lpDataFetchMethod, PCKINDEXTABLE &pckFileIndex/*, LPBYTE &lpCompressedBuffer, DWORD &dwMallocSize*/);

private:
	SRWLOCK						m_LockQueue, m_LockMaxMemory;

	CONDITION_VARIABLE			m_cvReadyToPut, m_cvReadThreadComplete, m_cvMemoryNotEnough;
	BOOL						m_memoryNotEnoughBlocked;

	deque<PCKINDEXTABLE>		m_QueueContent;
	vector<PCKINDEXTABLE_COMPRESS> m_Index_Compress;
protected:
	vector<FILES_TO_COMPRESS>	m_FilesToBeAdded;
};


