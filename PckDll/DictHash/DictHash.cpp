//////////////////////////////////////////////////////////////////////
// DictHash.cpp: 用于解析完美世界公司的zup文件中的数据
// 解码zup的base64字典哈希表
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#include "DictHash.h"
#include <string.h>
#include <stdlib.h>

#if PCK_DEBUG_OUTPUT
#include <windows.h>
#include <stdio.h>
#endif

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4267 )


// JS Hash Function
unsigned int _fastcall JSHash(const char *str)
{
	unsigned int hash = 1315423911;

	while(*str) {
		hash ^= ((hash << 5) + (*str++) + (hash >> 2));
	}

	return (hash & HASH_TABLE_LENGTH);

}

void _fastcall zupbase64cpy(char *_dst, const char *_src, int len)
{

	for(; len > 0; len--) {
		if('-' == *_src)
			_src++, *_dst++ = '/';
		else
			*_dst++ = *_src++;
	}
}

CDictHash::CDictHash()
{
	lpDictHashTable = new LPZUP_FILENAME_DICT[HASH_TABLE_LENGTH + 1];
	memset(lpDictHashTable, 0, sizeof(LPZUP_FILENAME_DICT) * (HASH_TABLE_LENGTH + 1));

}

CDictHash::~CDictHash()
{

	LPZUP_FILENAME_DICT *lpDictHash = lpDictHashTable;

	for(unsigned int i = 0;i < HASH_TABLE_LENGTH;i++) {
		if(NULL != *lpDictHash) {
			Dealloc(*lpDictHash);
		}
		lpDictHash++;
	}

	delete[] lpDictHashTable;

}

LPZUP_FILENAME_DICT CDictHash::add(const char *keystr)
{

	unsigned char len = strlen(keystr);
	char szlwrstr[MAX_PATH_PCK];
	memcpy(szlwrstr, keystr, len + 1);

	unsigned int key = JSHash(strlwr(szlwrstr));
	unsigned int sizeStuct = sizeof(ZUP_FILENAME_DICT);

	LPZUP_FILENAME_DICT	lpDict = *(lpDictHashTable + key);

	if(NULL == lpDict) {
		lpDict = *(lpDictHashTable + key) = AllocNode(sizeStuct);
		lpDict->realstrlength = len;
		memcpy(lpDict->base64str, szlwrstr, len);
		//memcpy(lpDict->realbase64str, keystr, len);
		zupbase64cpy(lpDict->realbase64str, keystr, len);

		return lpDict;
	} else {

		do {
			if(0 == strcmp(lpDict->base64str, szlwrstr)) {
				return 0;
			}

			if(NULL == lpDict->next) {
				lpDict->next = AllocNode(sizeStuct);
				lpDict = lpDict->next;
				lpDict->realstrlength = len;
				memcpy(lpDict->base64str, szlwrstr, len);
				//memcpy(lpDict->realbase64str, keystr, len);
				zupbase64cpy(lpDict->realbase64str, keystr, len);

				return lpDict;
			} else {
				lpDict = lpDict->next;
			}

		} while(1);

	}

}

LPZUP_FILENAME_DICT CDictHash::find(const char *keystr)
{
	unsigned int key = JSHash(keystr);

	LPZUP_FILENAME_DICT	lpDict = *(lpDictHashTable + key);

	if(NULL == lpDict) {
		return 0;
	} else {
		if(NULL == lpDict->next) {
			return lpDict;
		} else {

			do {
				if(0 == strcmp(lpDict->base64str, keystr)) {
					return lpDict;
				}
				lpDict = lpDict->next;
			} while(NULL != lpDict);

			return 0;
		}
	}

}


LPZUP_FILENAME_DICT CDictHash::AllocNode(unsigned int sizeStuct)
{
	LPZUP_FILENAME_DICT		lpMallocNode;

	if(NULL == (lpMallocNode = (LPZUP_FILENAME_DICT)malloc(sizeStuct))) {
		return lpMallocNode;
	}
	//初始化内存
	memset(lpMallocNode, 0, sizeStuct);

	return lpMallocNode;

}

void CDictHash::Dealloc(LPZUP_FILENAME_DICT lpDictHash)
{
#if PCK_DEBUG_OUTPUT
	LPZUP_FILENAME_DICT lpDebug = lpDictHash;
#endif
	LPZUP_FILENAME_DICT lpTemp = lpDictHash;

	while(NULL != lpDictHash) {
		lpTemp = lpDictHash->next;
		free(lpDictHash);
		lpDictHash = lpTemp;
	}

}
