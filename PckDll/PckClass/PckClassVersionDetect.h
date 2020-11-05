//////////////////////////////////////////////////////////////////////
// PckClassVersionDetect.h: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 版本相关的检测以及数据的写入、读取
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2017.6.27
//////////////////////////////////////////////////////////////////////
#pragma once
#include "PckClassBaseFeatures.h"

//PckVersion
#ifndef _DEBUG

#define PCK_VERSION_START_ID		0
#define	PCK_VERSION_ZX				0
#define	PCK_VERSION_ZXNEW			1
#define	PCK_VERSION_SDS				2
#define	PCK_VERSION_SDSNEW			3
#define PCK_VERSION_SM				4
#define PCK_VERSION_SMNEW			5
#define PCK_VERSION_KDXY			6
#define PCK_VERSION_KDXYNEW			7
#define PCK_VERSION_RWPD			8
#define PCK_VERSION_RWPDNEW			9
#define PCK_VERSION_END_ID			9
#define PCK_VERSION_NORMAL_NUMS		10
#define PCK_VERSION_SP_START_ID		10
#define	PCK_VERSION_XAJH			10
#define PCK_VERSION_SP_END_ID		10

#else

#define PCK_VERSION_START_ID		0
#define	PCK_VERSION_ZX				0
#define	PCK_VERSION_ZXNEW			1
#define	PCK_VERSION_SDS				2
#define	PCK_VERSION_SDSNEW			3
#define PCK_VERSION_SM				4
#define PCK_VERSION_SMNEW			5
#define PCK_VERSION_END_ID			5
#define PCK_VERSION_NORMAL_NUMS		6
#define PCK_VERSION_SP_START_ID		6
#define	PCK_VERSION_XAJH			6
#define PCK_VERSION_SP_END_ID		6

#endif

#define PCK_VERSION_INVALID		(-1)

#define MAX_SEARCH_DEPTH		10000

#define MAX_FILE_PACKAGE		0x7fffff00U

#define AFPCK_VERSION_202		0x00020002
#define AFPCK_VERSION_203		0x00020003

#define AFPCK_SAFEHEAFER_TAG1	0x4DCA23EF
#define AFPCK_SAFEHEAFER_TAG2	0x56A089B7

typedef PCKHEAD_V2030 PCKHEAD_VXAJH, *LPPCKHEAD_VXAJH;


static std::vector<PCK_VERSION_FUNC>	cPckVersionFunc;

typedef struct _PCK_VERSION_ID
{
	wchar_t			name[64];
	PCK_CATEGORY	VersionId;
	uint32_t		Version;
	uint32_t		AlgorithmId;
}PCK_VERSION_ID;

typedef struct _PCK_SP_VERSION_ID
{
	wchar_t			name[64];
	uint32_t		Version;
	uint32_t		AlgorithmId;
	uint32_t		SafeHeaderTag1 = AFPCK_SAFEHEAFER_TAG1;
	uint32_t		SafeHeaderTag2 = 0xffffffff;
	uint64_t		MaskDword = 0xffffffff;
	uint32_t		MaxPackageSize = MAX_FILE_PACKAGE;
}PCK_SP_VERSION_ID;

class CPckClassVersionDetect :
	private virtual CPckClassBaseFeatures
{
public:
	CPckClassVersionDetect();
	~CPckClassVersionDetect();

	//设置版本
	const	PCK_KEYS*	GetPckVersion();
	BOOL	SetSavePckVersion(int verID);
	static	const	wchar_t*	GetPckVersionNameById(int id);
	static	uint32_t	GetPckVersionCount();

	static	int		AddPckVersion(int AlgorithmId, int Version);

protected:
	//检测pck的版本并写入到iDetectedPckID，同时读取文件头和尾
	BOOL	DetectPckVerion(LPCWSTR lpszPckFile);

private:

	static const std::vector <PCK_VERSION_ID>		cPckIDs;
	static const std::map <PCK_CATEGORY, PCK_SP_VERSION_ID>	cPckSPIDs;
	static const std::vector<PCK_KEYS>				cPckSPKeys;
	static const std::vector<PCK_VERSION_FUNC>		cPckSPVersionFunc;

	//填充版本信息
	static void		FillGeneralVersionInfo();
	static void		FillSpecialVersionInfo();
	static int		FillUnknownVersionInfo(DWORD AlgorithmId, DWORD Version);

	//PCK版本判断
	static void		SetAlgorithmId(DWORD id, PCK_VERSION_FUNC* lpPckVersionFunc);
	static void		SetDataFmtFunc(PCK_VERSION_FUNC* lpPckVersionFunc);
	void	PrintInvalidVersionDebugInfo(const wchar_t * lpszPckFile);

};

class detectversion_error : public std::exception { // base of all generic_error exceptions
public:
	explicit detectversion_error(const char* prefix, const std::string& _Message) { buf.assign(prefix); buf.append(_Message); }
	explicit detectversion_error(const char* prefix, const char* _Message) { buf.assign(prefix); buf.append(_Message); }
	explicit detectversion_error(const std::string& _Message) { buf = _Message; }
	explicit detectversion_error(const char* _Message) { buf.assign(_Message); }

	virtual char const* what() const { return buf.c_str(); };

protected:
	std::string buf;
};
