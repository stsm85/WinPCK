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
#include <thread>

#pragma region 线程控制

//线程运行参数
BOOL CPckControlCenter::isThreadWorking()
{
	return cParams.cVarParams.bThreadRunning;
}

void CPckControlCenter::ForceBreakThreadWorking()
{
	cParams.cVarParams.bForcedStopWorking = TRUE;
}

#pragma endregion


#pragma region 线程任务信息
//
int CPckControlCenter::GetLastErrorMsg()
{
	return cParams.cVarParams.errMessageNo;
}

BOOL CPckControlCenter::isLastOptSuccess()
{
	return (PCK_OK == GetLastErrorMsg());
}

#pragma endregion

#pragma region 内存占用
//内存占用
uint32_t CPckControlCenter::getMTMemoryUsed()
{
	return cParams.cVarParams.dwMTMemoryUsed;
}

//内存值
void CPckControlCenter::setMTMaxMemory(uint32_t dwMTMaxMemory)
{
	if ((0 < dwMTMaxMemory) && (getMaxMemoryAllowed() >= dwMTMaxMemory)) {
		cParams.dwMTMaxMemory = dwMTMaxMemory;
	}
	else {
		cParams.dwMTMaxMemory = getMaxMemoryAllowed();
	}
}

uint32_t CPckControlCenter::getMTMaxMemory()
{
	return cParams.dwMTMaxMemory;
}

//最大内存getDefaultMaxMemoryAllowed
uint32_t CPckControlCenter::getMaxMemoryAllowed()
{
	return MT_MAX_MEMORY;
}

#pragma endregion

#pragma region 线程数


uint32_t CPckControlCenter::getMaxThread()
{
	return cParams.dwMTThread;
}

void CPckControlCenter::setMaxThread(uint32_t dwThread)
{
	cParams.dwMTThread = dwThread;
}

//线程默认参数
uint32_t CPckControlCenter::getMaxThreadUpperLimit()
{
	return (thread::hardware_concurrency() + ((thread::hardware_concurrency() + (thread::hardware_concurrency() & 1)) >> 1));
}

#pragma endregion

#pragma region 压缩等级

//压缩等级
uint32_t CPckControlCenter::getCompressLevel()
{
	return cParams.dwCompressLevel;
}

void CPckControlCenter::setCompressLevel(uint32_t dwCompressLevel)
{
	cParams.dwCompressLevel = dwCompressLevel;
}

//压缩等级默认参数
//参数默认值
uint32_t CPckControlCenter::getDefaultCompressLevel()
{
	return Z_DEFAULT_COMPRESS_LEVEL;
}

uint32_t CPckControlCenter::getMaxCompressLevel()
{
	return MAX_COMPRESS_LEVEL;
}

#pragma endregion


#pragma region 进度相关

uint32_t CPckControlCenter::getUIProgress()
{
	return cParams.cVarParams.dwUIProgress;
}

void CPckControlCenter::setUIProgress(uint32_t dwUIProgress)
{
	cParams.cVarParams.dwUIProgress = dwUIProgress;
}

uint32_t CPckControlCenter::getUIProgressUpper()
{
	return cParams.cVarParams.dwUIProgressUpper;
}

//void CPckControlCenter::setUIProgressUpper(DWORD dwUIProgressUpper)
//{
//	cParams.cVarParams.dwUIProgressUpper = dwUIProgressUpper;
//}

#pragma endregion

#pragma region 打印添加/新增文件结果

uint32_t CPckControlCenter::GetUpdateResult_OldFileCount()
{
	return cParams.cVarParams.dwOldFileCount;
}

uint32_t CPckControlCenter::GetUpdateResult_PrepareToAddFileCount()
{
	return cParams.cVarParams.dwPrepareToAddFileCount;
}

uint32_t CPckControlCenter::GetUpdateResult_ChangedFileCount()
{
	return cParams.cVarParams.dwChangedFileCount;
}

uint32_t CPckControlCenter::GetUpdateResult_DuplicateFileCount()
{
	return cParams.cVarParams.dwDuplicateFileCount;
}

uint32_t CPckControlCenter::GetUpdateResult_FinalFileCount()
{
	return cParams.cVarParams.dwFinalFileCount;
}

#pragma endregion
