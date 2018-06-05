#include "PckClass.h"

VOID CPckClass::RenameIndex(LPPCK_PATH_NODE lpNode, char* lpszReplaceString)
{
	m_classNode.RenameIndex(lpNode, lpszReplaceString);
}

VOID CPckClass::RenameIndex(LPPCKINDEXTABLE lpIndex, char* lpszReplaceString)
{
	m_classNode.RenameIndex(lpIndex, lpszReplaceString);
}

BOOL CPckClass::RenameNode(LPPCK_PATH_NODE lpNode, char* lpszReplaceString)
{
	return m_classNode.RenameNode(lpNode, lpszReplaceString);
}

BOOL CPckClass::GetCurrentNodeString(char *szCurrentNodePathString, LPPCK_PATH_NODE lpNode)
{
	return m_classNode.GetCurrentNodeString(szCurrentNodePathString, lpNode);
}

VOID CPckClass::DeleteNode(LPPCK_PATH_NODE lpNode)
{
	m_classNode.DeleteNode(lpNode);
}
