#pragma once
#include "PckClassBaseFeatures.h"
#include "PckClassZlib.h"

class CPckClassHeadTail :
	public virtual CPckClassBaseFeatures
{
public:
	CPckClassHeadTail();
	~CPckClassHeadTail();

	//设置附加信息
	const char*	GetAdditionalInfo();
	BOOL	SetAdditionalInfo(const char* lpszAdditionalInfo);

private:

};
