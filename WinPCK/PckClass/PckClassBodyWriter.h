#pragma once
#include "PckClassBaseFeatures.h"
#include "PckClassZlib.h"
#include "MapViewFile.h"
#include "PckClassFileDisk.h"

class CPckClassBodyWriter :
	public virtual CPckClassBaseFeatures,
	public virtual CPckClassZlib,
	public virtual CPckClassFileDisk
{
public:
	CPckClassBodyWriter();
	~CPckClassBodyWriter();

protected:

	//获取数据压缩后的大小，如果源大小小于一定值就不压缩
	DWORD GetCompressBoundSizeByFileSize(LPPCKFILEINDEX	lpPckFileIndex, DWORD dwFileSize);

};

