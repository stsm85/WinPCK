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
#include "PckClass.h"

BOOL CPckClass::MountPckFile(LPCWSTR	szFile)
{
	try {
		if (!DetectPckVerion(szFile))
			return FALSE;

		if (!ReadPckFileIndexes())
			return FALSE;
		//将最后一个Index的entryType置为PCK_ENTRY_TYPE_TAIL_INDEX
		m_PckAllInfo.lpPckIndexTable[m_PckAllInfo.dwFileCount].entryType = PCK_ENTRY_TYPE_TAIL_INDEX;
		return TRUE;
	}
	catch (MyException e) {
		Logger.e(e.what());
		return FALSE;
	}
	
}

void CPckClass::BuildDirTree()
{
	//将读取的index中的ansi文本全部转换为Unicode
	GenerateUnicodeStringToIndex();
	//根据index中的文件名建立目录树
	ParseIndexTableToNode(m_PckAllInfo.lpPckIndexTable);
}
