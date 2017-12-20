//////////////////////////////////////////////////////////////////////
// PckConf.h: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 头文件
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2015.5.13
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#pragma warning ( disable : 4005 )
#pragma warning ( disable : 4995 )

//使用压缩程序单线程版本
#define PCK_COMPRESS_NEED_ST	0
