#pragma once

#include "MapViewFileMultiPck.h"
#include "PckClassWriteOperator.h"
#include "PckClassLog.h"

#include <functional>
#include <deque>
#include <mutex>


//获取文件方式
typedef enum { DATA_FROM_FILE, DATA_FROM_PCK } PCK_DATA_SOURCE;

//获取文件返回值
typedef enum { FD_OK, FD_END, FD_ERR, FD_CANCEL } FETCHDATA_RET;

typedef std::function<FETCHDATA_RET(PCKINDEXTABLE&)> FETCHDATA_FUNC;

class CPckClassWriteOperator;

typedef struct _DataFetchMethod
{
	vector<FILES_TO_COMPRESS>::const_iterator ciFilesList;
	vector<FILES_TO_COMPRESS>::const_iterator ciFilesListEnd;

	CMapViewFileMultiPckRead		*lpFileReadPCK;
	LPPCKINDEXTABLE					lpPckIndexTablePtrSrc;
	uint32_t						dwProcessIndex;
	uint32_t						dwTotalIndexCount;
	int								iStripFlag;
	//（界面线程中当前显示的）节点对应的pck中的文件路径
	wchar_t							szCurrentNodeString[MAX_PATH_PCK_260];
	//其长度
	int								nCurrentNodeStringLen;

}DATA_FETCH_METHOD, *LPDATA_FETCH_METHOD;

typedef struct _ThreadParams
{
	PCK_DATA_SOURCE				pck_data_src;
	DATA_FETCH_METHOD			cDataFetchMethod;
	CMapViewFileMultiPckWrite *	lpFileWrite;
	CPckClassWriteOperator *	lpPckClassThreadWorker;
	LPPCK_ALL_INFOS				lpPckAllInfo;
	LPPCK_RUNTIME_PARAMS		pckParams;

	uint64_t					dwAddressStartAt;

	//int							threadnum;
	//预计的压缩文件大小
	uint64_t					qwCompressTotalFileSize;
	//写入数据的目标数量,一般=mt_dwFileCount，添加时=重压缩时的有效文件数量
	uint32_t					dwFileCountOfWriteTarget;
}THREAD_PARAMS, *LPTHREAD_PARAMS;

#define MALLOCED_EMPTY_DATA			(1)

template <typename T>
_inline T * __fastcall mystrcpy(T * dest, const T *src)
{
	while ((*dest = *src))
		dest++, src++;
	return dest;
}


typedef std::function<void()> CompressThreadFunc;
typedef std::function<void()> WriteThreadFunc;

class CPckThreadRunner
{
public:
	CPckThreadRunner(LPTHREAD_PARAMS threadparams);
	virtual ~CPckThreadRunner();
	CPckThreadRunner(CPckThreadRunner const&) = delete;
	CPckThreadRunner& operator=(CPckThreadRunner const&) = delete;

	void start();


private:
	LPTHREAD_PARAMS			m_threadparams = nullptr;
	LPPCK_RUNTIME_PARAMS	m_lpPckParams = nullptr;
	CPckClassWriteOperator * m_lpPckClassBase;

	std::mutex				m_LockCompressedflag;

	std::mutex				m_LockReadFileMap;
#if PCK_DEBUG_OUTPUT
	std::mutex				m_LockThreadID;
	int						m_threadID = 0;		//线程ID
#endif

private:
#if PCK_DEBUG_OUTPUT
	uint32_t				m_dwCurrentQueuePosPut = 0, m_dwCurrentQueuePosGet = 0;				//当前队列位置
#endif

protected:
	uint64_t				mt_dwAddressQueue;												//全局压缩过程的写文件的位置，只由Queue控制
	uint64_t				mt_dwAddressNameQueue;											//读出的pck文件的压缩文件名索引起始位置

private:
	std::mutex					m_LockQueue, m_LockMaxMemory;

	std::condition_variable		m_cvReadyToPut, m_cvMemoryNotEnough;
	BOOL						m_memoryNotEnoughBlocked = FALSE;

	deque<PCKINDEXTABLE>		m_QueueContent;
	vector<PCKINDEXTABLE_COMPRESS> m_Index_Compress;



private:

	void startThread();

	void CompressThread(FETCHDATA_FUNC GetUncompressedData);
	void WriteThread(LPTHREAD_PARAMS threadparams);

	//压缩时内存使用
	FETCHDATA_RET	detectMaxToAddMemory(DWORD dwMallocSize);
	FETCHDATA_RET	detectMaxAndAddMemory(LPBYTE &_out_buffer, DWORD dwMallocSize);
	void	freeMaxToSubtractMemory(DWORD dwMallocSize);
	void	freeMaxAndSubtractMemory(LPBYTE &_out_buffer, DWORD dwMallocSize);

	//压缩数据队列

	BOOL	putCompressedDataQueue(PCKINDEXTABLE &lpPckFileIndexToCompress);
	BOOL	getCompressedDataQueue(LPBYTE &lpBuffer, PCKINDEXTABLE_COMPRESS &lpPckIndexTable);

	//在多线程运算中获取压缩好的源数据
	FETCHDATA_RET		GetUncompressedDataFromFile(LPDATA_FETCH_METHOD lpDataFetchMethod, PCKINDEXTABLE &pckFileIndex);
	FETCHDATA_RET		GetUncompressedDataFromPCK(LPDATA_FETCH_METHOD lpDataFetchMethod, PCKINDEXTABLE &pckFileIndex);

};

