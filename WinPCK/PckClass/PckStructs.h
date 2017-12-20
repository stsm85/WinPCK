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

#include "PckConf.h"

#if !defined(_PCKSTRUCTS_H_)
#define _PCKSTRUCTS_H_


typedef enum _FMTPCK
{
	FMTPCK_PCK					= 0,
	FMTPCK_ZUP					= 1,
	FMTPCK_CUP					= 2,
	FMTPCK_UNKNOWN				= 0x7fffffff
}FMTPCK;

typedef enum _PCKVER
{
	PCK_V2020,
	PCK_V2030,
	PCK_V2031
}PCKVERSION;

typedef struct _PCK_KEYS
{
	int			id;
	TCHAR		name[64];
	PCKVERSION	VersionId;
	DWORD		Version;
	//head
	DWORD		HeadVerifyKey1;
	DWORD		HeadVerifyKey2;
	//tail
	DWORD		TailVerifyKey1;
	QWORD		IndexesEntryAddressCryptKey;
	DWORD		TailVerifyKey2;
	//index
	DWORD		IndexCompressedFilenameDataLengthCryptKey1;
	DWORD		IndexCompressedFilenameDataLengthCryptKey2;
	//pkx
	DWORD		dwMaxSinglePckSize;
}PCK_KEYS, *LPPCK_KEYS;

typedef struct _PCK_VERSION_FUNC {

	const		PCK_KEYS*	cPckXorKeys;
	//头的size
	size_t		dwHeadSize;
	//尾的size
	size_t		dwTailSize;
	//fileindex的size
	size_t		dwFileIndexSize;
	//读取头
	//读取尾
	//读取fileindex
	BOOL (*PickIndexData)(void*, void*);
	//填入头
	LPVOID (*FillHeadData)(void*);
	//填入尾
	LPVOID (*FillTailData)(void*);
	//填入fileindex
	LPVOID (*FillIndexData)(void*, void*);
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

typedef struct _PCK_HEAD_V2020 {
	DWORD		dwHeadCheckHead;
	DWORD		dwPckSize;
	DWORD		dwHeadCheckTail;
}PCKHEAD_V2020, *LPPCKHEAD_V2020;

typedef struct _PCK_HEAD_V2030 {
	DWORD		dwHeadCheckHead;
	union {
		QWORD		dwPckSize;
		struct {
			DWORD		dwPckSizeLow;
			DWORD		dwHeadCheckTail;
		};
	};
}PCKHEAD_V2030, *LPPCKHEAD_V2030, PCKHEAD_V2031, *LPPCKHEAD_V2031;

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

typedef struct _PCK_TAIL_V2020 {
	DWORD		dwIndexTableCheckHead;
	DWORD		dwVersion0;
	DWORD		dwCryptedFileIndexesAddr;
	DWORD		dwNullDword;	//a5 != 0 ? 0x80000000 : 0 诛仙 a5 === 0
	char		szAdditionalInfo[PCK_ADDITIONAL_INFO_SIZE];
	DWORD		dwIndexTableCheckTail;
	DWORD		dwFileCount;
	DWORD		dwVersion;
}PCKTAIL_V2020, *LPPCKTAIL_V2020;

typedef struct _PCK_TAIL_V2030 {
	DWORD		dwIndexTableCheckHead;
	union {
		QWORD		dwCryptedFileIndexesAddr;
	struct {
			DWORD		dwVersion0;
			DWORD		dwCryptedFileIndexesAddrHigh;
		};
	};
	DWORD		dwNullDword;
	char		szAdditionalInfo[PCK_ADDITIONAL_INFO_SIZE];
	DWORD		dwIndexTableCheckTail;
	DWORD		dwFileCount;
	DWORD		dwVersion;
}PCKTAIL_V2030, *LPPCKTAIL_V2030;

typedef struct _PCK_TAIL_V2031 {
	DWORD		dwIndexTableCheckHead;
	DWORD		dwVersion0;
	QWORD		dwCryptedFileIndexesAddr;
	DWORD		dwNullDword;
	char		szAdditionalInfo[PCK_ADDITIONAL_INFO_SIZE];
	QWORD		dwIndexTableCheckTail;
	DWORD		dwFileCount;
	DWORD		dwVersion;
}PCKTAIL_V2031, *LPPCKTAIL_V2031;

typedef struct _PCK_FILE_INDEX_V2020 {
	char		szFilename[MAX_PATH_PCK_256];
	DWORD		dwUnknown1;
	DWORD		dwAddressOffset;
	DWORD		dwFileClearTextSize;
	DWORD		dwFileCipherTextSize;
	DWORD		dwUnknown2;
}PCKFILEINDEX_V2020, *LPPCKFILEINDEX_V2020;

typedef struct _PCK_FILE_INDEX_V2030 {
	char		szFilename[MAX_PATH_PCK_256];
	DWORD		dwUnknown1;
	QWORD		dwAddressOffset;
	DWORD		dwFileClearTextSize;
	DWORD		dwFileCipherTextSize;
	DWORD		dwUnknown2;
}PCKFILEINDEX_V2030, *LPPCKFILEINDEX_V2030;

typedef struct _PCK_FILE_INDEX_V2031 {
	char		szFilename[MAX_PATH_PCK_260];
	DWORD		dwUnknown1;
	QWORD		dwAddressOffset;
	DWORD		dwFileClearTextSize;
	DWORD		dwFileCipherTextSize;
	DWORD		dwUnknown2;
}PCKFILEINDEX_V2031, *LPPCKFILEINDEX_V2031;

typedef struct _PCK_FILE_INDEX {
	char		szFilename[MAX_PATH_PCK_260];
	QWORD		dwAddressOffset;
	DWORD		dwFileClearTextSize;
	DWORD		dwFileCipherTextSize;
}PCKFILEINDEX, *LPPCKFILEINDEX;
#pragma pack(pop)

typedef struct _PCK_INDEX_TABLE {
	PCKFILEINDEX	cFileIndex;
	BOOL			bSelected;			//界面上被选择时置1，一般如删除节点时使用
	BOOL			isRecompressed;		//压缩重建时使用
	BOOL			isInvalid;
}PCKINDEXTABLE, *LPPCKINDEXTABLE;


typedef struct _PCK_PATH_NODE {
	char			szName[MAX_PATH_PCK_260];
	DWORD			dwFilesCount;
	DWORD			dwDirsCount;
	QWORD			qdwDirClearTextSize;
	QWORD			qdwDirCipherTextSize;
	LPPCKINDEXTABLE	lpPckIndexTable;
	_PCK_PATH_NODE	*parentfirst;
	_PCK_PATH_NODE	*parent;
	_PCK_PATH_NODE	*child;
	_PCK_PATH_NODE	*next;
}PCK_PATH_NODE, *LPPCK_PATH_NODE;


typedef struct _FILES_TO_COMPRESS {
	DWORD			dwCompressedflag;
	DWORD			dwFileSize;
	char			*lpszFileTitle;
	DWORD			nBytesToCopy;
	//char			szBase64Name[MAX_PATH_PCK];
	char			szFilename[MAX_PATH];
	_FILES_TO_COMPRESS	*next;
	_PCK_INDEX_TABLE	*samePtr;
}FILES_TO_COMPRESS, *LPFILES_TO_COMPRESS;


typedef struct _PCK_INDEX_TABLE_COMPRESS {
	DWORD			dwIndexValueHead;
	DWORD			dwIndexValueTail;
	BYTE			buffer[MAX_INDEXTABLE_CLEARTEXT_LENGTH];
	DWORD			dwIndexDataLength;					//文件索引压缩后的大小
	DWORD			dwCompressedFilesize;				//压缩后的文件大小
	DWORD			dwMallocSize;						//申请空间使用的大小（>=压缩后的文件大小）
	QWORD			dwAddressFileDataToWrite;			//压缩后的数据写入文件的地址
	DWORD			dwAddressOfDuplicateOldDataArea;	//如果使用老数据区，其地址
	BOOL			bInvalid;							//添加模式时，文件名如果重复，使用已存在的文件索引，这个作废
	QWORD			dwAddressAddStep;					//写完文件后，dwAddress的指针应该加上的数字
}PCKINDEXTABLE_COMPRESS, *LPPCKINDEXTABLE_COMPRESS;


typedef struct _PCK_ALL_INFOS {
	QWORD			qwPckSize;
	DWORD			dwFileCount;
	QWORD			dwAddressName;
	char			szAdditionalInfo[PCK_ADDITIONAL_INFO_SIZE];
	TCHAR			szFilename[MAX_PATH];

	const PCK_VERSION_FUNC*	lpDetectedPckVerFunc;
	const PCK_VERSION_FUNC*	lpSaveAsPckVerFunc;

}PCK_ALL_INFOS, *LPPCK_ALL_INFOS;

typedef struct _PCK_WRITING_VARS {

	QWORD	dwAddress;		//压缩算法中使用的参数，当前写入地址或文件大小
	void*	lpFileRead;
	void*	lpFileWrite;
	int		level;

}PCK_WRITING_VARS, *LPPCK_WRITING_VARS;


#endif