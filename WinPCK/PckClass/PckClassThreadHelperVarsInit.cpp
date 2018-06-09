#include "PckClassThreadHelper.h"

CRITICAL_SECTION	CPckClassThreadHelper::g_cs;
CRITICAL_SECTION	CPckClassThreadHelper::g_mt_threadID;
CRITICAL_SECTION	CPckClassThreadHelper::g_mt_nMallocBlocked;
CRITICAL_SECTION	CPckClassThreadHelper::g_mt_lpMaxMemory;
CRITICAL_SECTION	CPckClassThreadHelper::g_dwCompressedflag;
CRITICAL_SECTION	CPckClassThreadHelper::g_mt_pckCompressedDataPtrArrayPtr;

int					CPckClassThreadHelper::mt_threadID;		//线程ID

HANDLE				CPckClassThreadHelper::mt_evtAllWriteFinish;											//线程事件
HANDLE				CPckClassThreadHelper::mt_evtAllCompressFinish;										//线程事件
HANDLE				CPckClassThreadHelper::mt_evtMaxMemory;												//线程事件
CMapViewFileRead	*CPckClassThreadHelper::mt_lpFileRead;													//全局读文件的句柄,目前仅用于重压缩
CMapViewFileWrite	*CPckClassThreadHelper::mt_lpFileWrite;												//全局写文件的句柄
QWORD				CPckClassThreadHelper::mt_CompressTotalFileSize;										//预计的压缩文件大小
LPDWORD				CPckClassThreadHelper::mt_lpdwCount;													//从界面线程传过来的文件计数指针
DWORD				CPckClassThreadHelper::mt_dwFileCount;													//从界面线程传过来的总文件数量指针
DWORD				CPckClassThreadHelper::mt_dwFileCountOfWriteTarget;									//写入数据的目标数量,一般=mt_dwFileCount，添加时=重压缩时的有效文件数量

#if USE_OLD_QUEUE
LPPCKINDEXTABLE_COMPRESS	CPckClassThreadHelper::mt_lpPckIndexTable;										//压缩后的文件名索引
BYTE			**	CPckClassThreadHelper::mt_pckCompressedDataPtrArray;									//内存申请指针的数组

DWORD				CPckClassThreadHelper::mt_dwMaxQueueLength;											//队列最大长度
#ifdef _DEBUG
DWORD				CPckClassThreadHelper::mt_dwCurrentQueuePosPut;
DWORD				CPckClassThreadHelper::mt_dwCurrentQueuePosGet;				//当前队列位置
#endif
DWORD				CPckClassThreadHelper::mt_dwCurrentQueueLength;										//当前队列可使用数据的个数
BYTE			**	CPckClassThreadHelper::mt_pckCompressedDataPtrArrayGet;
BYTE			**	CPckClassThreadHelper::mt_pckCompressedDataPtrArrayPut;
LPPCKINDEXTABLE_COMPRESS	CPckClassThreadHelper::mt_lpPckIndexTablePut;
LPPCKINDEXTABLE_COMPRESS	CPckClassThreadHelper::mt_lpPckIndexTableGet;
#else
deque<LPPCKINDEXTABLE_COMPRESS>	CPckClassThreadHelper::mt_CompressedIndexs;
CPckClassIndexDataAppend *		CPckClassThreadHelper::mt_lpIndexData = NULL;
#endif
//QWORD				mt_dwAddress;
QWORD				CPckClassThreadHelper::mt_dwAddressQueue;												//全局压缩过程的写文件的位置，只由Queue控制
QWORD				CPckClassThreadHelper::mt_dwAddressNameQueue;											//读出的pck文件的压缩文件名索引起始位置

BOOL				*CPckClassThreadHelper::mt_lpbThreadRunning;											//线程是否处于运行状态的值，当为false时线程退出
DWORD				*CPckClassThreadHelper::mt_lpMaxMemory;												//压缩过程允许使用的最大内存，这是一个计数回显用的，引用从界面线程传来的数值并回显
DWORD				CPckClassThreadHelper::mt_MaxMemory;													//压缩过程允许使用的最大内存
int					CPckClassThreadHelper::mt_nMallocBlocked;												//因缓存用完被暂停的线程数


																							//添加时使用变量

char				CPckClassThreadHelper::mt_szCurrentNodeString[MAX_PATH_PCK_260];						//（界面线程中当前显示的）节点对应的pck中的文件路径
int					CPckClassThreadHelper::mt_nCurrentNodeStringLen;