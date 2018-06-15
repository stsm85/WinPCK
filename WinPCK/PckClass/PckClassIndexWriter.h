#pragma once
#include "PckClassNode.h"

class CPckClassIndexWriter :
	public virtual CPckClassNode
{
public:
	CPckClassIndexWriter();
	~CPckClassIndexWriter();

protected:
	BOOL WriteAllIndex(CMapViewFileWrite *lpWrite, LPPCK_ALL_INFOS lpPckAllInfo, QWORD &dwAddress);

private:
	BOOL WritePckIndex(CMapViewFileWrite *lpWrite, const PCKINDEXTABLE_COMPRESS *lpPckIndexTablePtr, QWORD &dwAddress);
	//BOOL WritePckIndexTable(CMapViewFileWrite *lpWrite, LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr, DWORD &dwFileCount, QWORD &dwAddress);

};

