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

#include "PckDefines.h"
#include <vector>
using namespace std;

#if !defined(_PCKSTRUCTS_H_)
#define _PCKSTRUCTS_H_


typedef enum _FMTPCK
{
	FMTPCK_PCK = 0,
	FMTPCK_ZUP = 1,
	FMTPCK_CUP = 2,
	FMTPCK_UNKNOWN = 0x7fffffff
}FMTPCK;


typedef enum _PCKVER
{
	PCK_V2020,
	PCK_V2031,
	PCK_VXAJH
}PCK_CATEGORY;

typedef struct _PCK_KEYS
{
	int			id;
	TCHAR		name[64];
	PCK_CATEGORY	CategoryId;
	DWORD		Version;
	//head
	DWORD		HeadVerifyKey1;
	DWORD		HeadVerifyKey2;
	//tail
	DWORD		TailVerifyKey1;
	QWORD		IndexesEntryAddressCryptKey;
	DWORD		TailVerifyKey2;	//在文件尾第-3个DWORD(xx xx xx xx 00 00 00 00 00 00 00 00)
	//index
	DWORD		IndexCompressedFilenameDataLengthCryptKey1;//pck中的每一个文件都有一个索引存在文件尾（除最后280字节）
	DWORD		IndexCompressedFilenameDataLengthCryptKey2;//数据为len^key1, len^key2, data(PCKFILEINDEX), (11 11 11 11 22 22 22 22 dd dd dd dd .....)
	//pkx
	DWORD		dwMaxSinglePckSize;
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
	LPVOID(*FillHeadData)(void*);
	//填入尾
	LPVOID(*FillTailData)(void*);
	//填入fileindex
	LPVOID(*FillIndexData)(void*, void*);
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
	DWORD		dwHeadCheckHead;
	DWORD		dwPckSize;
	DWORD		dwHeadCheckTail;
}PCKHEAD_V2020, *LPPCKHEAD_V2020;

typedef struct _PCK_HEAD_V2030
{
	DWORD		dwHeadCheckHead;
	union
	{
		QWORD		dwPckSize;
		struct
		{
			DWORD		dwPckSizeLow;
			DWORD		dwHeadCheckTail;
		};
	};
}PCKHEAD_V2030, *LPPCKHEAD_V2030, PCKHEAD_VXAJH, *LPPCKHEAD_VXAJH;

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
	DWORD		dwIndexTableCheckHead;
	DWORD		dwVersion0;
	DWORD		dwCryptedFileIndexesAddr;
	DWORD		dwNullDword;	//a5 != 0 ? 0x80000000 : 0 诛仙 a5 === 0
	char		szAdditionalInfo[PCK_ADDITIONAL_INFO_SIZE];
	DWORD		dwIndexTableCheckTail;
	DWORD		dwFileCount;
	DWORD		dwVersion;
}PCKTAIL_V2020, *LPPCKTAIL_V2020;

typedef struct _PCK_TAIL_V2030
{
	DWORD		dwIndexTableCheckHead;
	DWORD		dwVersion0;
	QWORD		dwCryptedFileIndexesAddr;
	DWORD		dwNullDword;
	char		szAdditionalInfo[PCK_ADDITIONAL_INFO_SIZE];
	QWORD		dwIndexTableCheckTail;
	DWORD		dwFileCount;
	DWORD		dwVersion;
}PCKTAIL_V2030, *LPPCKTAIL_V2030;

typedef struct _PCK_TAIL_VXAJH
{
	DWORD		dwIndexTableCheckHead;
	union
	{
		QWORD		dwCryptedFileIndexesAddr;
		struct
		{
			DWORD		dwVersion0;
			DWORD		dwCryptedFileIndexesAddrHigh;
		};
	};
	DWORD		dwNullDword;
	char		szAdditionalInfo[PCK_ADDITIONAL_INFO_SIZE];
	DWORD		dwIndexTableCheckTail;
	DWORD		dwFileCount;
	DWORD		dwVersion;
}PCKTAIL_VXAJH, *LPPCKTAIL_VXAJH;

typedef struct _PCK_FILE_INDEX_V2020
{
	char		szFilename[MAX_PATH_PCK_256];
	DWORD		dwUnknown1;
	DWORD		dwAddressOffset;
	DWORD		dwFileClearTextSize;
	DWORD		dwFileCipherTextSize;
	DWORD		dwUnknown2;
}PCKFILEINDEX_V2020, *LPPCKFILEINDEX_V2020;

typedef struct _PCK_FILE_INDEX_V2030
{
	char		szFilename[MAX_PATH_PCK_260];
	DWORD		dwUnknown1;
	QWORD		dwAddressOffset;
	DWORD		dwFileClearTextSize;
	DWORD		dwFileCipherTextSize;
	DWORD		dwUnknown2;
}PCKFILEINDEX_V2030, *LPPCKFILEINDEX_V2030;

typedef struct _PCK_FILE_INDEX_VXAJH
{
	char		szFilename[MAX_PATH_PCK_256];
	DWORD		dwUnknown1;
	QWORD		dwAddressOffset;
	DWORD		dwFileClearTextSize;
	DWORD		dwFileCipherTextSize;
	DWORD		dwUnknown2;
}PCKFILEINDEX_VXAJH, *LPPCKFILEINDEX_VXAJH;

typedef struct _PCK_FILE_INDEX
{
	char		szFilename[MAX_PATH_PCK_260];		//使用pck内部ansi编码，默认CP936
	wchar_t		szwFilename[MAX_PATH_PCK_260];		//通用Unicode编码
	QWORD		dwAddressOffset;
	DWORD		dwFileClearTextSize;
	DWORD		dwFileCipherTextSize;
}PCKFILEINDEX, *LPPCKFILEINDEX;
#pragma pack(pop)


typedef struct _PCK_INDEX_TABLE
{
	PCKFILEINDEX	cFileIndex;
	//BOOL			bSelected;			//界面上被选择时置1，一般如界面操作删除、重命名节点时使用
	//BOOL			isRecompressed;		//压缩重建时使用，在进行多线程操作时，已经进行过重压缩操作的文件此值置TRUE
	BOOL			isInvalid;			//添加文件时，如果文件名重复，则被覆盖的文件设置为TRUE（在打开文件建立文件目录树时，同名的文件，后面的会覆盖前面的，被覆盖的此值设为TRUE)
	BOOL			isProtected;		//文件设置保护，不允许删除
	BOOL			isToDeDelete;		//设置为TRUE后，文件会被删除，isProtected为TRUE的除外
	DWORD			dwMallocSize;						//申请空间使用的大小（>=压缩后的文件大小）
	LPBYTE			compressed_file_data;				//此index对应的压缩数据
	size_t			nFilelenBytes;			//文件名(pck ansi)长度字节数
	size_t			nFilelenLeftBytes;		//文件名(pck ansi)剩余可用字节数，重命名时用，使用MAX_PATH_PCK_256
}PCKINDEXTABLE, *LPPCKINDEXTABLE;


typedef struct _PCK_PATH_NODE
{
	wchar_t			szName[MAX_PATH_PCK_260];
	DWORD			dwFilesCount;
	DWORD			dwDirsCount;
	size_t			nNameSizeAnsi;		//节点名的pck ansi 长度
	size_t			nMaxNameSizeAnsi;	//
	QWORD			qdwDirClearTextSize;
	QWORD			qdwDirCipherTextSize;
	LPPCKINDEXTABLE	lpPckIndexTable;
	_PCK_PATH_NODE	*parentfirst;		//非根目录下的..目录使用，指向..目录在上级目录的节点
	_PCK_PATH_NODE	*parent;			//非根目录下的..目录使用，指向本目录在上级目录的节点
	_PCK_PATH_NODE	*child;				//普通目录指向下级目录的..目录的节点
	_PCK_PATH_NODE	*next;				//本级目录指向下一项的节点
}PCK_PATH_NODE, *LPPCK_PATH_NODE;


typedef struct _FILES_TO_COMPRESS
{
#ifdef _DEBUG
	int				id;
#endif
	DWORD			dwCompressedflag;
	DWORD			dwFileSize;
	DWORD			nBytesToCopy;
	//char			szBase64Name[MAX_PATH_PCK];
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
			DWORD			dwIndexValueHead;
			DWORD			dwIndexValueTail;
			BYTE			buffer[MAX_INDEXTABLE_CLEARTEXT_LENGTH];
		};
	};
	DWORD			dwIndexDataLength;					//文件索引压缩后的大小
	DWORD			dwCompressedFilesize;				//压缩后的文件大小
	DWORD			dwMallocSize;						//申请空间使用的大小（>=压缩后的文件大小）
	QWORD			dwAddressFileDataToWrite;			//压缩后的数据写入文件的地址
	DWORD			dwAddressOfDuplicateOldDataArea;	//如果使用老数据区，其地址
	//BOOL			bInvalid;							//添加模式时，文件名如果重复，使用已存在的文件索引，这个作废
	//QWORD			dwAddressAddStep;					//写完文件后，dwAddress的指针应该加上的数字
}PCKINDEXTABLE_COMPRESS, *LPPCKINDEXTABLE_COMPRESS;


typedef struct _PCK_ALL_INFOS
{
	BOOL				isPckFileLoaded;	//是否已成功加载PCK文件 
	QWORD				qwPckSize;
	DWORD				dwFileCount;
	QWORD				dwAddressOfFilenameIndex;		//此值指向pck文件数据区的末尾，也就是文件索引的压缩数据的起始位置
	char				szAdditionalInfo[PCK_ADDITIONAL_INFO_SIZE];
	TCHAR				szFilename[MAX_PATH];
	TCHAR				szFileTitle[MAX_PATH];

	LPPCKINDEXTABLE		lpPckIndexTable;	//PCK文件的索引
	PCK_PATH_NODE		cRootNode;			//PCK文件节点的根节点

	TCHAR				szNewFilename[MAX_PATH];
	DWORD				dwFileCountOld;
	DWORD				dwFileCountToAdd;
	DWORD				dwFinalFileCount;

	vector<FILES_TO_COMPRESS>				*lpFilesToBeAdded;
	const vector<PCKINDEXTABLE_COMPRESS>	*lpPckIndexTableToAdd;

	const PCK_VERSION_FUNC*	lpDetectedPckVerFunc;
	const PCK_VERSION_FUNC*	lpSaveAsPckVerFunc;

}PCK_ALL_INFOS, *LPPCK_ALL_INFOS;

typedef struct _RESTORE_INFOS
{
	BOOL			isValid;
	TCHAR			szFile[MAX_PATH];
	DWORD			dwMaxSinglePckSize;
	PCKHEAD_V2020	Head;
	QWORD			dwAddressOfFilenameIndex;
	size_t			sizeOfIndexTailBuffer;
	LPBYTE			lpIndexTailBuffer;
}RESTORE_INFOS, *LPRESTORE_INFOS;


#endif