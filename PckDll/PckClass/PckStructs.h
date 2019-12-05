//////////////////////////////////////////////////////////////////////
// PckStructs.h: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 头文件
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2015.5.13
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "PckDefines.h"
#include <vector>

#if !defined(_PCKSTRUCTS_H_)
#define _PCKSTRUCTS_H_


typedef struct _PCK_KEYS
{
	int32_t			id;
	wchar_t			name[64];
	PCK_CATEGORY	CategoryId;
	uint32_t		Version;
	//head
	uint32_t		HeadVerifyKey1;
	uint32_t		HeadVerifyKey2;
	//tail
	uint32_t		TailVerifyKey1;
	uint64_t		IndexesEntryAddressCryptKey;
	uint32_t		TailVerifyKey2;	//在文件尾第-3个DWORD(xx xx xx xx 00 00 00 00 00 00 00 00)
	//index
	uint32_t		IndexCompressedFilenameDataLengthCryptKey1;//pck中的每一个文件都有一个索引存在文件尾（除最后280字节）
	uint32_t		IndexCompressedFilenameDataLengthCryptKey2;//数据为len^key1, len^key2, data(PCKFILEINDEX), (11 11 11 11 22 22 22 22 dd dd dd dd .....)
	//pkx
	uint32_t		dwMaxSinglePckSize;
}PCK_KEYS, *LPPCK_KEYS;

typedef struct _PCK_VERSION_FUNC
{

	//const		PCK_KEYS*	cPckXorKeys;
	PCK_KEYS	cPckXorKeys;
	//头的size
	size_t		dwHeadSize;
	//尾的size
	size_t		dwTailSize;
	//fileindex的size
	size_t		dwFileIndexSize;
	//读取头
	//读取尾
	//读取fileindex
	BOOL(*PickIndexData)(void*, void*);
	//填入头
	void*(*FillHeadData)(void*);
	//填入尾
	void*(*FillTailData)(void*);
	//填入fileindex
	void*(*FillIndexData)(void*, void*);
}PCK_VERSION_FUNC, *LPPCK_VERSION_FUNC;

//****** structures ******* 
#pragma pack(push, 4)

/*
** PCKHEAD PCK文件的文件头结构
** size = 12
**
** dwHeadCheckHead
** 与文件所属的游戏相关，不同游戏的值不同
**
** dwPckSize
** 整个pck文件的大小
**
** dwHeadCheckTail
** 当文件版本＝2.0.2时可用，当版本＝2.0.3时，合并到dwPckSize
** 中作为高位使用
**
*/

typedef struct _PCK_HEAD_V2020
{
	uint32_t		dwHeadCheckHead;
	uint32_t		dwPckSize;
	uint32_t		dwHeadCheckTail;
}PCKHEAD_V2020, *LPPCKHEAD_V2020;

typedef struct _PCK_HEAD_V2030
{
	uint32_t		dwHeadCheckHead;
	union
	{
		uint64_t		dwPckSize;
		struct
		{
			uint32_t		dwPckSizeLow;
			uint32_t		dwHeadCheckTail;
		};
	};
}PCKHEAD_V2030, *LPPCKHEAD_V2030;

/*
** PCKTAIL PCK文件的文件尾（最后8字节）结构
** size = 8
**
** dwFileCount
** 文件中压缩的文件数量
**
** dwVersion
** pck文件的版本
**
*/

typedef struct _PCK_TAIL_V2020
{
	uint32_t		dwIndexTableCheckHead;	//guardByte0
	uint32_t		dwVersion0;				//dwVersion
	uint32_t		dwEntryOffset;
	uint32_t		dwFlags;				//package flags. the highest bit means the encrypt state;, a5 != 0 ? 0x80000000 : 0 诛仙 a5 === 0
	char			szAdditionalInfo[PCK_ADDITIONAL_INFO_SIZE];
	uint32_t		dwIndexTableCheckTail;	//guardByte1
	uint32_t		dwFileCount;
	uint32_t		dwVersion;
}PCKTAIL_V2020, *LPPCKTAIL_V2020;

typedef struct _PCK_TAIL_V2030
{
	uint32_t		dwIndexTableCheckHead;
	uint32_t		dwVersion0;
	uint64_t		dwEntryOffset;
	uint32_t		dwFlags;
	char			szAdditionalInfo[PCK_ADDITIONAL_INFO_SIZE];
	uint64_t		dwIndexTableCheckTail;
	uint32_t		dwFileCount;
	uint32_t		dwVersion;
}PCKTAIL_V2030, *LPPCKTAIL_V2030;

typedef struct _PCK_TAIL_VXAJH
{
	uint32_t		dwIndexTableCheckHead;
	union
	{
		uint64_t		dwEntryOffset;
		struct
		{
			uint32_t		dwVersion0;
			uint32_t		dwCryptedFileIndexesAddrHigh;
		};
	};
	uint32_t		dwFlags;
	char			szAdditionalInfo[PCK_ADDITIONAL_INFO_SIZE];
	uint32_t		dwIndexTableCheckTail;
	uint32_t		dwFileCount;
	uint32_t		dwVersion;
}PCKTAIL_VXAJH, *LPPCKTAIL_VXAJH;

typedef struct _PCK_FILE_INDEX_V2020
{
	char			szFilename[MAX_PATH_PCK_256];
	uint32_t		dwUnknown1;
	uint32_t		dwAddressOffset;
	uint32_t		dwFileClearTextSize;
	uint32_t		dwFileCipherTextSize;
	uint32_t		dwUnknown2;
}PCKFILEINDEX_V2020, *LPPCKFILEINDEX_V2020;

typedef struct _PCK_FILE_INDEX_V2030
{
	char			szFilename[MAX_PATH_PCK_260];
	uint32_t		dwUnknown1;
	uint64_t		dwAddressOffset;
	uint32_t		dwFileClearTextSize;
	uint32_t		dwFileCipherTextSize;
	uint32_t		dwUnknown2;
}PCKFILEINDEX_V2030, *LPPCKFILEINDEX_V2030;


#if PCK_V2031_ENABLE
/*
新诛仙索引大小改成了288，新加了4字节内容
目前影响不大，暂时不添加
*/
typedef struct _PCK_FILE_INDEX_V2031
{
	char			szFilename[MAX_PATH_PCK_260];
	uint32_t		dwUnknown1;
	uint64_t		dwAddressOffset;
	uint32_t		dwFileClearTextSize;
	uint32_t		dwFileCipherTextSize;
	uint32_t		dwUnknown2;
	uint32_t		dwUnknown3;
}PCKFILEINDEX_V2031, *LPPCKFILEINDEX_V2031;

#endif

typedef struct _PCK_FILE_INDEX_VXAJH
{
	char			szFilename[MAX_PATH_PCK_256];
	uint32_t		dwUnknown1;
	uint64_t		dwAddressOffset;
	uint32_t		dwFileClearTextSize;
	uint32_t		dwFileCipherTextSize;
	uint32_t		dwUnknown2;
}PCKFILEINDEX_VXAJH, *LPPCKFILEINDEX_VXAJH;

typedef struct _PCK_FILE_INDEX
{
	wchar_t			szwFilename[MAX_PATH_PCK_260];		//通用Unicode编码
	char			szFilename[MAX_PATH_PCK_260];		//使用pck内部ansi编码，默认CP936
	uint64_t		dwAddressOffset;
	ulong_t			dwFileClearTextSize;
	ulong_t			dwFileCipherTextSize;
}PCKFILEINDEX, *LPPCKFILEINDEX;
#pragma pack(pop)


typedef struct _PCK_INDEX_TABLE
{
	int				entryType;
	PCKFILEINDEX	cFileIndex;
	BOOL			isInvalid;			//添加文件时，如果文件名重复，则被覆盖的文件设置为TRUE（在打开文件建立文件目录树时，同名的文件，后面的会覆盖前面的，被覆盖的此值设为TRUE)
	BOOL			isProtected;		//文件设置保护，不允许删除
	BOOL			isToDeDelete;		//设置为TRUE后，文件会被删除，isProtected为TRUE的除外
	uint32_t		dwMallocSize;						//申请空间使用的大小（>=压缩后的文件大小）
	LPBYTE			compressed_file_data;				//此index对应的压缩数据
	size_t			nFilelenBytes;			//文件名(pck ansi)长度字节数
	size_t			nFilelenLeftBytes;		//文件名(pck ansi)剩余可用字节数，重命名时用，使用MAX_PATH_PCK_256
}PCKINDEXTABLE, *LPPCKINDEXTABLE;


typedef struct _PCK_PATH_NODE
{
	int				entryType;
	wchar_t			szName[MAX_PATH_PCK_260];
	uint32_t		dwFilesCount;
	uint32_t		dwDirsCount;
	size_t			nNameSizeAnsi;		//节点名的pck ansi 长度, 在..目录记录本目录路径（如gfx\下的..目录）的长度（ansi）
	size_t			nMaxNameSizeAnsi;	//
	uint64_t		qdwDirClearTextSize;
	uint64_t		qdwDirCipherTextSize;
	LPPCKINDEXTABLE	lpPckIndexTable;
	_PCK_PATH_NODE	*parentfirst;		//非根目录下的..目录使用，指向..目录在上级目录的节点，也就是被点击的目录所在的..目录
	_PCK_PATH_NODE	*parent;			//非根目录下的..目录使用，指向本目录在上级目录的节点，也就是被点击的目录的上级目录
	_PCK_PATH_NODE	*child;				//普通目录指向下级目录的..目录的节点
	_PCK_PATH_NODE	*next;				//本级目录指向下一项的节点
}PCK_PATH_NODE, *LPPCK_PATH_NODE;


typedef struct _FILES_TO_COMPRESS
{
#if PCK_DEBUG_OUTPUT
	int				id;
#endif
	uint32_t			dwCompressedflag;
	uint32_t			dwFileSize;
	uint32_t			nBytesToCopy;
	char			szFilename[MAX_PATH];
	wchar_t			szwFilename[MAX_PATH];
	size_t			nFileTitleLen;
	_FILES_TO_COMPRESS	*next;
	_PCK_INDEX_TABLE	*samePtr;
}FILES_TO_COMPRESS, *LPFILES_TO_COMPRESS;


typedef struct _PCK_INDEX_TABLE_COMPRESS
{
	union{
		BYTE			compressed_index_data[1];
		struct
		{
			uint32_t			dwIndexValueHead;
			uint32_t			dwIndexValueTail;
			BYTE			buffer[MAX_INDEXTABLE_CLEARTEXT_LENGTH];
		};
	};
	ulong_t				dwIndexDataLength;					//文件索引压缩后的大小
	uint32_t			dwCompressedFilesize;				//压缩后的文件大小
	uint32_t			dwMallocSize;						//申请空间使用的大小（>=压缩后的文件大小）
	uint64_t			dwAddressFileDataToWrite;			//压缩后的数据写入文件的地址
	uint32_t			dwAddressOfDuplicateOldDataArea;	//如果使用老数据区，其地址
}PCKINDEXTABLE_COMPRESS, *LPPCKINDEXTABLE_COMPRESS;


typedef struct _PCK_ALL_INFOS
{
	BOOL				isPckFileLoaded;	//是否已成功加载PCK文件 
	uint64_t				qwPckSize;
	uint32_t				dwFileCount;
	uint64_t				dwAddressOfFileEntry;		//此值指向pck文件数据区的末尾，也就是文件索引的压缩数据的起始位置
	char				szAdditionalInfo[PCK_ADDITIONAL_INFO_SIZE];
	wchar_t				szFilename[MAX_PATH];
	wchar_t				szFileTitle[MAX_PATH];

	//std::vector<PCKINDEXTABLE> lstFileEntry;	//PCK文件的索引
	LPPCKINDEXTABLE		lpPckIndexTable;	//PCK文件的索引
	PCK_PATH_NODE		cRootNode;			//PCK文件节点的根节点

	wchar_t				szNewFilename[MAX_PATH];
	uint32_t				dwFileCountOld;
	uint32_t				dwFileCountToAdd;
	uint32_t				dwFinalFileCount;

	std::vector<FILES_TO_COMPRESS>				*lpFilesToBeAdded;
	const std::vector<PCKINDEXTABLE_COMPRESS>	*lpPckIndexTableToAdd;

	const PCK_VERSION_FUNC*	lpDetectedPckVerFunc;
	const PCK_VERSION_FUNC*	lpSaveAsPckVerFunc;

}PCK_ALL_INFOS, *LPPCK_ALL_INFOS;



class CPckControlCenter;



typedef struct _PCK_VARIETY_PARAMS {

	uint32_t		dwOldFileCount;
	uint32_t		dwPrepareToAddFileCount;
	uint32_t		dwChangedFileCount;
	uint32_t		dwDuplicateFileCount;
	uint32_t		dwFinalFileCount;

	//DWORD		dwUseNewDataAreaInDuplicateFileSize;
	uint32_t		dwDataAreaSize;
	uint32_t		dwRedundancyDataSize;

	LPCSTR		lpszAdditionalInfo;

	uint32_t		dwUIProgress;
	uint32_t		dwUIProgressUpper;

	uint32_t		dwMTMemoryUsed;

	BOOL		bThreadRunning;
	BOOL		bForcedStopWorking;	//强制停止
	int			errMessageNo;			//0 - ok

}PCK_VARIETY_PARAMS;


typedef struct _PCK_RUNTIME_PARAMS {

	PCK_VARIETY_PARAMS	cVarParams;

	uint32_t		dwMTMaxMemory;		//最大使用内存
	uint32_t		dwMTThread;			//压缩线程数
	uint32_t		dwCompressLevel;	//数据压缩率

	//int			code_page;			//pck文件使用编码

	CPckControlCenter	*lpPckControlCenter;

}PCK_RUNTIME_PARAMS, *LPPCK_RUNTIME_PARAMS;



#endif