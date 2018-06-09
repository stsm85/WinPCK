#include "PckClassThreadHelper.h"

void CPckClassThreadHelper::detectMaxAndAddMemory(DWORD dwMallocSize)
{
	EnterCriticalSection(&g_mt_lpMaxMemory);

	if((*mt_lpMaxMemory) >= mt_MaxMemory) {
		LeaveCriticalSection(&g_mt_lpMaxMemory);
		{
			EnterCriticalSection(&g_mt_nMallocBlocked);
			mt_nMallocBlocked++;
			LeaveCriticalSection(&g_mt_nMallocBlocked);
		}
		WaitForSingleObject(mt_evtMaxMemory, INFINITE);
		{
			EnterCriticalSection(&g_mt_nMallocBlocked);
			mt_nMallocBlocked--;
			LeaveCriticalSection(&g_mt_nMallocBlocked);
		}
	} else
		LeaveCriticalSection(&g_mt_lpMaxMemory);

	{
		EnterCriticalSection(&g_mt_lpMaxMemory);
		(*mt_lpMaxMemory) += dwMallocSize;
		LeaveCriticalSection(&g_mt_lpMaxMemory);
	}
}

void CPckClassThreadHelper::freeMaxAndSubtractMemory(DWORD dwMallocSize)
{
	EnterCriticalSection(&g_mt_lpMaxMemory);
	(*mt_lpMaxMemory) -= dwMallocSize;
	if((*mt_lpMaxMemory) < mt_MaxMemory) {
		LeaveCriticalSection(&g_mt_lpMaxMemory);

		EnterCriticalSection(&g_mt_nMallocBlocked);
		int nMallocBlocked = mt_nMallocBlocked;
		LeaveCriticalSection(&g_mt_nMallocBlocked);
		for(int i = 0; i < nMallocBlocked; i++)
			SetEvent(mt_evtMaxMemory);

	} else {
		LeaveCriticalSection(&g_mt_lpMaxMemory);
	}
}