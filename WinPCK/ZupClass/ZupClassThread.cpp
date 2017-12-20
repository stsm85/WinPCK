//////////////////////////////////////////////////////////////////////
// ZupClassThread.cpp: 用于解析完美世界公司的zup文件中的数据，并显示在List中
// 头文件
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.5.23
//////////////////////////////////////////////////////////////////////

#include "ZupClass.h"


BOOL CZupClass::CreatePckFile(LPTSTR szPckFile, LPTSTR szPath)
{
	PrintLogE(TEXT_NOTSUPPORT);
	return FALSE;
}

//重建pck文件
BOOL CZupClass::RebuildPckFile(LPTSTR szRebuildPckFile)
{
	PrintLogE(TEXT_NOTSUPPORT);
	return FALSE;
}

BOOL CZupClass::RecompressPckFile(LPTSTR szRecompressPckFile)
{
	PrintLogE(TEXT_NOTSUPPORT);
	return FALSE;
}

//更新pck文件//有bug
BOOL CZupClass::UpdatePckFile(LPTSTR szPckFile, TCHAR (*lpszFilePath)[MAX_PATH], int nFileCount, LPPCK_PATH_NODE lpNodeToInsert)
{
	PrintLogE(TEXT_NOTSUPPORT);
	return FALSE;
}

//重命名文件
BOOL CZupClass::RenameFilename()
{
	PrintLogE(TEXT_NOTSUPPORT);
	return FALSE;
}




