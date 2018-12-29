#pragma once
#include "PckClassNode.h"

class CPckClassIndexWriter :
	public virtual CPckClassNode
{
public:
	CPckClassIndexWriter();
	~CPckClassIndexWriter();

protected:
	BOOL WriteAllIndex(CMapViewFileMultiPckWrite *lpWrite, LPPCK_ALL_INFOS lpPckAllInfo, QWORD &dwAddress);

};

