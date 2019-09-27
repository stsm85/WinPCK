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

#include "Raw2HexString.h"

#pragma region 说明

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
#pragma endregion

#pragma region CPckClassVersionDetect

CPckClassVersionDetect::CPckClassVersionDetect()
{
	if (cPckVersionFunc.empty()) {
		FillGeneralVersionInfo();
		FillSpecialVersionInfo();
	}
}

CPckClassVersionDetect::~CPckClassVersionDetect()
{
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

const PCK_VERSION_ID CPckClassVersionDetect::cPckIDs[] = 
{
	{ PCK_VERSION_ZX,		TEXT("诛仙"),			PCK_V2020, AFPCK_VERSION_202, 0 },
	{ PCK_VERSION_ZXNEW,	TEXT("诛仙(新)"),		PCK_V2030, AFPCK_VERSION_203, 0 },
	{ PCK_VERSION_SDS,		TEXT("圣斗士"),			PCK_V2020, AFPCK_VERSION_202, 161 },
	{ PCK_VERSION_SDSNEW,	TEXT("圣斗士(新)"),		PCK_V2030, AFPCK_VERSION_203, 161 },
	{ PCK_VERSION_SM,		TEXT("神魔"),			PCK_V2020, AFPCK_VERSION_202, 131 },
	{ PCK_VERSION_SMNEW,	TEXT("神魔(新)"),		PCK_V2030, AFPCK_VERSION_203, 131 },
#ifndef _DEBUG
	{ PCK_VERSION_KDXY,		TEXT("口袋西游"),		PCK_V2020, AFPCK_VERSION_202, 121 },
	{ PCK_VERSION_KDXYNEW,	TEXT("口袋西游(新)"),	PCK_V2030, AFPCK_VERSION_203, 121 },
	{ PCK_VERSION_RWPD,		TEXT("热舞派对"),		PCK_V2020, AFPCK_VERSION_202, 111 },
	{ PCK_VERSION_RWPDNEW,	TEXT("热舞派对(新)"),	PCK_V2030, AFPCK_VERSION_203, 111 },
#endif
	PCK_VERSION_INVALID,
};

const PCK_KEYS CPckClassVersionDetect::cPckSPKeys[] =
/*		ID				名称				版本ID		版本值	0xAAAAAAAA	0xBBBBBBBB	0xCCCCCCCC	0xDDDDDDDDEEEEEEEE	0xFFFFFFFF	0xGGGGGGGG	0xHHHHHHHH	分块大小
		id				name				VersionId	Version	HeadVerifyKey1			TailVerifyKey1					TailVerifyKey2			IndexCompressedFilenameDataLengthCryptKey2
																			HeadVerifyKey2			IndexesEntryAddressCryptKey		IndexCompressedFilenameDataLengthCryptKey1*/
{ { PCK_VERSION_XAJH,	TEXT("笑傲江湖"),	PCK_VXAJH, AFPCK_VERSION_203, 0x5edb34f0, 0x00000000, 0x7b2a7820, 0x49ab7f1d33c3eddb, 0xa75dc142, 0x62a4f9e1, 0x3520c3d5, 0xffffff00 },
};

#define PCK_VER_FUNC_LINE(_id, _head_ver, _tail_ver, _index_ver) \
{cPckSPKeys[(_id) - PCK_VERSION_SP_START_ID], sizeof(PCKHEAD_V##_head_ver), sizeof(PCKTAIL_V##_tail_ver), sizeof(PCKFILEINDEX_V##_index_ver), PickIndexData_V##_index_ver, FillHeadData_V##_head_ver, FillTailData_V##_tail_ver, FillIndexData_V##_index_ver}

const PCK_VERSION_FUNC CPckClassVersionDetect::cPckSPVersionFunc[] = 
{	PCK_VER_FUNC_LINE(PCK_VERSION_XAJH, 2030, XAJH, XAJH),
	PCK_VERSION_INVALID };


template<typename T, typename X>
BOOL get_pckAllInfo_by_version(CMapViewFileMultiPckRead& cRead, T& pckTail, PCK_ALL_INFOS& pckAllInfo, X* pPckHead, int id, uint64_t& qwPckSizeInHeader)
{
	cRead.SetFilePointer(qwPckSizeInHeader - ((uint64_t)(sizeof(T))), FILE_BEGIN);
	if (!cRead.Read(&pckTail, sizeof(T))) {
		throw MyExceptionEx(TEXT_READFILE_FAIL);
	}
	if (cPckVersionFunc[id].cPckXorKeys.TailVerifyKey1 == pckTail.dwIndexTableCheckHead) {
		pckAllInfo.qwPckSize = pPckHead->dwPckSize;
		pckAllInfo.dwAddressOfFileEntry = pckTail.dwEntryOffset ^ cPckVersionFunc[id].cPckXorKeys.IndexesEntryAddressCryptKey;
		memcpy(pckAllInfo.szAdditionalInfo, pckTail.szAdditionalInfo, PCK_ADDITIONAL_INFO_SIZE);
		return TRUE;
	}
	return FALSE;
}

void CPckClassVersionDetect::FillGeneralVersionInfo()
{
	PCK_VERSION_FUNC cPckVersionFuncToAdd;
	LPPCK_KEYS lpPckKeys = &cPckVersionFuncToAdd.cPckXorKeys;
	const PCK_VERSION_ID *	lpPckIDs = cPckIDs;
	
	while (PCK_VERSION_INVALID != lpPckIDs->id) {
		memset(&cPckVersionFuncToAdd, 0, sizeof(PCK_VERSION_FUNC));
		lpPckKeys->id = lpPckIDs->id;
		wcscpy(lpPckKeys->name, lpPckIDs->name);
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
	if ((PCK_VERSION_INVALID != AlgorithmId) && ((AFPCK_VERSION_202 == Version) || (AFPCK_VERSION_203 == Version))) {

		PCK_VERSION_FUNC cPckVersionFuncToAdd;

		LPPCK_KEYS lpUnknownPckKeys = &cPckVersionFuncToAdd.cPckXorKeys;
		LPPCK_VERSION_FUNC lpUnknownPckVersionFunc = &cPckVersionFuncToAdd;
		lpUnknownPckKeys->id = cPckVersionFunc.size();
		swprintf_s(lpUnknownPckKeys->name, L"识别的未知格式(ver=0x%x id=%d)", Version, AlgorithmId);
		lpUnknownPckKeys->CategoryId = AFPCK_VERSION_202 == Version ? PCK_V2020 : PCK_V2030;
		lpUnknownPckKeys->Version = Version;

		SetAlgorithmId(AlgorithmId, &cPckVersionFuncToAdd);
		cPckVersionFunc.push_back(cPckVersionFuncToAdd);

		return cPckVersionFunc.size() - 1;
	}
	return PCK_VERSION_INVALID;
}

int	CPckClassVersionDetect::AddPckVersion(int AlgorithmId, int Version)
{
	return FillUnknownVersionInfo(AlgorithmId, Version);
}

void CPckClassVersionDetect::SetAlgorithmId(DWORD id, LPPCK_VERSION_FUNC lpPckVersionFunc)
{
	CPckAlgorithmId AlgorithmId(id);

	LPPCK_KEYS lpPckKey = &lpPckVersionFunc->cPckXorKeys;

	lpPckKey->HeadVerifyKey1 = AFPCK_SAFEHEAFER_TAG1;
	lpPckKey->TailVerifyKey1 = AlgorithmId.GetPckGuardByte0();
	lpPckKey->TailVerifyKey2 = AlgorithmId.GetPckGuardByte1();
	lpPckKey->IndexCompressedFilenameDataLengthCryptKey1 = AlgorithmId.GetPckMaskDword();
	lpPckKey->IndexCompressedFilenameDataLengthCryptKey2 = AlgorithmId.GetPckCheckMask() ^ AlgorithmId.GetPckMaskDword();
	lpPckKey->dwMaxSinglePckSize = MAX_FILE_PACKAGE;

	if (AFPCK_VERSION_202 == lpPckKey->Version) {

		lpPckKey->HeadVerifyKey2 = AFPCK_SAFEHEAFER_TAG2;
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

const PCK_KEYS* CPckClassVersionDetect::GetPckVersion()
{
	return &m_PckAllInfo.lpDetectedPckVerFunc->cPckXorKeys;
}


BOOL CPckClassVersionDetect::SetSavePckVersion(int verID)
{
	if (cPckVersionFunc.empty()) {
		FillGeneralVersionInfo();
		FillSpecialVersionInfo();
	}

	if (0 <= verID && cPckVersionFunc.size() > verID) {
		m_PckAllInfo.lpSaveAsPckVerFunc = &cPckVersionFunc[verID];
		return TRUE;
	}
	else
		Logger.w(TEXT_INVALID_VERSION);

	return FALSE;
}

const wchar_t* CPckClassVersionDetect::GetPckVersionNameById(int id)
{
	if ((id >= 0) && (cPckVersionFunc.size() > id))
		return cPckVersionFunc[id].cPckXorKeys.name;
	return L"";
}

uint32_t CPckClassVersionDetect::GetPckVersionCount()
{
	if (cPckVersionFunc.empty()) {
		FillGeneralVersionInfo();
		FillSpecialVersionInfo();
	}
	return cPckVersionFunc.size();
}


#define PRINT_HEAD_SIZE		0x20
#define PRINT_TAIL_SIZE		0x580

void CPckClassVersionDetect::PrintInvalidVersionDebugInfo(const wchar_t * lpszPckFile)
{
	//打印详细原因：
	//hex 一行长度89 数据一共402行，大小0x8BC2
	char szPrintf[8192];

	BYTE buf[PRINT_HEAD_SIZE + PRINT_TAIL_SIZE + 0x10];

	//读取文件头
	CMapViewFileMultiPckRead *lpRead = new CMapViewFileMultiPckRead();

	if (!lpRead->OpenPck(lpszPckFile)) {
		Logger_el(TEXT_OPENNAME_FAIL, lpszPckFile);
		goto dect_err;
	}

	if (lpRead->GetFileSize() <= (PRINT_TAIL_SIZE + PRINT_HEAD_SIZE)) {

		if (!lpRead->Read(buf, lpRead->GetFileSize())) {
			Logger_el(TEXT_READFILE_FAIL);
			goto dect_err;
		}

		CRaw2HexString cHexStr(buf, lpRead->GetFileSize());

		sprintf_s(szPrintf, "文件信息：\n文件大小：%lld\n文件概要数据：\n", lpRead->GetFileSize());
		strcat_s(szPrintf, cHexStr.GetHexString());


	}
	else {

		if (!lpRead->Read(buf, PRINT_HEAD_SIZE)) {
			Logger_el(TEXT_READFILE_FAIL);
			goto dect_err;
		}

		uint64_t qwWhereToMove = (lpRead->GetFileSize() - PRINT_TAIL_SIZE) & 0xfffffffffffffff0;
		uint64_t qwBytesToRead = lpRead->GetFileSize() - qwWhereToMove;

		lpRead->SetFilePointer(qwWhereToMove, FILE_BEGIN);

		if (!lpRead->Read(buf + PRINT_HEAD_SIZE, qwBytesToRead)) {
			Logger_el(TEXT_READFILE_FAIL);
			goto dect_err;
		}

		CRaw2HexString cHexStrHead(buf, PRINT_HEAD_SIZE);
		CRaw2HexString cHexStrTail(buf + PRINT_HEAD_SIZE, qwBytesToRead, qwWhereToMove);

		sprintf_s(szPrintf, "文件信息：\n文件大小：%lld\n文件概要数据：\n", lpRead->GetFileSize());
#if PCK_DEBUG_OUTPUT
		size_t len1 = strlen(cHexStrHead.GetHexString());
		size_t len2 = strlen(cHexStrTail.GetHexString());
		size_t lens = len1 + len2 + strlen(szPrintf) + strlen("......\n");
#endif
		strcat_s(szPrintf, cHexStrHead.GetHexString());
		strcat_s(szPrintf, "......\n");
		strcat_s(szPrintf, cHexStrTail.GetHexString());
	}

	Logger.d(szPrintf);
dect_err:
	delete lpRead;

}
#undef PRINT_HEAD_SIZE
#undef PRINT_TAIL_SIZE

//读取文件头和尾确定pck文件版本，返回版本ID
BOOL CPckClassVersionDetect::DetectPckVerion(LPCWSTR lpszPckFile)
{
	PCKHEAD_V2020 cPckHead;
	uint32_t	dwTailVals[4];
	size_t		dwVerionDataCount = cPckVersionFunc.size();

	int iDetectedPckID = PCK_VERSION_INVALID;
	//读取文件头
	CMapViewFileMultiPckRead cRead;

	if (!cRead.OpenPck(lpszPckFile)) {
		Logger_el(TEXT_OPENNAME_FAIL, lpszPckFile);
		goto dect_err;
	}

	if (!cRead.Read(&cPckHead, sizeof(PCKHEAD_V2020))) {
		Logger_el(TEXT_READFILE_FAIL);
		goto dect_err;
	}

	//判断是不是64位的文件大小，head中的文件大小是否和实际相符
	uint64_t qwPckSizeInHeader = (0x100 < cPckHead.dwHeadCheckTail) ? cPckHead.dwPckSize : ((PCKHEAD_V2030*)&cPckHead)->dwPckSize;

	if (qwPckSizeInHeader > cRead.GetFileSize())
		throw MyException("size in header is bigger than file size");

	cRead.SetFilePointer(qwPckSizeInHeader - ((uint64_t)(sizeof(uint32_t) * 4)), FILE_BEGIN);

	if (!cRead.Read(&dwTailVals, sizeof(uint32_t) * 4)) {
		Logger_el(TEXT_READFILE_FAIL);
		goto dect_err;
	}

	//dwTailVals[3] 为文件末尾4字节，一般存放版本
	if (AFPCK_VERSION_203 == dwTailVals[3]) {
		if (0 != dwTailVals[0])
			dwTailVals[1] = dwTailVals[0];

		for (int i = 0; i < dwVerionDataCount; i++) {
			if ((cPckVersionFunc[i].cPckXorKeys.Version == AFPCK_VERSION_203) &&
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

		if (AFPCK_SAFEHEAFER_TAG1 == cPckHead.dwHeadCheckHead) {
			//验证未知格式，暂时默认遍历到10000
			for (int i = 0; i < MAX_SEARCH_DEPTH; i++) {

				CPckAlgorithmId AlgorithmId(i);

				if (AlgorithmId.GetPckGuardByte1() == dwTailVals[1]) {

					iDetectedPckID = FillUnknownVersionInfo(i, dwTailVals[3]);
					////重建对话框pck列表
					//BuildSaveDlgFilterString();
					break;
				}

			}
		}
	}

	if (PCK_VERSION_INVALID == iDetectedPckID) {

		Logger_el(TEXT_VERSION_NOT_FOUND);
		goto dect_err;
	}

	PCK_CATEGORY ver = cPckVersionFunc[iDetectedPckID].cPckXorKeys.CategoryId;
	BOOL		isFoundVer = FALSE;

	switch (ver) {
	case PCK_V2020:

		try {
			PCKTAIL_V2020 PckTail;
			isFoundVer = get_pckAllInfo_by_version(cRead, PckTail, m_PckAllInfo, &cPckHead, iDetectedPckID, qwPckSizeInHeader);
		}
		catch (MyException ex) {
			Logger.e(ex.what());
			goto dect_err;
		}
		break;

	case PCK_V2030:
		try {
			PCKTAIL_V2030 PckTail;
			isFoundVer = get_pckAllInfo_by_version(cRead, PckTail, m_PckAllInfo, (PCKHEAD_V2030*)&cPckHead, iDetectedPckID, qwPckSizeInHeader);
		}
		catch (MyException ex) {
			Logger.e(ex.what());
			goto dect_err;
		}
		break;
	case PCK_VXAJH:
		try {
			PCKTAIL_VXAJH PckTail;
			isFoundVer = get_pckAllInfo_by_version(cRead, PckTail, m_PckAllInfo, (PCKHEAD_VXAJH*)&cPckHead, iDetectedPckID, qwPckSizeInHeader);
		}
		catch (MyException ex) {
			Logger.e(ex.what());
			goto dect_err;
		}
		break;
	}

	if (!isFoundVer) {
		Logger_el(TEXT_VERSION_NOT_FOUND);
		goto dect_err;
	}

	m_PckAllInfo.dwFinalFileCount = m_PckAllInfo.dwFileCountOld = m_PckAllInfo.dwFileCount = dwTailVals[2];
	wcscpy_s(m_PckAllInfo.szFilename, lpszPckFile);
	m_PckAllInfo.lpSaveAsPckVerFunc = m_PckAllInfo.lpDetectedPckVerFunc = &cPckVersionFunc[iDetectedPckID];

	//调整文件大小
	uint64_t qwSizeFileBefore = cRead.GetFileSize();

	if (qwPckSizeInHeader < qwSizeFileBefore){

		cRead.clear();
		CMapViewFileMultiPckWrite cWrite(cPckVersionFunc[iDetectedPckID].cPckXorKeys.dwMaxSinglePckSize);
		
		if (cWrite.OpenPck(lpszPckFile, OPEN_EXISTING)) {
			
			cWrite.SetFilePointer(qwPckSizeInHeader);
			cWrite.SetEndOfFile();

			Logger.i("Pck file size does not match, adjusted from %llu to %llu", qwSizeFileBefore, qwPckSizeInHeader);

		}
	}

	return TRUE;

dect_err:
	m_PckAllInfo.lpSaveAsPckVerFunc = NULL;
	PrintInvalidVersionDebugInfo(lpszPckFile);
	return FALSE;
}

template<typename T>
void* FillHeadData(LPPCK_ALL_INFOS lpPckAllInfo)
{ 
	static DWORD headbuf[3]; 
	T lpHead = (T)headbuf; 
	lpHead->dwHeadCheckHead = lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.HeadVerifyKey1; 
	if (lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.HeadVerifyKey2) { 
		lpHead->dwHeadCheckTail = lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.HeadVerifyKey2; 
		lpHead->dwPckSize = (DWORD)lpPckAllInfo->qwPckSize; 
	} else { 
		lpHead->dwPckSize = lpPckAllInfo->qwPckSize; 
	} 
	return headbuf; 
}

template<typename T>
void* FillTailData(LPPCK_ALL_INFOS lpPckAllInfo)
{ 
	static BYTE tailbuf[MAX_TAIL_LENGTH];  
	T lpTail = (T)tailbuf; 
	lpTail->dwIndexTableCheckHead = lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.TailVerifyKey1; 
	lpTail->dwVersion0 = lpTail->dwVersion = lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.Version; 
	lpTail->dwEntryOffset = lpPckAllInfo->dwAddressOfFileEntry ^ lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.IndexesEntryAddressCryptKey; 
	lpTail->dwFlags = 0; 
	memcpy(lpTail->szAdditionalInfo, lpPckAllInfo->szAdditionalInfo, sizeof(lpTail->szAdditionalInfo)); 
	lpTail->dwIndexTableCheckTail = lpPckAllInfo->lpSaveAsPckVerFunc->cPckXorKeys.TailVerifyKey2; 
	lpTail->dwFileCount = lpPckAllInfo->dwFinalFileCount; 
	return tailbuf; 
}

template<typename T>
void* FillIndexData(LPPCKFILEINDEX lpFileIndex, T lpPckIndexTableClear)
{
	memcpy(lpPckIndexTableClear->szFilename, lpFileIndex->szFilename, sizeof(lpPckIndexTableClear->szFilename));
	lpPckIndexTableClear->dwUnknown1 = lpPckIndexTableClear->dwUnknown2 = 0;
	lpPckIndexTableClear->dwAddressOffset = lpFileIndex->dwAddressOffset;
	lpPckIndexTableClear->dwFileCipherTextSize = lpFileIndex->dwFileCipherTextSize;
	lpPckIndexTableClear->dwFileClearTextSize = lpFileIndex->dwFileClearTextSize;
	return (void*)lpPckIndexTableClear;
}

template<typename T>
BOOL PickIndexData(LPPCKFILEINDEX lpFileIndex, T* lpPckIndexTableClear)
{ 
	lpFileIndex->dwAddressOffset = lpPckIndexTableClear->dwAddressOffset; 
	lpFileIndex->dwFileCipherTextSize = lpPckIndexTableClear->dwFileCipherTextSize; 
	lpFileIndex->dwFileClearTextSize = lpPckIndexTableClear->dwFileClearTextSize; 
	memcpy(lpFileIndex->szFilename, lpPckIndexTableClear->szFilename, sizeof(lpPckIndexTableClear->szFilename)); 
	return TRUE; 
}

//template
//void* FillIndexData<LPPCKFILEINDEX_V2020>(void *param, LPPCKFILEINDEX_V2020 lpPckIndexTableClear);


//文件头、尾的数据填写和数据写入
void* CPckClassVersionDetect::FillHeadData_V2020(void *param)
{
	return FillHeadData<LPPCKHEAD_V2020>((LPPCK_ALL_INFOS)param);
}

void* CPckClassVersionDetect::FillHeadData_V2030(void *param)
{
	return FillHeadData<LPPCKHEAD_V2030>((LPPCK_ALL_INFOS)param);
}

void* CPckClassVersionDetect::FillTailData_V2020(void *param)
{
	return FillTailData<LPPCKTAIL_V2020>((LPPCK_ALL_INFOS)param);
}

void* CPckClassVersionDetect::FillTailData_V2030(void *param)
{
	return FillTailData<LPPCKTAIL_V2030>((LPPCK_ALL_INFOS)param);
}

void* CPckClassVersionDetect::FillTailData_VXAJH(void *param)
{
	return FillTailData<LPPCKTAIL_VXAJH>((LPPCK_ALL_INFOS)param);
}


//数据从param -> lpTail
void* CPckClassVersionDetect::FillIndexData_V2020(void *param, void *pckFileIndexBuf)
{
	return FillIndexData((LPPCKFILEINDEX)param, (LPPCKFILEINDEX_V2020)pckFileIndexBuf);
}

void* CPckClassVersionDetect::FillIndexData_V2030(void *param, void *pckFileIndexBuf)
{
	return FillIndexData((LPPCKFILEINDEX)param, (LPPCKFILEINDEX_V2030)pckFileIndexBuf);
}

void* CPckClassVersionDetect::FillIndexData_VXAJH(void *param, void *pckFileIndexBuf)
{
	return FillIndexData((LPPCKFILEINDEX)param, (LPPCKFILEINDEX_VXAJH)pckFileIndexBuf);
}

//数据从lpIndex -> param
BOOL CPckClassVersionDetect::PickIndexData_V2020(void *param, void* lpIndex)
{
	return PickIndexData((LPPCKFILEINDEX)param, (LPPCKFILEINDEX_V2020)lpIndex);
}

BOOL CPckClassVersionDetect::PickIndexData_V2030(void *param, void* lpIndex)
{
	return PickIndexData((LPPCKFILEINDEX)param, (LPPCKFILEINDEX_V2030)lpIndex);
}

BOOL CPckClassVersionDetect::PickIndexData_VXAJH(void *param, void* lpIndex)
{
	return PickIndexData((LPPCKFILEINDEX)param, (LPPCKFILEINDEX_VXAJH)lpIndex);
}

#pragma endregion