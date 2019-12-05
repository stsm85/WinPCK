#pragma once

#include <Windows.h>
#include <stdint.h>
#include "gccException.h"

typedef unsigned long       ulong_t;
typedef int                 BOOL;
typedef unsigned char       BYTE;

typedef const wchar_t * 	LPCWSTR;
typedef wchar_t *			LPWSTR;
typedef const char * 		LPCSTR;
typedef char *				LPSTR;

/* Define ssize_t */
#ifdef _WIN64
typedef long long ssize_t;
#else
typedef int ssize_t;
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif


//版本信息
#define WINPCK_VERSION_NUMBER  1,33,0,2
#define WINPCK_VERSION        "1.33.0.2"

//调度输出
#ifdef _DEBUG
#define PCK_DEBUG_OUTPUT		0
#define PCK_DEBUG_OUTPUT_FILE	0
#else
#define PCK_DEBUG_OUTPUT		0
#define PCK_DEBUG_OUTPUT_FILE	0
#endif

#define PCK_V2031_ENABLE		0

//打开、关闭、复原等事件注册
#define PCK_FILE_OPEN_SUCESS		1
#define PCK_FILE_CLOSE				2


#define MAX_PATH			260

//zlib
#define Z_OK				0

//params
#define	MT_MAX_MEMORY				0x80000000	//2048MB
#define MAX_COMPRESS_LEVEL			12
#define Z_DEFAULT_COMPRESS_LEVEL	9

#define PCK_OK					0   /* Successful result */
/* beginning-of-error-codes */
#define PCK_MSG_USERCANCELED	1   /* 用户取消 */
#define PCK_ERROR				2   /* Generic error */
#define PCK_ERR_DISKFULL		3	/* 磁盘已满 */	//磁盘空间不足，申请空间：%d，剩余空间：%d
#define PCK_ERR_VIEWMAP_FAIL	4	/* Mapping 失败 */
#define PCK_ERR_VIEW			5	/* View 失败 */
#define PCK_ERR_OPENMAPVIEWR	6	/* OpenMappingViewAllRead 失败 */
#define PCK_ERR_MALLOC			7	/* 内存申请失败 */

/* end-of-error-codes */

//数字转字符时使用的字符串长度
#define CHAR_NUM_LEN 12

#define	MAX_INDEXTABLE_CLEARTEXT_LENGTH	0x120
#define MAX_TAIL_LENGTH					300

#define	PCK_BEGINCOMPRESS_SIZE			20
#define	MAX_PATH_PCK_256				256
#define	MAX_PATH_PCK_260				260
#define PCK_ADDITIONAL_INFO_SIZE		252
#define PCK_DATA_START_AT				12

#define PCK_ENTRY_TYPE_INDEX			1
#define PCK_ENTRY_TYPE_NODE				2
#define PCK_ENTRY_TYPE_FOLDER			4
#define PCK_ENTRY_TYPE_DOTDOT			8
#define PCK_ENTRY_TYPE_ROOT				16
#define PCK_ENTRY_TYPE_TAIL_INDEX		0x80000000

#define	PCK_ADDITIONAL_KEY				"Angelica File Package"
#define	PCK_ADDITIONAL_INFO				PCK_ADDITIONAL_KEY", Perfect World Co. Ltd. 2002~2008. All Rights Reserved.\r\nCreated by WinPCK v" WINPCK_VERSION  
