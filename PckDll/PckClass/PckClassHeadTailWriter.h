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

	BOOL WriteHeadAndTail(CMapViewFileMultiPckWrite *lpWrite, LPPCK_ALL_INFOS PckAllInfo, QWORD &dwAddress, BOOL isRenewAddtional = TRUE);


};
