//////////////////////////////////////////////////////////////////////
// PckClassExceptionRestore.cpp: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 获取基本信息，用于写入文件后造成的文件损坏的恢复
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2017.6.27
//////////////////////////////////////////////////////////////////////

BOOL GetPckBasicInfo(LPTSTR lpszFile, PCKHEAD_V2020 *lpHead, LPBYTE &lpFileIndexData, DWORD &dwPckFileIndexDataSize);
BOOL SetPckBasicInfo(PCKHEAD_V2020 *lpHead, LPBYTE lpFileIndexData, DWORD &dwPckFileIndexDataSize);

