//////////////////////////////////////////////////////////////////////
// ZupHeader.h: 用于解析完美世界公司的zup文件中的数据，并显示在List中
// 头文件
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.5.23
//////////////////////////////////////////////////////////////////////

#if !defined(_ZUPHEADER_H_)
#define _ZUPHEADER_H_

//#define	TEXT_SORRY			"sorry"
#define	TEXT_NOTSUPPORT		"暂时不支持此功能"

typedef struct _ZUP_INC {
	unsigned short		wID;
	char				szMD5[34];
	unsigned char		abMD5[16];

	
	_ZUP_INC			*next;
}ZUPINC, *LPZUPINC;

typedef struct _ZUP_INC_LIST {

	unsigned short		wID;
	unsigned short		wMinVersion;
	unsigned short		wTargetVersion;
	unsigned long		dwTotalSize;
	char				szSingature[192];
	unsigned char		abSingature[128];
	LPZUPINC			lpZupIncFileList;

	_ZUP_INC_LIST		*next;

}ZUPINCLIST, *LPZUPINCLIST;



#endif