//////////////////////////////////////////////////////////////////////
// ZupClassFunction.cpp: 用于解析完美世界公司的zup文件中的数据，并显示在List中
// 头文件
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.5.23
//////////////////////////////////////////////////////////////////////

#include "ZupClass.h"

#include "..\base64\base64.h"

_inline void CZupClass::DecodeDict(LPZUP_FILENAME_DICT lpZupDict)
{
	char			szUTF8str[MAX_PATH_PCK];
	wchar_t			szUCS2Str[MAX_PATH_PCK];

	//base64解码
	DWORD	dwRealLen = lpZupDict->realstrlength/*strlen(lpZupDict->base64str)*/;
	//Base64Decode((unsigned char *)szUTF8str, (unsigned char *)lpZupDict->realbase64str, dwRealLen);
	base64_decode(lpZupDict->realbase64str, dwRealLen, szUTF8str);

	MultiByteToWideChar(CP_UTF8, 0, szUTF8str, -1, szUCS2Str, MAX_PATH_PCK);
	//UTF82WCS2(szUTF8str, -1, szUCS2Str, MAX_PATH_PCK);
	lpZupDict->realstrlength = WideCharToMultiByte(CP_ACP, 0, szUCS2Str, -1, lpZupDict->realstr, MAX_PATH_PCK, "_", 0) - 1;

}

VOID CZupClass::AddDict(char *&lpszStringToAdd)
{

	LPZUP_FILENAME_DICT lpZupDict;

	char			*lpszFilename = lpszStringToAdd;
	char			*lpszToFind;


	if('\\' == *lpszFilename || '/' == *lpszFilename)lpszFilename++;

	lpszToFind = lpszFilename;

	BOOL bContinueWhile = TRUE;

	do{

		switch(*lpszFilename)
		{
		case '\\':
			
			*lpszFilename++ = 0;
			if(NULL != (lpZupDict = m_lpDictHash->add(lpszToFind)))
			{
				DecodeDict(lpZupDict);
			}
			lpszToFind = lpszFilename;

			break;

		case '/':
			
			*lpszFilename++ = 0;
			if(NULL != (lpZupDict = m_lpDictHash->add(lpszToFind)))
			{
				DecodeDict(lpZupDict);
			}
			lpszToFind = lpszFilename;

			break;

		case 0x0a:

			*lpszFilename++ = 0;
			if(NULL != (lpZupDict = m_lpDictHash->add(lpszToFind)))
			{
				DecodeDict(lpZupDict);
			}
			lpszToFind = lpszFilename;
			
			bContinueWhile = FALSE;
			break;

		default:
			lpszFilename++;

			break;
		}
	}while(bContinueWhile);

	lpszStringToAdd = lpszFilename;
}

void CZupClass::DecodeFilename(char *_dst, char *_src)
{
	char	__srcbuf[MAX_PATH_PCK];
	char	*__srcbufptr;

	memset(_dst, 0, MAX_PATH_PCK);
	//复制"element\"
	memcpy(_dst, _src, 8);
	_dst += 8, _src += 8;
	
	//char	*_srcptr = _src;
	//DWORD	dwBase64Len;

	while(0 != *_src)
	{
		__srcbufptr = __srcbuf;

		while( *_src && '\\' != *_src && '/' != *_src)
		{
			*__srcbufptr++ = *_src++;
		}
		
		*__srcbufptr = 0;

		//在dict中查找base64
		LPZUP_FILENAME_DICT	lpZupDict;

		if(NULL != (lpZupDict = m_lpDictHash->find(__srcbuf)))
		{
			memcpy(_dst, lpZupDict->realstr, lpZupDict->realstrlength);
			_dst += lpZupDict->realstrlength;
			*_dst++ = *_src++;
		}else{
			*_dst++ = *_src++;
		}
	}
}


BOOL CZupClass::BuildZupBaseDict()
{
	LPPCKINDEXTABLE	lpPckIndexTable = m_lpPckIndexTable;

	for(unsigned long i = 0;i<m_PckAllInfo.dwFileCount;i++)
	{
		//以element\开头的都需要解码
		//其他直接复制
		//"element\" = 0x6d656c65, 0x5c746e656d656c65
		//0x2D76 = "v-"
		if(0x2D76 == *(WORD*)lpPckIndexTable->cFileIndex.szFilename){
			//wID = atoul(lpNode->szName + 2, NULL, 10);
			//读取inc文件
			char	*_incbuf = (char*) malloc (lpPckIndexTable->cFileIndex.dwFileClearTextSize);

			if(NULL == _incbuf)
			{
				//lpNode = lpNode->next;
				continue;
			}

			if(GetSingleFileData(NULL, lpPckIndexTable, _incbuf))
			{
				char	*__lpincbuf = _incbuf;
				char	*__lpincbufnextline = _incbuf;
				char	*__lpincbufEnd = _incbuf + lpPckIndexTable->cFileIndex.dwFileClearTextSize;
				//char	*lpszTemp;

				//分层
				//1、'#'://版本
				//2、!+文件
				//3、-签名
				//1、版本
				while(0x0a != *__lpincbufnextline && __lpincbufnextline < __lpincbufEnd)
					__lpincbufnextline++;

				*__lpincbufnextline++ = 0;

				//在此处加入处理版本更新的信息


				//2、!+文件
				__lpincbuf = __lpincbufnextline;
				while('!' == *__lpincbuf || '+' == *__lpincbuf)
				{
					//md5值 
					//34个字节的md5信息
					//memcpy(szMD5, __lpincbuf, 33);
					//szMD5[33] = 0;
					__lpincbuf += 34;

					//文件名，base64
					AddDict(__lpincbuf);
				}

				//3、//签名


				//while(__lpincbuf <= __lpincbufEnd)
				//{

				//	while(0x0d != *__lpincbuf && 0x0a != *__lpincbuf && __lpincbuf <= __lpincbufEnd)
				//		__lpincbuf++;


				//	while((0x0d == *__lpincbuf || 0x0a == *__lpincbuf) && __lpincbuf <= __lpincbufEnd)
				//		__lpincbuf++;


				//	switch(*__lpincbuf)
				//	{
				//	case '#'://版本
				//		//while(32 == (*__lpincbuf++))
				//		//	;

				//		//wMinVersion = strtoul(__lpincbuf, &lpszTemp, 10);
				//		//__lpincbuf = lpszTemp;

				//		//while(32 == (*__lpincbuf++))
				//		//	;

				//		//wTargetVersion = strtoul(__lpincbuf, &lpszTemp, 10);
				//		//__lpincbuf = lpszTemp;


				//		//while(32 == (*__lpincbuf++))
				//		//	;

				//		//dwTotalSize = strtoul(__lpincbuf, &lpszTemp, 10);
				//		//__lpincbuf = lpszTemp;

				//		break;

				//	case '!':
				//	case '+':
				//		//34个字节的md5信息
				//		//memcpy(szMD5, __lpincbuf, 33);
				//		//szMD5[33] = 0;
				//		__lpincbuf += 34;

				//		//文件名，base64
				//		AddDict(__lpincbuf);

				//		break;

				//	case '-':
				//		//签名
				//		

				//		break;

				//	default:

				//		break;
				//	}

				//}

			}

			free(_incbuf);
		}
		lpPckIndexTable++;
	}
	//if(NULL == m_lpZupDict)
	//	return FALSE;
	//else
		return TRUE;
}


//删除一个节点
VOID CZupClass::DeleteNode(LPPCK_PATH_NODE lpNode)
{
	PrintLogE(TEXT_NOTSUPPORT);
}

//重命名一个节点
BOOL CZupClass::RenameNode(LPPCK_PATH_NODE lpNode, char* lpszReplaceString)
{
	PrintLogE(TEXT_NOTSUPPORT);
	return FALSE;
}

BOOL CZupClass::RenameNodeEnum(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp)
{
	PrintLogE(TEXT_NOTSUPPORT);
	return FALSE;
}

BOOL CZupClass::RenameNode(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp)
{
	PrintLogE(TEXT_NOTSUPPORT);
	return FALSE;
}

VOID CZupClass::RenameIndex(LPPCK_PATH_NODE lpNode, char* lpszReplaceString)
{
	PrintLogE(TEXT_NOTSUPPORT);
	return;
}

VOID CZupClass::RenameIndex(LPPCKINDEXTABLE lpIndex, char* lpszReplaceString)
{
	PrintLogE(TEXT_NOTSUPPORT);
	return;
}


LPPCKINDEXTABLE CZupClass::GetBaseFileIndex(LPPCKINDEXTABLE lpIndex, LPPCKINDEXTABLE lpZeroBaseIndex)
{
	return m_lpPckIndexTable + (lpIndex - lpZeroBaseIndex);
}