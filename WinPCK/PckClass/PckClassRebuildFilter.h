#pragma once

namespace NPckClassRebuildFilter
{
	BOOL	ParseScript(LPCTSTR lpszScriptFile, PCK_ALL_INFOS &_DstPckAllInfo);
	void	ResetRebuildFilterInIndexList(PCK_ALL_INFOS &_DstPckAllInfo);
}

