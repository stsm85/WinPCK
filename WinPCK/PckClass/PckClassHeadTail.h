#pragma once
#include "PckClassBaseFeatures.h"
#include "PckClassZlib.h"

class CPckClassHeadTail :
	public virtual CPckClassBaseFeatures,
	public virtual CPckClassZlib
{
public:
	CPckClassHeadTail();
	~CPckClassHeadTail();

	//设置附加信息
	char*	GetAdditionalInfo();
	BOOL	SetAdditionalInfo();

private:

};
