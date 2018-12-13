#pragma once
#include "PckClassBaseFeatures.h"
#include "PckClassCodepage.h"
#include "PckClassZlib.h"
#include "MapViewFileMultiPck.h"

class CPckClassIndex :
	public virtual CPckClassBaseFeatures
{
public:
	CPckClassIndex();
	~CPckClassIndex();

	//根据文件数量申请文件索引的内存
	LPPCKINDEXTABLE		AllocPckIndexTableByFileCount();
	//将lpPckIndexTable->cFileIndex.szFilename 中的Ansi字符转换志Unicode并写到lpPckIndexTable->cFileIndex.szwFilename中
	void		GenerateUnicodeStringToIndex();
	// 文件头、尾等结构体的读取
	BOOL		ReadPckFileIndexes();

protected:

	void*	AllocMemory(size_t	sizeStuct);
	//重建时重新计算文件数量，除去无效的和文件名重复的
	DWORD	ReCountFiles();
	//按修改后的索引数据按版本填入到结构体中并压缩好
	LPPCKINDEXTABLE_COMPRESS FillAndCompressIndexData(LPPCKINDEXTABLE_COMPRESS lpPckIndexTableComped, LPPCKFILEINDEX lpPckFileIndexToCompress);

};

