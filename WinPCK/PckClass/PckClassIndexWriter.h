#pragma once
#include "PckClassNode.h"

class CPckClassIndexWriter :
	public virtual CPckClassNode
{
public:
	CPckClassIndexWriter();
	~CPckClassIndexWriter();

protected:

	BOOL WritePckIndex(CMapViewFileWrite *lpWrite, LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr, QWORD &dwAddress);
	BOOL WritePckIndexTable(CMapViewFileWrite *lpWrite, LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr, DWORD &dwFileCount, QWORD &dwAddress);
};

