//////////////////////////////////////////////////////////////////////
// globals.h: WinPCK 全局头文件
// 界面类的全局头文件
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <windows.h>
#include <assert.h>
#include <vector>
using namespace std;

//****** some defines *******
#define _USE_CUSTOMDRAW_


#define	THIS_NAME			"WinPCK "
#define	THIS_VERSION		"v1.24.0.2 "
#define	THIS_DESC			"完美世界系列游戏PCK文件包综合查看器 " 
#define	THIS_AUTHOR			"程序编写: stsm/liqf/李秋枫\r\n" \
							"E-Mail: stsm85@126.com "
#define THIS_UESDLIB		"本软件开源，源代码地址：\r\nhttps://github.com/stsm85\r\n\r\n" \
							"使用开源库：\r\n" \
							"VC LTL(Copyright (C) Chuyu-Team)\r\n" \
							"libdeflate(Copyright (C) 2016 Eric Biggers)\r\n" \
							"zlib(Copyright (C) 1995-2011 Mark Adler)\r\n" \
							"tlib(Copyright (C) 1996-2010 H.Shirouzu) "

#ifdef _WIN64
	#define	THIS_MAIN_CAPTION	THIS_NAME \
								"64bit " \
								THIS_VERSION
#else
	#define	THIS_MAIN_CAPTION	THIS_NAME \
								THIS_VERSION
#endif
							

//#define	SHELL_LISTVIEW_PARENT_CLASS		TEXT("SHELLDLL_DefView")
#define	SHELL_LISTVIEW_ROOT_CLASS1		TEXT("ExploreWClass")
#define	SHELL_LISTVIEW_ROOT_CLASS2		TEXT("CabinetWClass")
//#define	SHELL_EXENAME					TEXT("\\explorer.exe")

#define	TEXT_INVALID_PATHCHAR			"\\/:*?\"<>|"

#define	TEXT_FILE_FILTER				TEXT(	"PCK压缩文件(*.pck;*.zup)\0*.pck;*.zup\0")	\
										TEXT(	"所有文件\0*.*\0\0")


#define Z_DEFAULT_COMPRESS_LEVEL	9

#define WM_FRESH_MAIN_CAPTION		(WM_USER + 1)
#define WM_TIMER_PROGRESS_100		(WM_USER + 1)

#define	TIMER_PROGRESS				100

#define	MT_MAX_MEMORY				0x80000000	//2048MB

#define MAX_COMPRESS_LEVEL	12

#ifndef SE_CREATE_SYMBOLIC_LINK_NAME
#define SE_CREATE_SYMBOLIC_LINK_NAME  TEXT("SeCreateSymbolicLinkPrivilege")
#endif

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

#endif