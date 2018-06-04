#pragma once
#include "PckClass.h"
namespace NPckClassAllocFuncs
{
	void*	AllocMemory(size_t	sizeStuct);
	VOID	DeAllocMultiNodes(LPPCK_PATH_NODE lpThisNode);

	//VOID	DeallocateFileinfo(LPFILES_TO_COMPRESS p_firstFile);
}
