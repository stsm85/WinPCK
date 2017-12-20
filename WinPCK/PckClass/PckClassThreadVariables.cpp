//////////////////////////////////////////////////////////////////////
// PckClassThreadVariables.cpp: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 
//
// 此程序由 李秋枫/stsm/liqf 编写，pck结构参考若水的pck结构.txt，并
// 参考了其易语言代码中并于读pck文件列表的部分
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2017.7.5
//////////////////////////////////////////////////////////////////////
/////////以下变量用于多线程pck压缩的测试
#define PCK_MODE_COMPRESS_CREATE	1
#define PCK_MODE_COMPRESS_ADD		2

//用户取消或发生错误时，保存当前已完成压缩的文件数
#define SET_PCK_FILE_COUNT_GLOBAL	mt_dwFileCountOfWriteTarget = lpPckParams->cVarParams.dwUIProgress;
#define SET_PCK_FILE_COUNT_AT_FAIL	lpPckParams->cVarParams.bThreadRunning = FALSE;SET_PCK_FILE_COUNT_GLOBAL

#ifdef _DEBUG
char *formatString(const char *format, ...)
{
	const int BUFFER_SIZE = 4097;
	char strbuf[BUFFER_SIZE];
	memset(strbuf, 0, sizeof(strbuf));

	va_list ap;
	va_start(ap, format);
	int result = ::vsnprintf(strbuf, BUFFER_SIZE - 1, format, ap);
	va_end(ap);

	return strbuf;
}

int logOutput(const char *file, const char *text)
{
	char szFile[MAX_PATH];
	if (0 == strncmp("CPckClass::", file, strlen("CPckClass::"))) {
		sprintf_s(szFile, "G:\\pcktest\\%s.log", file + strlen("CPckClass::"));
	}else{
		sprintf_s(szFile, "G:\\pcktest\\%s.log", file);
	}
	

	FILE *pFile = fopen(szFile, "ab");
	if (pFile != NULL)
	{
		fseek(pFile, 0, SEEK_END);
		//string tstr = getLogString() + " - ";
		//int ret = fwrite(tstr.c_str(), 1, tstr.length(), pFile);
		int ret = fwrite(text, 1, strlen(text), pFile);
		fclose(pFile);
		pFile = NULL;
		return ret;
	}
	return -1;
}
#endif

_inline char * __fastcall mystrcpy(char * dest, const char *src)
{
	while ((*dest = *src))
		dest++, src++;
	return dest;
}

CRITICAL_SECTION	g_cs;
CRITICAL_SECTION	g_mt_threadID;
CRITICAL_SECTION	g_mt_nMallocBlocked;
CRITICAL_SECTION	g_mt_lpMaxMemory;
CRITICAL_SECTION	g_dwCompressedflag;
CRITICAL_SECTION	g_mt_pckCompressedDataPtrArrayPtr;

int					mt_threadID;		//线程ID

LPPCKINDEXTABLE_COMPRESS	mt_lpPckIndexTable;										//压缩后的文件名索引
BYTE			**	mt_pckCompressedDataPtrArray;									//内存申请指针的数组
																					//HANDLE			mt_evtToWrite;													//线程事件
HANDLE				mt_evtAllWriteFinish;											//线程事件
HANDLE				mt_evtAllCompressFinish;										//线程事件
HANDLE				mt_evtMaxMemory;												//线程事件
																					//HANDLE				mt_hFileToWrite, mt_hMapFileToWrite;						//全局写文件的句柄
CMapViewFileRead	*mt_lpFileRead;													//全局读文件的句柄,目前仅用于重压缩
CMapViewFileWrite	*mt_lpFileWrite;												//全局写文件的句柄
QWORD				mt_CompressTotalFileSize;										//预计的压缩文件大小
LPDWORD				mt_lpdwCount;													//从界面线程传过来的文件计数指针
DWORD				mt_dwFileCount;													//从界面线程传过来的总文件数量指针
//DWORD				mt_dwNoDupFileCount;											//重压缩时的有效文件数量
DWORD				mt_dwFileCountOfWriteTarget;									//写入数据的目标数量,一般=mt_dwFileCount，添加时=重压缩时的有效文件数量

DWORD				mt_dwMaxQueueLength;											//队列最大长度
#ifdef _DEBUG
DWORD				mt_dwCurrentQueuePosPut, mt_dwCurrentQueuePosGet;				//当前队列位置
#endif
DWORD				mt_dwCurrentQueueLength;										//当前队列可使用数据的个数
BYTE			**	mt_pckCompressedDataPtrArrayGet, **mt_pckCompressedDataPtrArrayPut;
LPPCKINDEXTABLE_COMPRESS	mt_lpPckIndexTablePut, mt_lpPckIndexTableGet;

//QWORD				mt_dwAddress;
QWORD				mt_dwAddressQueue;												//全局压缩过程的写文件的位置，只由Queue控制
QWORD				mt_dwAddressNameQueue;											//读出的pck文件的压缩文件名索引起始位置

																					//size_t			mt_nLen;														//传来的目录的长度，用于截取文件名放到pck中
BOOL				*mt_lpbThreadRunning;											//线程是否处于运行状态的值，当为false时线程退出
DWORD				*mt_lpMaxMemory;												//压缩过程允许使用的最大内存，这是一个计数回显用的，引用从界面线程传来的数值并回显
DWORD				mt_MaxMemory;													//压缩过程允许使用的最大内存
int					mt_nMallocBlocked;												//因缓存用完被暂停的线程数
//DWORD				mt_level;														//压缩等级


																					//添加时使用变量

char				mt_szCurrentNodeString[MAX_PATH_PCK_260];						//（界面线程中当前显示的）节点对应的pck中的文件路径
int					mt_nCurrentNodeStringLen;										//其长度

