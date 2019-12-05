//////////////////////////////////////////////////////////////////////
// ZupClass.h: 用于解析完美世界公司的zup文件中的数据，并显示在List中
// 头文件
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.5.23
//////////////////////////////////////////////////////////////////////

#include "PckClass.h"
#include "ZupHeader.h"
#include "DictHash.h"
#include "PckClassLog.h"

#if !defined(_ZUPCLASS_H_)
#define _ZUPCLASS_H_

class CZupClass : public CPckClass
{
public:

	//CZupClass();
	CZupClass(LPPCK_RUNTIME_PARAMS inout);
	virtual ~CZupClass();

	BOOL	Init(LPCWSTR	szFile) override;

	const	LPPCKINDEXTABLE		GetPckIndexTable() override;

	//重建pck文件
	virtual BOOL	RebuildPckFile(LPCWSTR lpszScriptFile, LPCWSTR szRebuildPckFile, BOOL bUseRecompress) override { Logger.e(TEXT_NOTSUPPORT);return FALSE; }
	virtual BOOL	StripPck(LPCWSTR lpszStripedPckFile, int flag) override { Logger.e(TEXT_NOTSUPPORT); return FALSE; }

	//新建、更新pck文件
	virtual BOOL	UpdatePckFile(LPCWSTR szPckFile, const vector<wstring> &lpszFilePath, const PCK_PATH_NODE* lpNodeToInsert) override { Logger.e(TEXT_NOTSUPPORT);return FALSE; }

	//重命名文件
	virtual BOOL	RenameFilename() override { Logger.e(TEXT_NOTSUPPORT);return FALSE; }

	//删除一个节点
	virtual void	DeleteNode(LPPCK_PATH_NODE lpNode) override { Logger.e(TEXT_NOTSUPPORT);}
	virtual void	DeleteNode(LPPCKINDEXTABLE lpIndex) override { Logger.e(TEXT_NOTSUPPORT); }

	//重命名一个节点
	virtual	BOOL	RenameNode(LPPCK_PATH_NODE lpNode, const wchar_t* lpszReplaceString) override { Logger.e(TEXT_NOTSUPPORT); return FALSE; }
	virtual	BOOL	RenameIndex(LPPCK_PATH_NODE lpNode, const wchar_t* lpszReplaceString) override { Logger.e(TEXT_NOTSUPPORT); return FALSE; }
	virtual	BOOL	RenameIndex(LPPCKINDEXTABLE lpIndex, const wchar_t* lpszReplaceString) override { Logger.e(TEXT_NOTSUPPORT); return FALSE; }

	//预览文件
	virtual BOOL	GetSingleFileData(const PCKINDEXTABLE* const lpZupFileIndexTable, char *buffer, size_t sizeOfBuffer = 0) override;
protected:
	virtual BOOL	GetSingleFileData(LPVOID lpvoidFileRead, const PCKINDEXTABLE* const lpPckFileIndexTable, char *buffer, size_t sizeOfBuffer = 0) override;

private:

	LPPCKINDEXTABLE				m_lpZupIndexTable;
	CDictHash					*m_lpDictHash;

private:

	void	BuildDirTree();
	BOOL	BuildZupBaseDict();
	void	DecodeFilename(char *_dst, wchar_t *_wdst, char *_src);

	_inline void	DecodeDict(LPZUP_FILENAME_DICT lpZupDict);
	//void	AddDict(char *&lpszStringToAdd);
	void	AddDict(std::string& base_file);

	const PCKINDEXTABLE* GetBaseFileIndex(const PCKINDEXTABLE* lpIndex, const PCKINDEXTABLE* lpZeroBaseIndex);

};

#endif