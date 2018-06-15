#include "PckClassThread.h"


FETCHDATA_RET CPckClassThreadWorker::detectMaxToAddMemory(DWORD dwMallocSize)
{
	while(1) {
		
		if(isThreadWorkerStillWorking()) {

			if(m_lpPckParams->cVarParams.dwMTMemoryUsed >= m_lpPckParams->dwMTMaxMemory) {

				logOutput(__FUNCTION__ "_Sleep", "SleepConditionVariableSRW\r\n");
				m_memoryNotEnoughBlocked = TRUE;
				SleepConditionVariableSRW(&m_cvMemoryNotEnough, &m_LockMaxMemory, 5000, 0/*CONDITION_VARIABLE_LOCKMODE_SHARED*/);

				logOutput(__FUNCTION__ "_Sleep", "Awake\r\n");
			} else {
				return FD_OK;
			}
		} else {
			logOutput(__FUNCTION__ "_Sleep", "user cancled\r\n");
			return FD_CANCEL;
			//break;
		}
	}

	return FD_OK;
}

FETCHDATA_RET CPckClassThreadWorker::detectMaxAndAddMemory(LPBYTE &_out_buffer, DWORD dwMallocSize)
{
	FETCHDATA_RET rtn = FD_OK;
	int retry_count = 10;
	AcquireSRWLockExclusive(&m_LockMaxMemory);

	if(FD_OK == (rtn = detectMaxToAddMemory(dwMallocSize))) {

		while(NULL == (_out_buffer = (LPBYTE)malloc(dwMallocSize))) {
			assert(FALSE);
			logOutput(__FUNCTION__ "_Sleep", "malloc failure\r\n");
			m_memoryNotEnoughBlocked = TRUE;
			SleepConditionVariableSRW(&m_cvMemoryNotEnough, &m_LockMaxMemory, 5000, 0/*CONDITION_VARIABLE_LOCKMODE_SHARED*/);

			if(!isThreadWorkerStillWorking()) {
				rtn = FD_CANCEL;
				break;
			}

			if(0 < retry_count--)continue;
			rtn = FD_ERR;
			break;
		}
		
		if(NULL != _out_buffer)m_lpPckParams->cVarParams.dwMTMemoryUsed += dwMallocSize;
	}

	ReleaseSRWLockExclusive(&m_LockMaxMemory);
	return rtn;
}


void CPckClassThreadWorker::freeMaxToSubtractMemory(DWORD dwMallocSize)
{

	AcquireSRWLockShared(&m_LockMaxMemory);
	DWORD dwMTMemoryUsed = m_lpPckParams->cVarParams.dwMTMemoryUsed;
	ReleaseSRWLockShared(&m_LockMaxMemory);

	dwMTMemoryUsed -= dwMallocSize;
	AcquireSRWLockExclusive(&m_LockMaxMemory);
	m_lpPckParams->cVarParams.dwMTMemoryUsed = dwMTMemoryUsed;
	ReleaseSRWLockExclusive(&m_LockMaxMemory);

	if(m_memoryNotEnoughBlocked){
		logOutput(__FUNCTION__ "_Sleep", "WakeAllConditionVariable\r\n");
		m_memoryNotEnoughBlocked = FALSE;
		WakeAllConditionVariable(&m_cvMemoryNotEnough);
	}

}

void CPckClassThreadWorker::freeMaxAndSubtractMemory(LPBYTE &_In_out_buffer, DWORD dwMallocSize)
{
	free(_In_out_buffer);
	freeMaxToSubtractMemory(dwMallocSize);
}