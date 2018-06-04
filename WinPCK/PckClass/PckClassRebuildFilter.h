#pragma once

class CPckClassRebuildScriptFilter
{
public:
	CPckClassRebuildScriptFilter(PCK_ALL_INFOS _DstPckAllInfo, LPPCKINDEXTABLE _lpDstPckIndexTable, PCK_PATH_NODE _DstRootNode);
	~CPckClassRebuildScriptFilter();


	BOOL	ParseScript(LPCTSTR lpszScriptFile);
	static void	ResetRebuildFilterInIndexList(LPPCKINDEXTABLE lpDstPckIndexTable, DWORD dwFileCount);

private:
	BOOL	ApplyScript2IndexList(VOID *pfirstFileOp);	//将脚本内容应用到文件列表中

	PCK_ALL_INFOS			m_DstPckAllInfo;
	LPPCKINDEXTABLE			m_lpDstPckIndexTable;	//索引
	PCK_PATH_NODE			m_DstRootNode;			//根节点

	CPckClassLog	m_PckLog;						//打印日志

};

