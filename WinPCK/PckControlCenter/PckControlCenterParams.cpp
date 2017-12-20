//////////////////////////////////////////////////////////////////////
// PckControlCenterParams.cpp: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 头文件,界面与PCK类的数据交互，控制中心
//
// 此程序由 李秋枫/stsm/liqf 编写，pck结构参考若水的pck结构.txt，并
// 参考了其易语言代码中并于读pck文件列表的部分
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.10.10
//////////////////////////////////////////////////////////////////////

#include "PckControlCenter.h"



LPPCK_RUNTIME_PARAMS CPckControlCenter::GetParams()
{
	return &cParams;
}

//void CPckControlCenter::SetParams(LPPCK_RUNTIME_PARAMS in)
//{
//	memcpy(&cParams, in, sizeof(PCK_RUNTIME_PARAMS));
//}


//BOOL	CPckControlCenter::ConfirmErrors(LPCSTR lpszMainString, LPCSTR lpszCaption, UINT uType)
//{
//	int rtn = MessageBoxA(hWndMain, lpszMainString, lpszCaption, uType);
//	return ( IDYES == rtn || IDOK == rtn )
//}
//
//BOOL	CPckControlCenter::ConfirmErrors(LPCWSTR lpszMainString, LPCWSTR lpszCaption, UINT uType)
//{
//	int rtn = MessageBoxW(hWndMain, lpszMainString, lpszCaption, uType);
//	return ( IDYES == rtn || IDOK == rtn )
//}


