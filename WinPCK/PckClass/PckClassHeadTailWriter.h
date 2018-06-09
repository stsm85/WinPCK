#pragma once
#include "PckClassHeadTail.h"
#include "MapViewFile.h"


class CPckClassHeadTailWriter :
	public virtual CPckClassHeadTail
{
public:
	CPckClassHeadTailWriter();
	~CPckClassHeadTailWriter();

protected:

	void AfterProcess(CMapViewFileWrite *lpWrite, PCK_ALL_INFOS &PckAllInfo, QWORD &dwAddress, BOOL isRenewAddtional = TRUE);


};
