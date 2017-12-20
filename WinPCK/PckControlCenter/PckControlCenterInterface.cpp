
//////////////////////////////////////////////////////////////////////
// PckControlCenterInterface.cpp: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 头文件,界面与PCK类的数据交互，控制中心
//
// 此程序由 李秋枫/stsm/liqf 编写，pck结构参考若水的pck结构.txt，并
// 参考了其易语言代码中并于读pck文件列表的部分
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2015.5.19
//////////////////////////////////////////////////////////////////////

#include "PckControlCenter.h"
#include "PckClass.h"

CONST int	CPckControlCenter::GetListCurrentHotItem()
{
	return cParams.iListHotItem;
}

void CPckControlCenter::SetListCurrentHotItem(int _val)
{
	cParams.iListHotItem = _val;
}

CONST BOOL	CPckControlCenter::GetListInSearchMode()
{
	return m_isSearchMode;
}

void CPckControlCenter::SetListInSearchMode(BOOL _val)
{
	m_isSearchMode = _val;
}

LPCTSTR	CPckControlCenter::GetSaveDlgFilterString()
{
	return m_lpClassPck->GetSaveDlgFilterString();//cParams.lpPckVersion->GetSaveDlgFilterString();
}

BOOL CPckControlCenter::isSupportAddFileToPck()
{
	switch(m_emunFileFormat)
	{
	case FMTPCK_PCK:
		return TRUE;
		break;

	case FMTPCK_ZUP:
		return FALSE;
		break;
	}
	return FALSE;
}
