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

//检测pck的版本并写入到iDetectedPckID，同时读取文件头和尾
BOOL	DetectPckVerion(LPCTSTR lpszPckFile, LPPCK_ALL_INFOS pckAllInfo);
//void	PrintInvalidVersionDebugInfo();

TCHAR		szSaveDlgFilterString[1024];

const static PCK_KEYS			cPckKeys[PCK_VERSION_NUMS];
const static PCK_VERSION_FUNC	cPckVersionFunc[PCK_VERSION_NUMS];

void	BuildSaveDlgFilterString();

//文件头、尾的数据填写和数据写入
static LPVOID	FillHeadData_V2020(void *param);
static LPVOID	FillHeadData_V2030(void *param);

static LPVOID	FillTailData_V2020(void *param);
static LPVOID	FillTailData_V2030(void *param);
static LPVOID	FillTailData_V2031(void *param);

static LPVOID FillIndexData_V2020(void *param, void *pckFileIndexBuf);
static LPVOID FillIndexData_V2030(void *param, void *pckFileIndexBuf);
static LPVOID FillIndexData_V2031(void *param, void *pckFileIndexBuf);

//数据从lpIndex -> param
static BOOL PickIndexData_V2020(void *param, void* lpIndex);
static BOOL PickIndexData_V2030(void *param, void* lpIndex);
static BOOL PickIndexData_V2031(void *param, void* lpIndex);