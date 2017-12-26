//////////////////////////////////////////////////////////////////////
// tAttrDlg.cpp: WinPCK 界面线程部分
// 对话框代码
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2017.12.26
//////////////////////////////////////////////////////////////////////

#include "miscdlg.h"
#include <shlwapi.h>
#include "PckHeader.h"

TAttrDlg::TAttrDlg(void *_lpPckInfo, void *_lpRootInfo, QWORD _qwPckFileSize, char *_lpszPath, BOOL _isSearchMode, TWin *_win) : TDlg(IDD_DIALOG_ATTR, _win)
{
	lpPckInfo = _lpPckInfo;
	lpRootInfo = _lpRootInfo;
	lpszPath = _lpszPath;
	isSearchMode = _isSearchMode;
	qwPckFileSize = _qwPckFileSize;

}

//TAttrDlg::~TAttrDlg()
//{
//}

BOOL TAttrDlg::EvCreate(LPARAM lParam)
{
	//ultoa(lpPckNode->lpPckIndexTable->cFileIndex.dwFileCipherTextSize, szPrintf, 10));

	char	*lpszFilename;
	char	szPrintf[64];
	char	szPrintfBytesize[16];
	LPCSTR	lpszFormat = "%s (%llu 字节)";
	LPCSTR	lpszFileFormat = "%s (%u 字节)";



	if(isSearchMode) {
		LPPCKINDEXTABLE	lpPckIndex = (LPPCKINDEXTABLE)lpPckInfo;
		char	szFilename[MAX_PATH_PCK_260];

		memcpy(szFilename, lpPckIndex->cFileIndex.szFilename, MAX_PATH_PCK_260);

		SetDlgItemTextA(IDC_EDIT_ATTR_TYPE, "文件");

		if(NULL != (lpszFilename = strrchr(szFilename, '\\'))) {

			*lpszFilename++ = 0;
			SetDlgItemTextA(IDC_EDIT_ATTR_PATH, szFilename);

		} else if(NULL != (lpszFilename = strrchr(szFilename, '/'))) {

			*lpszFilename++ = 0;
			SetDlgItemTextA(IDC_EDIT_ATTR_PATH, szFilename);

		} else {

			lpszFilename = lpPckIndex->cFileIndex.szFilename;
			SetDlgItemTextA(IDC_EDIT_ATTR_PATH, "");
		}

		SetDlgItemTextA(IDC_EDIT_ATTR_NAME, lpszFilename);

		//压缩大小
		sprintf_s(szPrintf, 64, lpszFileFormat,
			StrFormatByteSizeA(lpPckIndex->cFileIndex.dwFileCipherTextSize, szPrintfBytesize, 16),
			lpPckIndex->cFileIndex.dwFileCipherTextSize);

		SetDlgItemTextA(IDC_EDIT_ATTR_CIPHER, szPrintf);

		//实际大小
		sprintf_s(szPrintf, 64, lpszFileFormat,
			StrFormatByteSizeA(lpPckIndex->cFileIndex.dwFileClearTextSize, szPrintfBytesize, 16),
			lpPckIndex->cFileIndex.dwFileClearTextSize);

		SetDlgItemTextA(IDC_EDIT_ATTR_SIZE, szPrintf);

		//压缩率
		if(0 == lpPckIndex->cFileIndex.dwFileClearTextSize)
			SetDlgItemTextA(IDC_EDIT_ATTR_CMPR, "-");
		else {
			sprintf_s(szPrintf, 64, "%.1f%%", lpPckIndex->cFileIndex.dwFileCipherTextSize * 100 / (double)lpPckIndex->cFileIndex.dwFileClearTextSize);
			SetDlgItemTextA(IDC_EDIT_ATTR_CMPR, szPrintf);
		}

		//包含
		SetDlgItemTextA(IDC_EDIT_ATTR_FILECOUNT, "-");

		//偏移地址
		//		#ifdef PCKV202
		sprintf_s(szPrintf, 64, "%llu (0x%016llX)", lpPckIndex->cFileIndex.dwAddressOffset, lpPckIndex->cFileIndex.dwAddressOffset);
		SetDlgItemTextA(IDC_EDIT_ATTR_ADDR, szPrintf);

	} else {

		LPPCK_PATH_NODE	lpPckNode = (LPPCK_PATH_NODE)lpPckInfo;

		SetDlgItemTextA(IDC_EDIT_ATTR_NAME, lpszFilename = lpPckNode->szName);
		SetDlgItemTextA(IDC_EDIT_ATTR_PATH, lpszPath);

		if(NULL == lpPckNode->child)//文件
		{
			SetDlgItemTextA(IDC_EDIT_ATTR_TYPE, "文件");

			//压缩大小
			sprintf_s(szPrintf, 64, lpszFileFormat,
				StrFormatByteSizeA(lpPckNode->lpPckIndexTable->cFileIndex.dwFileCipherTextSize, szPrintfBytesize, 16),
				lpPckNode->lpPckIndexTable->cFileIndex.dwFileCipherTextSize);

			SetDlgItemTextA(IDC_EDIT_ATTR_CIPHER, szPrintf);


			//实际大小
			sprintf_s(szPrintf, 64, lpszFileFormat,
				StrFormatByteSizeA(lpPckNode->lpPckIndexTable->cFileIndex.dwFileClearTextSize, szPrintfBytesize, 16),
				lpPckNode->lpPckIndexTable->cFileIndex.dwFileClearTextSize);

			SetDlgItemTextA(IDC_EDIT_ATTR_SIZE, szPrintf);



			//压缩率
			if(0 == lpPckNode->lpPckIndexTable->cFileIndex.dwFileClearTextSize)
				SetDlgItemTextA(IDC_EDIT_ATTR_CMPR, "-");
			else {
				sprintf_s(szPrintf, 64, "%.1f%%", lpPckNode->lpPckIndexTable->cFileIndex.dwFileCipherTextSize / (double)lpPckNode->lpPckIndexTable->cFileIndex.dwFileClearTextSize * 100);
				SetDlgItemTextA(IDC_EDIT_ATTR_CMPR, szPrintf);
			}

			//包含
			SetDlgItemTextA(IDC_EDIT_ATTR_FILECOUNT, "-");

			//偏移地址
			sprintf_s(szPrintf, 64, "%llu (0x%016llX)", lpPckNode->lpPckIndexTable->cFileIndex.dwAddressOffset, lpPckNode->lpPckIndexTable->cFileIndex.dwAddressOffset);
			SetDlgItemTextA(IDC_EDIT_ATTR_ADDR, szPrintf);
		} else {

			SetDlgItemTextA(IDC_EDIT_ATTR_TYPE, "文件夹");

			//压缩大小
			sprintf_s(szPrintf, 64, lpszFormat,
				StrFormatByteSize64A(lpPckNode->child->qdwDirCipherTextSize, szPrintfBytesize, 16),
				lpPckNode->child->qdwDirCipherTextSize);
			SetDlgItemTextA(IDC_EDIT_ATTR_CIPHER, szPrintf);


			//实际大小
			sprintf_s(szPrintf, 64, lpszFormat,
				StrFormatByteSize64A(lpPckNode->child->qdwDirClearTextSize, szPrintfBytesize, 16),
				lpPckNode->child->qdwDirClearTextSize);

			SetDlgItemTextA(IDC_EDIT_ATTR_SIZE, szPrintf);


			//压缩率
			sprintf_s(szPrintf, 64, "%.1f%%", lpPckNode->child->qdwDirCipherTextSize / (double)lpPckNode->child->qdwDirClearTextSize * 100.0);
			SetDlgItemTextA(IDC_EDIT_ATTR_CMPR, szPrintf);

			//包含
			sprintf_s(szPrintf, 64, "%u 个文件，%u 个文件夹", lpPckNode->child->dwFilesCount, lpPckNode->child->dwDirsCount);
			SetDlgItemTextA(IDC_EDIT_ATTR_FILECOUNT, szPrintf);

			SetDlgItemTextA(IDC_EDIT_ATTR_ADDR, "-");

		}


	}

	//文件包信息
	LPPCK_PATH_NODE	lpRootNode = (LPPCK_PATH_NODE)lpRootInfo;
	//文件总大小
	sprintf_s(szPrintf, 64, lpszFormat,
		StrFormatByteSize64A(lpRootNode->child->qdwDirClearTextSize, szPrintfBytesize, 16),
		lpRootNode->child->qdwDirClearTextSize);

	SetDlgItemTextA(IDC_EDIT_ATTR_ALLSIZE, szPrintf);

	//压缩包大小
	sprintf_s(szPrintf, 64, lpszFormat,
		StrFormatByteSize64A(lpRootNode->child->qdwDirCipherTextSize, szPrintfBytesize, 16),
		lpRootNode->child->qdwDirCipherTextSize);
	SetDlgItemTextA(IDC_EDIT_ATTR_PCKSIZE, szPrintf);

	//压缩率
	sprintf_s(szPrintf, 64, "%.1f%%", lpRootNode->child->qdwDirCipherTextSize / (double)lpRootNode->child->qdwDirClearTextSize * 100.0);
	SetDlgItemTextA(IDC_EDIT_ATTR_PCKCMPR, szPrintf);

	//冗余数据量
	//int dwDirt = dwPckFileSize - lpRootNode->child->dwDirCipherTextSize;
	sprintf_s(szPrintf, 64, lpszFormat,
		StrFormatByteSize64A(qwPckFileSize, szPrintfBytesize, 16),
		qwPckFileSize);
	SetDlgItemTextA(IDC_EDIT_ATTR_DIRT, szPrintf);


	//窗口文字
	char	szTitle[MAX_PATH];
	sprintf_s(szTitle, MAX_PATH, "%s 属性", lpszFilename);
	SetWindowTextA(szTitle);

	Show();

	return	FALSE;
}

BOOL TAttrDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	switch(wID) {
	case IDOK:
		EndDialog(wID);
		return	TRUE;

	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}
