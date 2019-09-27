//////////////////////////////////////////////////////////////////////
// PckClassRebuildFilter.cpp: 用于解析过滤脚本
//
// 此程序由 李秋枫/stsm/liqf 编写，部分代码改写自RapidCRC
//
// 此代码开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2018.5.15
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include "MapViewFileMultiPck.h"
#include "PckClassRebuildFilter.h"
#include "CharsCodeConv.h"
#include "TextLineSpliter.h"

#define MAX_SCRIPT_SIZE	(10*1024*1024)

CPckClassRebuildFilter::CPckClassRebuildFilter()
{
}

CPckClassRebuildFilter::~CPckClassRebuildFilter()
{
	ResetRebuildFilterInIndexList();
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

#pragma region 读取文件并转换为Unicode

BOOL CPckClassRebuildFilter::OpenScriptFileAndConvBufToUcs2(const wchar_t * lpszScriptFile)
{
	char   * lpBufferToRead;
	CMapViewFileRead	cFileRead;
	
	CTextUnitsW	cText2Line;

	//读取文件所有字符
	if (nullptr == (lpBufferToRead = (char*)cFileRead.OpenMappingViewAllRead(lpszScriptFile))) 
		return FALSE;

	CTextConv2UCS2 cText2Ucs;
	const wchar_t* lpszUnicodeString = cText2Ucs.GetUnicodeString(lpBufferToRead, cFileRead.GetFileSize());

	cText2Line.SplitLine(lpszUnicodeString, m_ScriptLines, LINE_TRIM_LEFT | LINE_TRIM_RIGHT | LINE_EMPTY_DELETE);

	return TRUE;
}
#pragma endregion


BOOL CPckClassRebuildFilter::ParseOneLine(FILEOP * pFileOp, LPCWSTR lpszLine)
{
	wchar_t szOperator[16] = { 0 };
	//首先检查16个字符内有没有空格或tab
	const wchar_t *lpszCell = lpszLine, *lpszSearch = lpszLine;
	size_t count = 0;

	wchar_t *lpszOperator = szOperator;
	BOOL isValid = FALSE;

	while((16 > (++count)) &&
		(*lpszSearch)) {

		if((' ' == *lpszSearch) || ('\t' == *lpszSearch)) {
			isValid = TRUE;
			break;
		}
		*lpszOperator++ = *lpszSearch++;
	}

	if(!isValid)
		return FALSE;

	//解析szOperator
	const wchar_t *lpszOpPos = wcsstr(szOperators, szOperator);
	if(NULL == lpszOpPos)
		return FALSE;

	pFileOp->op = SCRIPTOP((lpszOpPos - szOperators) / 8);

	//过滤掉剩下的空格和tab
	while((' ' == *lpszSearch) || ('\t' == *lpszSearch))
		lpszSearch++;

	isValid = FALSE;
	count = 0;

	if((MAX_PATH <= wcslen(lpszSearch)) || (0 == *lpszSearch))
		return FALSE;

	wcscpy(pFileOp->szFilename, lpszSearch);

	////检查文件名是否正确
	//if(OP_CheckFile == pFileOp->op) {

	//	if(0 == _tcsicmp(lpszFileName, pFileOp->szFilename))

	//		return TRUE;
	//	else
	//		return FALSE;
	//}

	return TRUE;
}

//分解脚本中的目录
void CPckClassRebuildFilter::SepratePaths(FILEOP * pFileOp)
{
	wcscpy(pFileOp->szFilenameBuffer, pFileOp->szFilename);

	pFileOp->lpszSepratedPaths[0] = pFileOp->szFilenameBuffer;

	wchar_t *lpszSearch = pFileOp->szFilenameBuffer;
	int nPathDepthCount = 1;


	while(*lpszSearch) {

		wchar_t *test = wcschr(lpszSearch, L'\\');
		//这里没有考虑存在\\双斜杠的情况
		if((L'\\' == *lpszSearch) || (L'/' == *lpszSearch)) {
			*lpszSearch = 0;
			++lpszSearch;
			pFileOp->lpszSepratedPaths[nPathDepthCount] = lpszSearch;
			++nPathDepthCount;
		}
		++lpszSearch;
	}

}

LPPCK_PATH_NODE CPckClassRebuildFilter::LocationFileIndex(LPWSTR *lpszPaths, LPPCK_PATH_NODE lpNode)
{
	LPCWSTR lpszSearchDir = *lpszPaths;

	if((NULL == lpszSearchDir) || (NULL == lpNode))
		return NULL;

	//直接跳过..目录
	LPPCK_PATH_NODE lpNodeSearch = lpNode->next;

	while(1) {

		if(NULL == lpNodeSearch)
			return NULL;

		if(0 == wcsicmp(lpszSearchDir, lpNodeSearch->szName)) {

			//是否已经匹配完
			if(NULL == *(lpszPaths + 1)) {

				return lpNodeSearch;
			} else {
				return LocationFileIndex(lpszPaths + 1, lpNodeSearch->child);
			}
		}
		lpNodeSearch = lpNodeSearch->next;
	}

	return NULL;
}

void CPckClassRebuildFilter::MarkFilterFlagToFileIndex(LPPCKINDEXTABLE	lpPckIndexTable, SCRIPTOP op)
{
	switch(op) {

	case OP_Protect:
		lpPckIndexTable->isProtected = TRUE;
		break;

	case OP_Delete:
	case OP_Rmdir:
		if((!lpPckIndexTable->isInvalid) && (!lpPckIndexTable->isProtected)) {

			lpPckIndexTable->isToDeDelete = TRUE;
			lpPckIndexTable->isInvalid = TRUE;
		}
		break;
	}
	m_EditedNode.push_back(lpPckIndexTable);
}

//将一行脚本内容应用到查找到的文件列表中
void CPckClassRebuildFilter::MarkFilterFlagToNode(LPPCK_PATH_NODE lpNode, SCRIPTOP op)
{
	lpNode = lpNode->child->next;

	while(NULL != lpNode) {
		//if(NULL == lpNode->child) {
		if (PCK_ENTRY_TYPE_FOLDER & lpNode->entryType) {
			MarkFilterFlagToNode(lpNode, op);
		} else {
			MarkFilterFlagToFileIndex(lpNode->lpPckIndexTable, op);
		}

		lpNode = lpNode->next;
	}

}


#pragma region ApplyScript2IndexList,将脚本内容应用到文件列表中

//将脚本内容应用到文件列表中
BOOL CPckClassRebuildFilter::ApplyScript2IndexList(LPPCK_PATH_NODE lpRootNode)
{
	//解析过程是否发生了错误
	BOOL bHasErrorHappend = FALSE;
	m_EditedNode.clear();

	for(size_t i = 0;i< m_FirstFileOp.size();i++){

		FILEOP * pFileOp = &m_FirstFileOp[i];

		if(OP_CheckFile != pFileOp->op) {

			//分解脚本中的目录
			SepratePaths(pFileOp);

			//定位文件索引
			LPPCK_PATH_NODE lpFoundNode = LocationFileIndex(pFileOp->lpszSepratedPaths, lpRootNode->child);

			if(NULL == lpFoundNode) {

				Logger.w(UCSTEXT("已解析脚本失败在: %s, 跳过..."), pFileOp->szFilename);
				bHasErrorHappend = TRUE;

			} else {
				if(PCK_ENTRY_TYPE_FOLDER & lpFoundNode->entryType){
				//if(NULL != lpFoundNode->child) {

					MarkFilterFlagToNode(lpFoundNode, pFileOp->op);
				} else {
					MarkFilterFlagToFileIndex(lpFoundNode->lpPckIndexTable, pFileOp->op);
				}
			}

		}
	}

	return (!bHasErrorHappend);
}

#pragma endregion

BOOL CPckClassRebuildFilter::ParseScript(const wchar_t * lpszScriptFile)
{

	if (!OpenScriptFileAndConvBufToUcs2(lpszScriptFile)) {
		Logger.w("读取脚本失败");
		return FALSE; 
	}

	m_FirstFileOp.push_back(FILEOP{ 0 });
	FILEOP * pFileOp = &m_FirstFileOp.back();

	for (int i = 0; i < m_ScriptLines.size(); i++) {

		//过滤注释行
		if (L';' != m_ScriptLines[i].at(0)) {
			//一行脚本分为两部分，操作和文件名
			if (ParseOneLine(pFileOp, m_ScriptLines[i].c_str())) {

				m_FirstFileOp.push_back(FILEOP{ 0 });
				pFileOp = &m_FirstFileOp.back();

			}
			else {

				Logger.w("脚本解析失败在行%d: %ls, 跳过...", i, m_ScriptLines[i].c_str());

				return FALSE;
			}
		}
	}

	Logger.i("解析脚本成功");
	return TRUE;
}

//清除掉重建包时所需要读取的过滤信息
void CPckClassRebuildFilter::ResetRebuildFilterInIndexList()
{
	for(DWORD i = 0;i < m_EditedNode.size();++i) {

		LPPCKINDEXTABLE lpPckIndexTable = m_EditedNode[i];

		if(lpPckIndexTable->isToDeDelete) {
			lpPckIndexTable->isInvalid = FALSE;
		}
		lpPckIndexTable->isProtected = lpPckIndexTable->isToDeDelete = FALSE;
	}
}

//应用脚本内容
BOOL CPckClassRebuildFilter::Apply(LPPCK_PATH_NODE lpRootNode)
{
	BOOL rtn = FALSE;

	//将数据应用于tree中
	rtn = ApplyScript2IndexList(lpRootNode);

	if (!rtn) {
		ResetRebuildFilterInIndexList();
		Logger.i("应用脚本失败");
	}
	else {
		Logger.i("应用脚本成功");
	}

	return rtn;
}

BOOL CPckClassRebuildFilter::ApplyScript(const wchar_t * lpszScriptFile, LPPCK_PATH_NODE lpRootNode)
{
	if (!ParseScript(lpszScriptFile))
		return FALSE;

	return Apply(lpRootNode);
}

BOOL CPckClassRebuildFilter::TestScript(const wchar_t * lpszScriptFile)
{
	return ParseScript(lpszScriptFile);
}


BOOL CPckClassRebuildFilter::ModelTextureCheck(LPCWSTR lpszFilename)
{
	//路径规则，*\textures\*.dds

	LPCWSTR constTexturePath = L"\\textures\\";
	LPCWSTR constDdsExt = L".dds";
	LPCWSTR constTgaExt = L".tga";

	LPCWSTR lpszTexturePath = wcsstr(lpszFilename, constTexturePath);

	if (nullptr == lpszTexturePath)
		return FALSE;

	lpszTexturePath += wcslen(constTexturePath);

	LPCWSTR subdir = wcschr(lpszTexturePath, L'\\');
	if (nullptr != subdir) {
		if (nullptr != wcschr(subdir+1, L'\\'))
			return FALSE;
	}

	lpszTexturePath += wcslen(lpszTexturePath) - wcslen(constDdsExt);
	if (0 == wcsicmp(lpszTexturePath, constDdsExt))
		return TRUE;

	if (0 == wcsicmp(lpszTexturePath, constTgaExt))
		return TRUE;

	return FALSE;
}

void CPckClassRebuildFilter::StripModelTexture(LPPCKINDEXTABLE lpPckIndexHead, DWORD dwFileCount, LPPCK_PATH_NODE lpRootNode, LPCWSTR lpszPckFilename)
{

	int nDetectOffset = 0;
#if 0
	//检测pck目录中的文件夹是否以pck文件名开头
	//如gfx.pck中根目录为gfx且只有一个
	LPPCK_PATH_NODE lpRootNodeFirstDir = lpRootNode->next;
	int nRootDirCount = 0;
	vector<wstring> sRootDirs;

	while (NULL != lpRootNodeFirstDir) {

		if (PCK_ENTRY_TYPE_FOLDER & lpRootNodeFirstDir->entryType) {

			sRootDirs.push_back(lpRootNodeFirstDir->szName);
			++nRootDirCount;
		}
		lpRootNodeFirstDir = lpRootNodeFirstDir->next;
	}

	//只有3个不到的根目录，取根目录名,对比文件名
	if (3 > nRootDirCount) {
		wchar_t szFileTitle[MAX_PATH];
		wchar_t* lpszExt = nullptr;

		wcscpy_s(szFileTitle, lpszPckFilename);

		if (nullptr != (lpszExt = wcsrchr(szFileTitle, L'.'))) {

			*lpszExt = 0;
		}

		//对比
		for (int i = 0; i < sRootDirs.size(); i++) {

			if (nullptr != wcsstr(szFileTitle, sRootDirs[i].c_str())) {

				nDetectOffset = sRootDirs[i].length() + 1;
				break;
			}
		}
	}
#endif

	LPPCKINDEXTABLE lpPckIndexTable = lpPckIndexHead;

	for (DWORD i = 0; i < dwFileCount; i++) {

		if (ModelTextureCheck(lpPckIndexTable->cFileIndex.szwFilename + nDetectOffset)) {
			if (!lpPckIndexTable->isInvalid) {

				lpPckIndexTable->isToDeDelete = TRUE;
				lpPckIndexTable->isInvalid = TRUE;

				m_EditedNode.push_back(lpPckIndexTable);
			}
		}
		lpPckIndexTable++;
	}
}