#include <pch.h>
#include "PckThreadRunner.h"

FETCHDATA_RET CPckThreadRunner::detectMaxToAddMemory(DWORD dwMallocSize)
{
	std::unique_lock<std::mutex> lckMaxMemory(m_LockMaxMemory);
	while (1) {

		if (!m_lpPckClassBase->CheckIfNeedForcedStopWorking()) {

			if (m_lpPckParams->cVarParams.dwMTMemoryUsed >= m_lpPckParams->dwMTMaxMemory) {

				Logger.trace(__FUNCTION__ "_Sleep, SleepConditionVariableSRW, dwMTMemoryUsed = {}, dwMTMaxMemory = {}", m_lpPckParams->cVarParams.dwMTMemoryUsed, m_lpPckParams->dwMTMaxMemory);
				m_memoryNotEnoughBlocked = TRUE;

				if (m_cvMemoryNotEnough.wait_for(lckMaxMemory, std::chrono::seconds(5)) == std::cv_status::timeout)
					Logger.trace(__FUNCTION__ "_Sleep, TimeOut, dwMTMemoryUsed = {}, dwMTMaxMemory = {}", m_lpPckParams->cVarParams.dwMTMemoryUsed, m_lpPckParams->dwMTMaxMemory);
				else
					Logger.trace(__FUNCTION__ "_Sleep, Awake, dwMTMemoryUsed = {}, dwMTMaxMemory = {}", m_lpPckParams->cVarParams.dwMTMemoryUsed, m_lpPckParams->dwMTMaxMemory);

			}
			else {
				return FD_OK;
			}
		}
		else {
			Logger.trace(__FUNCTION__ "_Sleep, user cancled");
			return FD_CANCEL;
		}
	}

	return FD_OK;
}

FETCHDATA_RET CPckThreadRunner::detectMaxAndAddMemory(LPBYTE &_out_buffer, DWORD dwMallocSize)
{
	FETCHDATA_RET rtn = FD_OK;
	int retry_count = 10;

	BOOL bMemIsEnough = (FD_OK == (rtn = detectMaxToAddMemory(dwMallocSize)));

	if (bMemIsEnough) {

		while (NULL == (_out_buffer = (LPBYTE)malloc(dwMallocSize + 1))) {
			//ƒ⁄¥Ê…Í«Î ß∞‹£¨¥¶¿Ì
			assert(FALSE);
			Logger.trace(__FUNCTION__ "_Sleep, malloc failure");
			m_memoryNotEnoughBlocked = TRUE;

			std::unique_lock<std::mutex> lckMaxMemory(m_LockMaxMemory);
			m_cvMemoryNotEnough.wait_for(lckMaxMemory, std::chrono::seconds(5));

			if (m_lpPckClassBase->CheckIfNeedForcedStopWorking()) {
				rtn = FD_CANCEL;
				break;
			}

			if (0 < retry_count--)continue;
			rtn = FD_ERR;
			break;
		}

		if (NULL != _out_buffer)
		{
			std::lock_guard<std::mutex> lckMaxMemory(m_LockMaxMemory);
			m_lpPckParams->cVarParams.dwMTMemoryUsed += dwMallocSize;
			Logger.trace(__FUNCTION__ "_Addmem, malloc size {}, dwMTMemoryUsed = {}", dwMallocSize, m_lpPckParams->cVarParams.dwMTMemoryUsed);
		}
	}

	return rtn;
}


void CPckThreadRunner::freeMaxToSubtractMemory(DWORD dwMallocSize)
{
	{
		std::lock_guard<std::mutex> lckMaxMemory(m_LockMaxMemory);
		m_lpPckParams->cVarParams.dwMTMemoryUsed -= dwMallocSize;
		Logger.trace(__FUNCTION__ "_freemem, dwMTMemoryUsed = {}, -dwMallocSize = {}", m_lpPckParams->cVarParams.dwMTMemoryUsed, dwMallocSize);
	}

	if (m_memoryNotEnoughBlocked) {
		Logger.trace(__FUNCTION__ "_Sleep, WakeAllConditionVariable");
		m_memoryNotEnoughBlocked = FALSE;
		m_cvMemoryNotEnough.notify_all();
	}
}

void CPckThreadRunner::freeMaxAndSubtractMemory(LPBYTE &_In_out_buffer, DWORD dwMallocSize)
{
	free(_In_out_buffer);
	freeMaxToSubtractMemory(dwMallocSize);
}