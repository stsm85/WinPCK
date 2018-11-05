//////////////////////////////////////////////////////////////////////
// PckClassVersionDetect.cpp: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 版本相关的检测以及数据的写入、读取
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2017.6.27
//////////////////////////////////////////////////////////////////////

#include "MapViewFileMultiPck.h"
#include "PckClassVersionDetect.h"
#include "PckAlgorithmId.h"

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4244 )
#pragma warning ( disable : 4146 )

#include "PckClassVersionDetectFunctionDefinitions.h"
#include "Raw2HexString.h"

/*
 **	IndexesEntryAddressCryptKey >= 0x80000000 ->0xffffffff80000000
 ** else
 **	0x7fffffff -> 0x000000007fffffff
 ** 0xA8937462 -> 0xffffffffA8937462
 **
*/

/*
诛仙 v202版本文件构造
		   00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
00000000h: EF 23 CA 4D 57 01 00 00 B7 89 A0 56 31 37 32 39 ; ?蔒W...穳燰1729
00000010h: 20 30 0D 0A 41 74 93 A8 70 36 A4 F1 78 DA 4B 2F ;  0..At摠p6ゑx贙/
00000020h: 4A 2C 2E 4E 2D 8E 29 4B 2D 2A CE CC CF D3 2B 2E ; J,.N-?K-*翁嫌+.
00000030h: 67 18 71 80 07 88 39 A0 18 04 00 CB 3F 07 8F EE ; g.q�.??..?.忣
00000040h: FE FD FD 02 00 02 00 76 74 93 A8 00 00 00 00 41 ; ?...vt摠....A
00000050h: 6E 67 65 6C 69 63 61 20 46 69 6C 65 20 50 61 63 ; ngelica File Pac
00000060h: 6B 61 67 65 0D 0A 43 72 65 61 74 65 20 62 79 20 ; kage..Create by
00000070h: 57 69 6E 50 43 4B 00 00 00 00 00 00 00 00 00 00 ; WinPCK..........
00000080h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
00000090h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
000000a0h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
000000b0h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
000000c0h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
000000d0h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
000000e0h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
000000f0h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
00000100h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
00000110h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
00000120h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
00000130h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
00000140h: 00 00 00 00 00 00 00 00 00 00 00 EF BE 0D F0 01 ; ...........锞.?
00000150h: 00 00 00 02 00 02 00                            ; .......

最后280字节：0x3f~0x156
		   00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
00000030h: xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx EE ; g.q�.??..?.忣
00000040h: FE FD FD 02 00 02 00 76 74 93 A8 00 00 00 00 41 ; ?...vt摠....A
00000050h: 6E 67 65 6C 69 63 61 20 46 69 6C 65 20 50 61 63 ; ngelica File Pac
00000060h: 6B 61 67 65 0D 0A 43 72 65 61 74 65 20 62 79 20 ; kage..Create by
00000070h: 57 69 6E 50 43 4B 00 00 00 00 00 00 00 00 00 00 ; WinPCK..........
00000080h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
00000090h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
000000a0h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
000000b0h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
000000c0h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
000000d0h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
000000e0h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
000000f0h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
00000100h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
00000110h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
00000120h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
00000130h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
00000140h: 00 00 00 00 00 00 00 00 00 00 00 EF BE 0D F0 01 ; ...........锞.?
00000150h: 00 00 00 02 00 02 00                            ; .......

0xAAAAAAAA: 0x00~0x03
0xBBBBBBBB: 0x08~0x0b
0xCCCCCCCC: 0x3f~0x42	dwIndexTableCheckHead
0xEEEEEEEE: 计算存放文件名及文件数据的列表（数组）地址的开始位置，从最后280字节开头的第8个字节开始，找到0xa8937476, 与0xA8937462进行xor,->0x14，取出数据：

xxxxxxxxxx 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
00000010h: xx xx xx xx 41 74 93 A8 70 36 A4 F1 78 DA 4B 2F ;  0..At摠p6ゑx贙/
00000020h: 4A 2C 2E 4E 2D 8E 29 4B 2D 2A CE CC CF D3 2B 2E ; J,.N-?K-*翁嫌+.
00000030h: 67 18 71 80 07 88 39 A0 18 04 00 CB 3F 07 8F
将0x14~0x17的值 0xA8937441与0xGGGGGGGG（0xA8937462）计算->0x23=35，这个地址是这个文件压缩的索引的大小
将0x18~0x1b的值 0xf1a43670与0xHHHHHHHH（0xF1A43653）计算->0x23=35，这个地址是这个文件压缩的索引的大小

00000010h: xx xx xx xx xx xx xx xx xx xx xx xx 78 DA 4B 2F ;  0..At摠p6ゑx贙/
00000020h: 4A 2C 2E 4E 2D 8E 29 4B 2D 2A CE CC CF D3 2B 2E ; J,.N-?K-*翁嫌+.
00000030h: 67 18 71 80 07 88 39 A0 18 04 00 CB 3F 07 8F xx ; g.q�.??..?.忣

0xFFFFFFFF: 0x14b~0x14e

其中 78 DA ** ** ** ** ** ** 是zlib的校验头，通常以78 01(1), 78 5e(2-5),78 9c(6), 78 da(7-12)开头

*/

#pragma region CPckClassVersionDetect

CPckClassVersionDetect::CPckClassVersionDetect()
{
	if (cPckVersionFunc.empty()) {
		FillGeneralVersionInfo();
		FillSpecialVersionInfo();
	}
	BuildSaveDlgFilterString();
}

CPckClassVersionDetect::~CPckClassVersionDetect()
{}

const PCK_VERSION_ID CPckClassVersionDetect::cPckIDs[] = 
{
	{ PCK_VERSION_ZX, TEXT("诛仙"), PCK_V2020, 0x20002, 0 },
	{ PCK_VERSION_ZXNEW,	TEXT("诛仙(新)"),		PCK_V2030, 0x20003, 0 },
	{ PCK_VERSION_SDS,		TEXT("圣斗士"),			PCK_V2020, 0x20002, 161 },
	{ PCK_VERSION_SDSNEW,	TEXT("圣斗士(新)"),		PCK_V2030, 0x20003, 161 },
	{ PCK_VERSION_SM,		TEXT("神魔"),			PCK_V2020, 0x20002, 131 },
	{ PCK_VERSION_SMNEW,	TEXT("神魔(新)"),		PCK_V2030, 0x20003, 131 },
#ifndef _DEBUG
	{ PCK_VERSION_KDXY,		TEXT("口袋西游"),		PCK_V2020, 0x20002, 121 },
	{ PCK_VERSION_KDXYNEW,	TEXT("口袋西游(新)"),	PCK_V2030, 0x20003, 121 },
	{ PCK_VERSION_RWPD,		TEXT("热舞派对"),		PCK_V2020, 0x20002, 111 },
	{ PCK_VERSION_RWPDNEW,	TEXT("热舞派对(新)"),	PCK_V2030, 0x20003, 111 },
#endif
	PCK_VERSION_INVALID,
};

const PCK_KEYS CPckClassVersionDetect::cPckSPKeys[] =
/*		ID				名称				版本ID		版本值	0xAAAAAAAA	0xBBBBBBBB	0xCCCCCCCC	0xDDDDDDDDEEEEEEEE	0xFFFFFFFF	0xGGGGGGGG	0xHHHHHHHH	分块大小
		id				name				VersionId	Version	HeadVerifyKey1			TailVerifyKey1					TailVerifyKey2			IndexCompressedFilenameDataLengthCryptKey2
																			HeadVerifyKey2			IndexesEntryAddressCryptKey		IndexCompressedFilenameDataLengthCryptKey1*/
{ { PCK_VERSION_XAJH,	TEXT("笑傲江湖"),	PCK_VXAJH, 0x20003, 0x5edb34f0, 0x00000000, 0x7b2a7820, 0x49ab7f1d33c3eddb, 0xa75dc142, 0x62a4f9e1, 0x3520c3d5, 0xffffff00 },
};

#define PCK_VER_FUNC_LINE(_id, _head_ver, _tail_ver, _index_ver) \
{cPckSPKeys[(_id) - PCK_VERSION_SP_START_ID], sizeof(PCKHEAD_V##_head_ver), sizeof(PCKTAIL_V##_tail_ver), sizeof(PCKFILEINDEX_V##_index_ver), PickIndexData_V##_index_ver, FillHeadData_V##_head_ver, FillTailData_V##_tail_ver, FillIndexData_V##_index_ver}

const PCK_VERSION_FUNC CPckClassVersionDetect::cPckSPVersionFunc[] = 
{	PCK_VER_FUNC_LINE(PCK_VERSION_XAJH, 2030, XAJH, XAJH),
	PCK_VERSION_INVALID };


void CPckClassVersionDetect::FillGeneralVersionInfo()
{
	PCK_VERSION_FUNC cPckVersionFuncToAdd;
	LPPCK_KEYS lpPckKeys = &cPckVersionFuncToAdd.cPckXorKeys;
	const PCK_VERSION_ID *	lpPckIDs = cPckIDs;
	
	while (PCK_VERSION_INVALID != lpPckIDs->id) {
		memset(&cPckVersionFuncToAdd, 0, sizeof(PCK_VERSION_FUNC));
		lpPckKeys->id = lpPckIDs->id;
		_tcscpy(lpPckKeys->name, lpPckIDs->name);
		lpPckKeys->CategoryId = lpPckIDs->VersionId;
		lpPckKeys->Version = lpPckIDs->Version;

		SetAlgorithmId(lpPckIDs->AlgorithmId, &cPckVersionFuncToAdd);

		cPckVersionFunc.push_back(cPckVersionFuncToAdd);

		++lpPckIDs;
	}

}

void CPckClassVersionDetect::FillSpecialVersionInfo()
{
	PCK_VERSION_FUNC cPckVersionFuncToAdd;
	const PCK_VERSION_FUNC * lpPckVersionFuncSP = cPckSPVersionFunc;

	while(PCK_VERSION_INVALID != lpPckVersionFuncSP->cPckXorKeys.id){

		memcpy(&cPckVersionFuncToAdd, lpPckVersionFuncSP, sizeof(PCK_VERSION_FUNC));

		cPckVersionFunc.push_back(cPckVersionFuncToAdd);

		++lpPckVersionFuncSP;
	}
}

int CPckClassVersionDetect::FillUnknownVersionInfo(DWORD AlgorithmId, DWORD Version)
{
	if (PCK_VERSION_INVALID != AlgorithmId) {

		PCK_VERSION_FUNC cPckVersionFuncToAdd;

		LPPCK_KEYS lpUnknownPckKeys = &cPckVersionFuncToAdd.cPckXorKeys;
		LPPCK_VERSION_FUNC lpUnknownPckVersionFunc = &cPckVersionFuncToAdd;
		lpUnknownPckKeys->id = AlgorithmId;
		_stprintf_s(lpUnknownPckKeys->name, TEXT("识别的未知格式(ver=0x%x id=%d)"), Version, AlgorithmId);
		lpUnknownPckKeys->CategoryId = 0x20002 == Version ? PCK_V2020 : PCK_V2030;
		lpUnknownPckKeys->Version = Version;

		SetAlgorithmId(AlgorithmId, &cPckVersionFuncToAdd);
		cPckVersionFunc.push_back(cPckVersionFuncToAdd);

		return cPckVersionFunc.size() - 1;
	}
	return PCK_VERSION_INVALID;
}

void CPckClassVersionDetect::SetAlgorithmId(DWORD id, LPPCK_VERSION_FUNC lpPckVersionFunc)
{
	CPckAlgorithmId AlgorithmId(id);

	LPPCK_KEYS lpPckKey = &lpPckVersionFunc->cPckXorKeys;

	lpPckKey->HeadVerifyKey1 = 0x4DCA23EF;
	lpPckKey->TailVerifyKey1 = AlgorithmId.GetPckGuardByte0();
	lpPckKey->TailVerifyKey2 = AlgorithmId.GetPckGuardByte1();
	lpPckKey->IndexCompressedFilenameDataLengthCryptKey1 = AlgorithmId.GetPckMaskDword();
	lpPckKey->IndexCompressedFilenameDataLengthCryptKey2 = AlgorithmId.GetPckCheckMask() ^ AlgorithmId.GetPckMaskDword();
	lpPckKey->dwMaxSinglePckSize = 0x7fffff00;

	if (0x20002 == lpPckKey->Version) {

		lpPckKey->HeadVerifyKey2 = 0x56A089B7;
		lpPckKey->IndexesEntryAddressCryptKey = AlgorithmId.GetPckMaskDword();

		lpPckVersionFunc->dwHeadSize = sizeof(PCKHEAD_V2020);
		lpPckVersionFunc->dwTailSize = sizeof(PCKTAIL_V2020);
		lpPckVersionFunc->dwFileIndexSize = sizeof(PCKFILEINDEX_V2020);
		lpPckVersionFunc->PickIndexData = PickIndexData_V2020;
		lpPckVersionFunc->FillHeadData = FillHeadData_V2020;
		lpPckVersionFunc->FillTailData = FillTailData_V2020;
		lpPckVersionFunc->FillIndexData = FillIndexData_V2020;
	}
	else {
		//0x20003
		lpPckKey->HeadVerifyKey2 = 0;
		lpPckKey->IndexesEntryAddressCryptKey = (AlgorithmId.GetPckMaskDword() & 0x80000000) ? 0xffffffff00000000 | AlgorithmId.GetPckMaskDword() : AlgorithmId.GetPckMaskDword();

		lpPckVersionFunc->dwHeadSize = sizeof(PCKHEAD_V2020);
		lpPckVersionFunc->dwTailSize = sizeof(PCKTAIL_V2030);
		lpPckVersionFunc->dwFileIndexSize = sizeof(PCKFILEINDEX_V2030);
		lpPckVersionFunc->PickIndexData = PickIndexData_V2030;
		lpPckVersionFunc->FillHeadData = FillHeadData_V2030;
		lpPckVersionFunc->FillTailData = FillTailData_V2030;
		lpPckVersionFunc->FillIndexData = FillIndexData_V2030;

	}
}

//生成界面保存文件时的Filter文本
void CPckClassVersionDetect::BuildSaveDlgFilterString()
{
	*szSaveDlgFilterString = 0;
	TCHAR szPrintf[256];

	for (int i = 0; i < cPckVersionFunc.size(); i++) {

		_stprintf_s(szPrintf, TEXT("%sPCK文件(*.pck)|*.pck|"), cPckVersionFunc[i].cPckXorKeys.name);
		_tcscat_s(szSaveDlgFilterString, szPrintf);

	}

	TCHAR *lpszStr = szSaveDlgFilterString;
	while (*lpszStr) {

		if (TEXT('|') == *lpszStr)
			*lpszStr = 0;
		++lpszStr;
	}

	*lpszStr = 0;
}


LPCTSTR CPckClassVersionDetect::GetSaveDlgFilterString()
{
	return szSaveDlgFilterString;
}

const PCK_KEYS* CPckClassVersionDetect::GetPckVersion()
{
	return &m_PckAllInfo.lpDetectedPckVerFunc->cPckXorKeys;
}


void CPckClassVersionDetect::SetSavePckVersion(int verID)
{
	if (0 <= verID && cPckVersionFunc.size() > verID) {
		m_PckAllInfo.lpSaveAsPckVerFunc = &cPckVersionFunc[verID];
	}
	else
		m_PckLog.PrintLogW(TEXT_INVALID_VERSION);
}


#define PRINT_HEAD_SIZE		0x20
#define PRINT_TAIL_SIZE		0x580

void CPckClassVersionDetect::PrintInvalidVersionDebugInfo(LPCTSTR lpszPckFile)
{
	//打印详细原因：
	//hex 一行长度89 数据一共402行，大小0x8BC2
	char szPrintf[8192];

	BYTE buf[PRINT_HEAD_SIZE + PRINT_TAIL_SIZE + 0x10];

	//读取文件头
	CMapViewFileMultiPckRead *lpRead = new CMapViewFileMultiPckRead();

	if (!lpRead->OpenPck(lpszPckFile)) {
		m_PckLog.PrintLogEL(TEXT_OPENNAME_FAIL, lpszPckFile, __FILE__, __FUNCTION__, __LINE__);
		goto dect_err;
	}

	if (lpRead->GetFileSize() <= (PRINT_TAIL_SIZE + PRINT_HEAD_SIZE)) {

		if (!lpRead->Read(buf, lpRead->GetFileSize())) {
			m_PckLog.PrintLogEL(TEXT_READFILE_FAIL, __FILE__, __FUNCTION__, __LINE__);
			goto dect_err;
		}

		CRaw2HexString cHexStr(buf, lpRead->GetFileSize());

		sprintf_s(szPrintf, "文件信息：\n文件大小：%lld\n文件概要数据：\n", lpRead->GetFileSize());
		strcat_s(szPrintf, cHexStr.GetHexString());


	}
	else {

		if (!lpRead->Read(buf, PRINT_HEAD_SIZE)) {
			m_PckLog.PrintLogEL(TEXT_READFILE_FAIL, __FILE__, __FUNCTION__, __LINE__);
			goto dect_err;
		}

		QWORD qwWhereToMove = (lpRead->GetFileSize() - PRINT_TAIL_SIZE) & 0xfffffffffffffff0;
		QWORD qwBytesToRead = lpRead->GetFileSize() - qwWhereToMove;

		lpRead->SetFilePointer(qwWhereToMove, FILE_BEGIN);

		if (!lpRead->Read(buf + PRINT_HEAD_SIZE, qwBytesToRead)) {
			m_PckLog.PrintLogEL(TEXT_READFILE_FAIL, __FILE__, __FUNCTION__, __LINE__);
			goto dect_err;
		}

		CRaw2HexString cHexStrHead(buf, PRINT_HEAD_SIZE);
		CRaw2HexString cHexStrTail(buf + PRINT_HEAD_SIZE, qwBytesToRead, qwWhereToMove);

		sprintf_s(szPrintf, "文件信息：\n文件大小：%lld\n文件概要数据：\n", lpRead->GetFileSize());
#if _DEBUG
		size_t len1 = strlen(cHexStrHead.GetHexString());
		size_t len2 = strlen(cHexStrTail.GetHexString());
		size_t lens = len1 + len2 + strlen(szPrintf) + strlen("......\n");
#endif
		strcat_s(szPrintf, cHexStrHead.GetHexString());
		strcat_s(szPrintf, "......\n");
		strcat_s(szPrintf, cHexStrTail.GetHexString());
	}

	m_PckLog.PrintLogD(szPrintf);
dect_err:
	delete lpRead;

}
#undef PRINT_HEAD_SIZE
#undef PRINT_TAIL_SIZE

//读取文件头和尾确定pck文件版本，返回版本ID
BOOL CPckClassVersionDetect::DetectPckVerion(LPCTSTR lpszPckFile)
{
	PCKHEAD_V2020 cPckHead;
	DWORD		dwTailVals[4];
	size_t		dwVerionDataCount = cPckVersionFunc.size();

	int iDetectedPckID = PCK_VERSION_INVALID;
	//读取文件头
	CMapViewFileMultiPckRead cRead;

	if (!cRead.OpenPck(lpszPckFile)) {
		m_PckLog.PrintLogEL(TEXT_OPENNAME_FAIL, lpszPckFile, __FILE__, __FUNCTION__, __LINE__);
		goto dect_err;
	}

	if (!cRead.Read(&cPckHead, sizeof(PCKHEAD_V2020))) {
		m_PckLog.PrintLogEL(TEXT_READFILE_FAIL, __FILE__, __FUNCTION__, __LINE__);
		goto dect_err;
	}

	//判断是不是64位的文件大小
	if (!cRead.SetPckPackSize((0x100 < cPckHead.dwHeadCheckTail) ? cPckHead.dwPckSize : ((PCKHEAD_V2030*)&cPckHead)->dwPckSize)) {

		m_PckLog.PrintLogEL(TEXT_PCK_SIZE_INVALID, __FILE__, __FUNCTION__, __LINE__);
		goto dect_err;
	}

	cRead.SetFilePointer(-((QWORD)(sizeof(DWORD) * 4)), FILE_END);

	if (!cRead.Read(&dwTailVals, sizeof(DWORD) * 4)) {
		m_PckLog.PrintLogEL(TEXT_READFILE_FAIL, __FILE__, __FUNCTION__, __LINE__);
		goto dect_err;
	}

	//dwTailVals[3] 为文件末尾4字节，一般存放版本
	if (0x20003 == dwTailVals[3]) {
		if (0 != dwTailVals[0])
			dwTailVals[1] = dwTailVals[0];

		for (int i = 0; i < dwVerionDataCount; i++) {
			if ((cPckVersionFunc[i].cPckXorKeys.Version == 0x20003) &&
				(cPckVersionFunc[i].cPckXorKeys.TailVerifyKey2 == dwTailVals[1]) &&
				(cPckVersionFunc[i].cPckXorKeys.HeadVerifyKey1 == cPckHead.dwHeadCheckHead)) {

				iDetectedPckID = i;
				break;
			}
		}
	}
	else {

		//当版本为202时，TailVerifyKey2的值可能为HeadVerifyKey2 或者 0，此例已在游戏：神魔的pck文件上出现。
		for (int i = 0; i < dwVerionDataCount; i++) {
			if ((cPckVersionFunc[i].cPckXorKeys.Version == dwTailVals[3]) &&
				(cPckVersionFunc[i].cPckXorKeys.TailVerifyKey2 == dwTailVals[1]) &&
				(cPckVersionFunc[i].cPckXorKeys.HeadVerifyKey1 == cPckHead.dwHeadCheckHead) &&
				((cPckVersionFunc[i].cPckXorKeys.HeadVerifyKey2 == cPckHead.dwHeadCheckTail) || (0 == cPckHead.dwHeadCheckTail))) {

				iDetectedPckID = i;
				break;
			}
		}
	}

	//已遍历所有格式，开始识别是否标准pck格式
	if (PCK_VERSION_INVALID == iDetectedPckID) {

		if (0x4DCA23EF == cPckHead.dwHeadCheckHead) {
			//验证未知格式，暂时默认遍历到10000
			for (int i = 0; i < 10000; i++) {

				CPckAlgorithmId AlgorithmId(i);

				if (AlgorithmId.GetPckGuardByte1() == dwTailVals[1]) {

					iDetectedPckID = FillUnknownVersionInfo(i, dwTailVals[3]);;
					//重建对话框pck列表
					BuildSaveDlgFilterString();
					break;
				}

			}
		}
	}

	if (PCK_VERSION_INVALID == iDetectedPckID) {

		m_PckLog.PrintLogEL(TEXT_VERSION_NOT_FOUND, __FILE__, __FUNCTION__, __LINE__);
		goto dect_err;
	}

	PCK_CATEGORY ver = cPckVersionFunc[iDetectedPckID].cPckXorKeys.CategoryId;
	BOOL		isFoundVer = FALSE;


	switch (ver) {
	case PCK_V2020:

		define_get_pckAllInfo_by_version(2020, iDetectedPckID);
		break;

	case PCK_V2030:

		define_get_pckAllInfo_by_version(2030, iDetectedPckID);
		break;
	case PCK_VXAJH:

		define_get_pckAllInfo_by_version(XAJH, iDetectedPckID);
		break;
	}

	if (!isFoundVer) {
		m_PckLog.PrintLogEL(TEXT_VERSION_NOT_FOUND, __FILE__, __FUNCTION__, __LINE__);
		goto dect_err;
	}

	m_PckAllInfo.lpSaveAsPckVerFunc = m_PckAllInfo.lpDetectedPckVerFunc = &cPckVersionFunc[iDetectedPckID];

	return TRUE;

dect_err:
	m_PckAllInfo.lpSaveAsPckVerFunc = NULL;
	PrintInvalidVersionDebugInfo(lpszPckFile);
	return FALSE;
}

define_one_FillHeadData_by_version(2020);
define_one_FillHeadData_by_version(2030);

define_one_FillTailData_by_version(2020);
define_one_FillTailData_by_version(2030);
define_one_FillTailData_by_version(XAJH);

//数据从param -> lpTail
define_one_FillIndexData_by_version(2020);
define_one_FillIndexData_by_version(2030);
define_one_FillIndexData_by_version(XAJH);

//数据从lpIndex -> param
define_one_PickIndexData_by_version(2020);
define_one_PickIndexData_by_version(2030);
define_one_PickIndexData_by_version(XAJH);

#pragma endregion