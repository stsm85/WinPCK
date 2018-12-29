#include "PckClassThread.h"


FETCHDATA_RET CPckClassThreadWorker::detectMaxToAddMemory(DWORD dwMallocSize)
{
	while(1) {
		
		if(!CheckIfNeedForcedStopWorking()) {

			if(m_lpPckParams->cVarParams.dwMTMemoryUsed >= m_lpPckParams->dwMTMaxMemory) {

				logOutput(__FUNCTION__ "_Sleep", "SleepConditionVariableSRW, dwMTMemoryUsed = %u, dwMTMaxMemory = %u\r\n", m_lpPckParams->cVarParams.dwMTMemoryUsed, m_lpPckParams->dwMTMaxMemory);
				m_memoryNotEnoughBlocked = TRUE;
				SleepConditionVariableSRW(&m_cvMemoryNotEnough, &m_LockMaxMemory, 5000, 0/*CONDITION_VARIABLE_LOCKMODE_SHARED*/);

				logOutput(__FUNCTION__ "_Sleep", "Awake, dwMTMemoryUsed = %u, dwMTMaxMemory = %du\r\n", m_lpPckParams->cVarParams.dwMTMemoryUsed, m_lpPckParams->dwMTMaxMemory);
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
	BOOL bMemIsEnough = (FD_OK == (rtn = detectMaxToAddMemory(dwMallocSize)));
	ReleaseSRWLockExclusive(&m_LockMaxMemory);

	if(bMemIsEnough) {

		while(NULL == (_out_buffer = (LPBYTE)malloc(dwMallocSize+1))) {
			assert(FALSE);
			logOutput(__FUNCTION__ "_Sleep", "malloc failure\r\n");
			m_memoryNotEnoughBlocked = TRUE;
			SleepConditionVariableSRW(&m_cvMemoryNotEnough, &m_LockMaxMemory, 5000, 0/*CONDITION_VARIABLE_LOCKMODE_SHARED*/);

			if(CheckIfNeedForcedStopWorking()) {
				rtn = FD_CANCEL;
				break;
			}

			if(0 < retry_count--)continue;
			rtn = FD_ERR;
			break;
		}
		
		if (NULL != _out_buffer)
		{
			AcquireSRWLockExclusive(&m_LockMaxMemory);
			m_lpPckParams->cVarParams.dwMTMemoryUsed += dwMallocSize;
			logOutput(__FUNCTION__ "_Addmem", "malloc size %u, dwMTMemoryUsed = %u\r\n", dwMallocSize, m_lpPckParams->cVarParams.dwMTMemoryUsed);
			ReleaseSRWLockExclusive(&m_LockMaxMemory);
		}
	}

	return rtn;
}


void CPckClassThreadWorker::freeMaxToSubtractMemory(DWORD dwMallocSize)
{

	AcquireSRWLockExclusive(&m_LockMaxMemory);
	m_lpPckParams->cVarParams.dwMTMemoryUsed -= dwMallocSize;
	logOutput(__FUNCTION__ "_freemem", "dwMTMemoryUsed = %u, -dwMallocSize = %u\r\n", m_lpPckParams->cVarParams.dwMTMemoryUsed, dwMallocSize);
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