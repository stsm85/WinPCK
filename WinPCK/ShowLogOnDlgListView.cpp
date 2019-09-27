//////////////////////////////////////////////////////////////////////
// ShowLogOnDlgListView.cpp: 将日志显示在listView中
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2018.6.4
//////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include "ShowLogOnDlgListView.h"
#include "resource.h"
#include <commctrl.h>
#include <stdio.h>

CLogUnits LogUnits;

void	PreInsertLogToList(const char ch, const wchar_t *str)
{
	LogUnits.InsertLog(ch, str);
}


CLogUnits::CLogUnits()
{
}

CLogUnits::~CLogUnits()
{
}
