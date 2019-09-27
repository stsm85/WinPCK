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
#include <intrin.h>
#include "CharsCodeConv.h"
#include "TextLineSpliter.h"

_inline void CZupClass::DecodeDict(LPZUP_FILENAME_DICT lpZupDict)
{
	char			szUTF8str[MAX_PATH_PCK];

	//base64解码
	DWORD	dwRealLen = lpZupDict->realstrlength;
	base64_decode(lpZupDict->realbase64str, dwRealLen, szUTF8str);

	lpZupDict->wrealstrlength = U8toW(szUTF8str, lpZupDict->wrealstr, MAX_PATH_PCK, -1) - 1;
	lpZupDict->realstrlength = CPckClassCodepage::PckFilenameCode2Ansi(lpZupDict->wrealstr, lpZupDict->realstr, MAX_PATH_PCK);


}

void CZupClass::AddDict(std::string& base_file)
{
	char ch[2] = { 0 };
	*ch = base_file.at(0);

	if ('/' == *ch || '\\' == *ch)
	{
		std::vector<std::string> line_content;
		CTextUnitsA::Split(base_file, line_content, ch, LINE_EMPTY_DELETE);

		for (int i = 0; i < line_content.size(); i++) {
			LPZUP_FILENAME_DICT lpZupDict;
			if (NULL != (lpZupDict = m_lpDictHash->add(line_content[i].c_str()))) {
				DecodeDict(lpZupDict);
			}
		}
	}
	else {

		LPZUP_FILENAME_DICT lpZupDict;
		if (NULL != (lpZupDict = m_lpDictHash->add(base_file.c_str()))) {
			DecodeDict(lpZupDict);
		}
	}
}

void CZupClass::DecodeFilename(char *_dst, wchar_t *_wdst, char *_src)
{
	char	__srcbuf[MAX_PATH_PCK];
	char	*__srcbufptr;

	memset(_dst + 8, 0, MAX_PATH_PCK - 8);
	//复制"element\"
	//memcpy(_dst, _src, 8);
	//_dst -> _wdst

	__m128i xmm0 = _mm_setzero_si128();
	__m128i xmm1 = _mm_loadl_epi64((__m128i*)_src);
	__m128i xmm2 = _mm_unpacklo_epi8(xmm1, xmm0);
	_mm_storeu_si128((__m128i*)_wdst, xmm2);

	_dst += 8, _src += 8, _wdst += 8;

	//char	*_srcptr = _src;
	//DWORD	dwBase64Len;

	while(0 != *_src) {
		__srcbufptr = __srcbuf;

		while(*_src && '\\' != *_src && '/' != *_src) {
			*__srcbufptr++ = *_src++;
		}

		*__srcbufptr = 0;

		//在dict中查找base64
		LPZUP_FILENAME_DICT	lpZupDict;

		if(NULL != (lpZupDict = m_lpDictHash->find(__srcbuf))) {
			memcpy(_dst, lpZupDict->realstr, lpZupDict->realstrlength);
			_dst += lpZupDict->realstrlength;

			memcpy(_wdst, lpZupDict->wrealstr, lpZupDict->wrealstrlength * sizeof(wchar_t));
			_wdst += lpZupDict->wrealstrlength;

			*_dst++ = *_src;
			*_wdst++ = *_src++;
		} else {
			*_dst++ = *_src;
			*_wdst++ = *_src++;
		}
	}
}


BOOL CZupClass::BuildZupBaseDict()
{
	LPPCKINDEXTABLE	lpPckIndexTable = m_PckAllInfo.lpPckIndexTable;

	for(uint32_t i = 0;i < m_PckAllInfo.dwFileCount;i++) {
		//以element\开头的都需要解码
		//其他直接复制
		//"element\" = 0x6d656c65, 0x5c746e656d656c65
		//0x2D76 = "v-"
		if(0x2D76 == *(uint16_t*)lpPckIndexTable->cFileIndex.szFilename) {
			//wID = atoul(lpNode->szName + 2, NULL, 10);
			//读取inc文件
			char	*_incbuf = (char*)malloc(lpPckIndexTable->cFileIndex.dwFileClearTextSize+1);

			if(NULL == _incbuf) {
				//lpNode = lpNode->next;
				continue;
			}

			if(GetSingleFileData(lpPckIndexTable, _incbuf)) {

				_incbuf[lpPckIndexTable->cFileIndex.dwFileClearTextSize] = 0;
				std::vector<std::string> lines;

				CTextUnitsA::SplitLine(_incbuf, lines, LINE_TRIM_LEFT | LINE_TRIM_RIGHT | LINE_EMPTY_DELETE);
				
				for (int j = 0; j < lines.size(); j++) {

					std::string& oneline = lines[j];
					if (oneline.at(0) == '!' || oneline.at(0) == '+') {
						std::vector<std::string> line_content;
						CTextUnitsA::Split(oneline, line_content, " ");
						AddDict(line_content[1]);
					}
				}
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

const PCKINDEXTABLE* CZupClass::GetBaseFileIndex(const PCKINDEXTABLE* lpIndex, const PCKINDEXTABLE* lpZeroBaseIndex)
{
	return m_PckAllInfo.lpPckIndexTable + (lpIndex - lpZeroBaseIndex);
}