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

#include "MapViewFile.h"
#include "PckClass.h"

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



*/
const PCK_KEYS CPckClass::cPckKeys[PCK_VERSION_NUMS] = \
/*		ID				名称				版本ID		版本值	0xAAAAAAAA	0xBBBBBBBB	0xCCCCCCCC	0xDDDDDDDDEEEEEEEE	0xFFFFFFFF	0xGGGGGGGG	0xHHHHHHHH	分块大小	
		id				name				VersionId	Version	HeadVerifyKey1			TailVerifyKey1					TailVerifyKey2			IndexCompressedFilenameDataLengthCryptKey2
																			HeadVerifyKey2			IndexesEntryAddressCryptKey		IndexCompressedFilenameDataLengthCryptKey1*/
{	{ PCK_VERSION_ZX,	TEXT("诛仙"),		PCK_V2020, 0x20002, 0x4DCA23EF, 0x56A089B7, 0xFDFDFEEE, 0x00000000A8937462, 0xF00DBEEF, 0xA8937462, 0xF1A43653, 0x7fffff00}, \
	{PCK_VERSION_SDS,	TEXT("圣斗士"),		PCK_V2020, 0x20002, 0x4DCA23EF, 0x56A089B7, 0x7b2a7820, 0x0000000062a4f9e1, 0xa75dc142, 0x62a4f9e1, 0x3520c3d5, 0x7fffff00}, \
	{PCK_VERSION_XAJH,	TEXT("笑傲江湖"),	PCK_V2030, 0x20003, 0x5edb34f0, 0x00000000, 0x7b2a7820, 0x49ab7f1d33c3eddb, 0xa75dc142, 0x62a4f9e1, 0x3520c3d5, 0xffffff00}, \
	{PCK_VERSION_ZXNEW, TEXT("诛仙3·十年"),PCK_V2031, 0x20003, 0x4DCA23EF, 0x00000000, 0xFDFDFEEE, 0xffffffffA8937462, 0xF00DBEEF, 0xA8937462, 0xF1A43653, 0x7fffff00}, \
	{PCK_VERSION_SM,	TEXT("神魔"),		PCK_V2020, 0x20002, 0x4DCA23EF, 0x56A089B7, 0xA508BDC4, 0x0000000021c31a3f, 0x853567C8, 0x21c31a3f, 0x185c2025, 0x7fffff00}, \
};

const PCK_VERSION_FUNC CPckClass::cPckVersionFunc[PCK_VERSION_NUMS] = \
{	{ &cPckKeys[PCK_VERSION_ZX],	sizeof(PCKHEAD_V2020), sizeof(PCKTAIL_V2020), sizeof(PCKFILEINDEX_V2020), PickIndexData_V2020, FillHeadData_V2020, FillTailData_V2020, FillIndexData_V2020}, \
	{&cPckKeys[PCK_VERSION_SDS],	sizeof(PCKHEAD_V2020), sizeof(PCKTAIL_V2020), sizeof(PCKFILEINDEX_V2020), PickIndexData_V2020, FillHeadData_V2020, FillTailData_V2020, FillIndexData_V2020}, \
	{&cPckKeys[PCK_VERSION_XAJH],	sizeof(PCKHEAD_V2030), sizeof(PCKTAIL_V2030), sizeof(PCKFILEINDEX_V2030), PickIndexData_V2030, FillHeadData_V2030, FillTailData_V2030, FillIndexData_V2030}, \
	{&cPckKeys[PCK_VERSION_ZXNEW],	sizeof(PCKHEAD_V2031), sizeof(PCKTAIL_V2031), sizeof(PCKFILEINDEX_V2031), PickIndexData_V2031, FillHeadData_V2030, FillTailData_V2031, FillIndexData_V2031}, \
	{&cPckKeys[PCK_VERSION_SM],		sizeof(PCKHEAD_V2020), sizeof(PCKTAIL_V2020), sizeof(PCKFILEINDEX_V2020), PickIndexData_V2020, FillHeadData_V2020, FillTailData_V2020, FillIndexData_V2020}, \
};

//生成界面保存文件时的Filter文本
void CPckClass::BuildSaveDlgFilterString()
{
	*szSaveDlgFilterString = 0;
	TCHAR szPrintf[256];

	for(int i = 0;i < PCK_VERSION_NUMS;i++) {

		_stprintf_s(szPrintf, TEXT("%sPCK文件(*.pck)|*.pck|"), cPckKeys[i].name);
		_tcscat_s(szSaveDlgFilterString, szPrintf);
	}

	TCHAR *lpszStr = szSaveDlgFilterString;
	while(*lpszStr) {

		if(TEXT('|') == *lpszStr)
			*lpszStr = 0;
		++lpszStr;
	}

	*lpszStr = 0;
}

LPCTSTR CPckClass::GetSaveDlgFilterString()
{
	return szSaveDlgFilterString;
}

const PCK_KEYS* CPckClass::GetPckVersion()
{
	return m_PckAllInfo.lpDetectedPckVerFunc->cPckXorKeys;
}

void CPckClass::SetPckVersion(int verID)
{
	if(0 <= verID && PCK_VERSION_NUMS > verID)
		m_PckAllInfo.lpSaveAsPckVerFunc = &cPckVersionFunc[verID];
	else
		PrintLogW(TEXT_INVALID_VERSION);
}


#define PRINT_HEAD_SIZE		0x20
#define PRINT_TAIL_SIZE		0x580
 
void CPckClass::PrintInvalidVersionDebugInfo(LPCTSTR lpszPckFile)
{
	//打印详细原因：
	//hex 一行长度89 数据一共402行，大小0x8BC2
	char szPrintf[8192];

	BYTE buf[PRINT_HEAD_SIZE + PRINT_TAIL_SIZE + 0x10];

	//读取文件头
	CMapViewFileRead *lpRead = new CMapViewFileRead();

	if(!lpRead->OpenPck(lpszPckFile)) {
		PrintLogE(TEXT_OPENNAME_FAIL, lpszPckFile, __FILE__, __FUNCTION__, __LINE__);
		goto dect_err;
	}

	if(lpRead->GetFileSize() <= (PRINT_TAIL_SIZE + PRINT_HEAD_SIZE)) {

		if(!lpRead->Read(buf, lpRead->GetFileSize())) {
			PrintLogE(TEXT_READFILE_FAIL, __FILE__, __FUNCTION__, __LINE__);
			goto dect_err;
		}

		CRaw2HexString cHexStr(buf, lpRead->GetFileSize());

		sprintf_s(szPrintf, "文件信息：\n文件大小：%lld\n文件概要数据：\n", lpRead->GetFileSize());
		strcat_s(szPrintf, cHexStr.GetHexString());


	} else {

		if(!lpRead->Read(buf, PRINT_HEAD_SIZE)) {
			PrintLogE(TEXT_READFILE_FAIL, __FILE__, __FUNCTION__, __LINE__);
			goto dect_err;
		}

		QWORD qwWhereToMove = (lpRead->GetFileSize() - PRINT_TAIL_SIZE) & 0xfffffffffffffff0;
		QWORD qwBytesToRead = lpRead->GetFileSize() - qwWhereToMove;

		lpRead->SetFilePointer(qwWhereToMove, FILE_BEGIN);

		if(!lpRead->Read(buf + PRINT_HEAD_SIZE, qwBytesToRead)) {
			PrintLogE(TEXT_READFILE_FAIL, __FILE__, __FUNCTION__, __LINE__);
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

	PrintLogD(szPrintf);
dect_err:
	delete lpRead;

}
#undef PRINT_HEAD_SIZE
#undef PRINT_TAIL_SIZE

//读取文件头和尾确定pck文件版本，返回版本ID
BOOL CPckClass::DetectPckVerion(LPCTSTR lpszPckFile, LPPCK_ALL_INFOS pckAllInfo)
{
	PCKHEAD_V2020 cPckHead;
	DWORD		dwTailVals[4];

	int iDetectedPckID = -1;
	//读取文件头
	CMapViewFileRead *lpRead = new CMapViewFileRead();

	if(!lpRead->OpenPck(lpszPckFile)) {
		PrintLogE(TEXT_OPENNAME_FAIL, lpszPckFile, __FILE__, __FUNCTION__, __LINE__);
		goto dect_err;
	}

	if(!lpRead->Read(&cPckHead, sizeof(PCKHEAD_V2020))) {
		PrintLogE(TEXT_READFILE_FAIL, __FILE__, __FUNCTION__, __LINE__);
		goto dect_err;
	}

	//判断是不是64位的文件大小
	if(!lpRead->SetPckPackSize((0x100 < cPckHead.dwHeadCheckTail) ? cPckHead.dwPckSize : ((PCKHEAD_V2030*)&cPckHead)->dwPckSize)) {

		PrintLogE(TEXT_PCK_SIZE_INVALID, __FILE__, __FUNCTION__, __LINE__);
		goto dect_err;
	}

	lpRead->SetFilePointer(-((QWORD)(sizeof(DWORD) * 4)), FILE_END);

	if(!lpRead->Read(&dwTailVals, sizeof(DWORD) * 4)) {
		PrintLogE(TEXT_READFILE_FAIL, __FILE__, __FUNCTION__, __LINE__);
		goto dect_err;
	}

	if(0x20003 == dwTailVals[3]) {
		if(0 != dwTailVals[0])
			dwTailVals[1] = dwTailVals[0];

		for(int i = 0;i < PCK_VERSION_NUMS;i++) {
			if((cPckKeys[i].Version == 0x20003) &&
				(cPckKeys[i].TailVerifyKey2 == dwTailVals[1]) &&
				(cPckKeys[i].HeadVerifyKey1 == cPckHead.dwHeadCheckHead)) {

				iDetectedPckID = i;
				break;
			}
		}
	} else {

		//当版本为202时，TailVerifyKey2的值可能为HeadVerifyKey2 或者 0，此例已在游戏：神魔的pck文件上出现。
		for(int i = 0;i < PCK_VERSION_NUMS;i++) {
			if((cPckKeys[i].Version == dwTailVals[3]) &&
				(cPckKeys[i].TailVerifyKey2 == dwTailVals[1]) &&
				(cPckKeys[i].HeadVerifyKey1 == cPckHead.dwHeadCheckHead) &&
				((cPckKeys[i].HeadVerifyKey2 == cPckHead.dwHeadCheckTail) || (0 == cPckHead.dwHeadCheckTail))) {

				iDetectedPckID = i;
				break;
			}
		}
	}

	//读取PCKINDEXADDR，验证
	if(-1 == iDetectedPckID) {
		PrintLogE(TEXT_VERSION_NOT_FOUND, __FILE__, __FUNCTION__, __LINE__);
		goto dect_err;
	}

	PCKVERSION ver = cPckKeys[iDetectedPckID].VersionId;
	BOOL		isFoundVer = FALSE;

	switch(ver) {
	case PCK_V2020:

		define_get_pckAllInfo_by_version(2020, iDetectedPckID);
		break;
	case PCK_V2030:

		define_get_pckAllInfo_by_version(2030, iDetectedPckID);
		break;
	case PCK_V2031:

		define_get_pckAllInfo_by_version(2031, iDetectedPckID);
		break;
	}

	if(!isFoundVer) {
		PrintLogE(TEXT_VERSION_NOT_FOUND, __FILE__, __FUNCTION__, __LINE__);
		goto dect_err;
	}

	pckAllInfo->lpSaveAsPckVerFunc = pckAllInfo->lpDetectedPckVerFunc = &cPckVersionFunc[iDetectedPckID];

	delete lpRead;
	return TRUE;

dect_err:
	pckAllInfo->lpSaveAsPckVerFunc = NULL;
	delete lpRead;
	PrintInvalidVersionDebugInfo(lpszPckFile);
	return FALSE;
}

define_one_FillHeadData_by_version(2020);
define_one_FillHeadData_by_version(2030);

define_one_FillTailData_by_version(2020);
define_one_FillTailData_by_version(2030);
define_one_FillTailData_by_version(2031);

//数据从param -> lpTail
define_one_FillIndexData_by_version(2020);
define_one_FillIndexData_by_version(2030);
define_one_FillIndexData_by_version(2031);

//数据从lpIndex -> param
define_one_PickIndexData_by_version(2020);
define_one_PickIndexData_by_version(2030);
define_one_PickIndexData_by_version(2031);
