#pragma once
#include "PckClassHeadTail.h"
#include "MapViewFileMultiPck.h"


class CPckClassHeadTailWriter :
	public virtual CPckClassHeadTail
{
public:
	CPckClassHeadTailWriter();
	~CPckClassHeadTailWriter();

protected:

	BOOL AfterProcess(CMapViewFileMultiPckWrite *lpWrite, PCK_ALL_INFOS &PckAllInfo, QWORD &dwAddress, BOOL isRenewAddtional = TRUE);


};
