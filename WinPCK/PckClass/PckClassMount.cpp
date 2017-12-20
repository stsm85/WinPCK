//////////////////////////////////////////////////////////////////////
// PckClassMount.cpp: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 有关类的初始化等
//
// 此程序由 李秋枫/stsm/liqf 编写，pck结构参考若水的pck结构.txt，并
// 参考了其易语言代码中并于读pck文件列表的部分
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2015.5.27
//////////////////////////////////////////////////////////////////////
#pragma warning ( disable : 4996 )

#include "PckClass.h"

BOOL CPckClass::MountPckFile(LPCTSTR	szFile)
{
	if(!DetectPckVerion(szFile, &m_PckAllInfo))
		return FALSE;

	if(!ReadPckFileIndexes())
		return FALSE;

	return TRUE;
}

void CPckClass::BuildDirTree()
{

	LPPCKINDEXTABLE lpPckIndexTable = m_lpPckIndexTable;

	for(DWORD i = 0;i<m_PckAllInfo.dwFileCount;++i)
	{
		//建立目录
		AddFileToNode(&m_RootNode, lpPckIndexTable);
		++lpPckIndexTable;
	}

}
