static char *tmisc_id = 
	"@(#)Copyright (C) 1996-2010 H.Shirouzu		tmisc.cpp	Ver0.99";
/* ========================================================================
	Project  Name			: Win32 Lightweight  Class Library Test
	Module Name				: Application Frame Class
	Create					: 1996-06-01(Sat)
	Update					: 2010-05-09(Sun)
	Copyright				: H.Shirouzu
	Reference				: 
	======================================================================== */

#include "tlib.h"
#include <stdio.h>

DWORD TWinVersion = ::GetVersion();

HINSTANCE defaultStrInstance;


BOOL THashObj::LinkHash(THashObj *top)
{
	if (priorHash)
		return FALSE;
	this->nextHash = top->nextHash;
	this->priorHash = top;
	top->nextHash->priorHash = this;
	top->nextHash = this;
	return TRUE;
}

BOOL THashObj::UnlinkHash()
{
	if (!priorHash)
		return FALSE;
	priorHash->nextHash = nextHash;
	nextHash->priorHash = priorHash;
	priorHash = nextHash = NULL;
	return TRUE;
}


THashTbl::THashTbl(int _hashNum, BOOL _isDeleteObj)
{
	hashTbl = NULL;
	registerNum = 0;
	isDeleteObj = _isDeleteObj;

	if ((hashNum = _hashNum) > 0) {
		Init(hashNum);
	}
}

THashTbl::~THashTbl()
{
	UnInit();
}

BOOL THashTbl::Init(int _hashNum)
{
	if ((hashTbl = new THashObj [hashNum = _hashNum]) == NULL) {
		return	FALSE;	// VC4's new don't occur exception
	}

	for (int i=0; i < hashNum; i++) {
		THashObj	*obj = hashTbl + i;
		obj->priorHash = obj->nextHash = obj;
	}
	registerNum = 0;
	return	TRUE;
}

void THashTbl::UnInit()
{
	if (hashTbl) {
		if (isDeleteObj) {
			for (int i=0; i < hashNum && registerNum > 0; i++) {
				THashObj	*start = hashTbl + i;
				for (THashObj *obj=start->nextHash; obj != start; ) {
					THashObj *next = obj->nextHash;
					delete obj;
					obj = next;
					registerNum--;
				}
			}
		}
		delete [] hashTbl;
		hashTbl = NULL;
		registerNum = 0;
	}
}

void THashTbl::Register(THashObj *obj, u_int hash_id)
{
	obj->hashId = hash_id;

	if (obj->LinkHash(hashTbl + (hash_id % hashNum))) {
		registerNum++;
	}
}

void THashTbl::UnRegister(THashObj *obj)
{
	if (obj->UnlinkHash()) {
		registerNum--;
	}
}

THashObj *THashTbl::Search(const void *data, u_int hash_id)
{
	THashObj *top = hashTbl + (hash_id % hashNum);

	for (THashObj *obj=top->nextHash; obj != top; obj=obj->nextHash) {
		if (obj->hashId == hash_id && IsSameVal(obj, data)) {
			return obj;
		}
	}
	return	NULL;
}


void InitInstanceForLoadStr(HINSTANCE hI)
{
	defaultStrInstance = hI;
}

LPSTR GetLoadStrA(UINT resId, HINSTANCE hI)
{
	static TResHash	*hash;

	if (hash == NULL) {
		hash = new TResHash(100);
	}

	char		buf[1024];
	TResHashObj	*obj;

	if ((obj = hash->Search(resId)) == NULL) {
		if (::LoadStringA(hI ? hI : defaultStrInstance, resId, buf, sizeof(buf)) >= 0) {
			obj = new TResHashObj(resId, strdup(buf));
			hash->Register(obj);
		}
	}
	return	obj ? (char *)obj->val : NULL;
}

LPWSTR GetLoadStrW(UINT resId, HINSTANCE hI)
{
	static TResHash	*hash;

	if (hash == NULL) {
		hash = new TResHash(100);
	}

	WCHAR		buf[1024];
	TResHashObj	*obj;

	if ((obj = hash->Search(resId)) == NULL) {
		if (::LoadStringW(hI ? hI : defaultStrInstance, resId, buf,
				sizeof(buf) / sizeof(WCHAR)) >= 0) {
			obj = new TResHashObj(resId, wcsdup(buf));
			hash->Register(obj);
		}
	}
	return	obj ? (LPWSTR)obj->val : NULL;
}

static LCID defaultLCID;

void TSetDefaultLCID(LCID lcid)
{
	defaultLCID = lcid ? lcid : ::GetSystemDefaultLCID();

	TSetThreadLocale(defaultLCID);
}

HMODULE TLoadLibraryA(LPSTR dllname)
{
	HMODULE	hModule = ::LoadLibraryA(dllname);

	if (defaultLCID) {
		TSetThreadLocale(defaultLCID);
	}

	return	hModule;
}

HMODULE TLoadLibraryW(LPWSTR dllname)
{
	HMODULE	hModule = ::LoadLibraryW(dllname);

	if (defaultLCID) {
		TSetThreadLocale(defaultLCID);
	}

	return	hModule;
}

/*
BOOL TSetPrivilege(LPTSTR pszPrivilege, BOOL bEnable)
{
    HANDLE           hToken;
    TOKEN_PRIVILEGES tp;

    if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken))
        return FALSE;

    if (!::LookupPrivilegeValue(NULL, pszPrivilege, &tp.Privileges[0].Luid))
        return FALSE;

    tp.PrivilegeCount = 1;

    if (bEnable)
         tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
         tp.Privileges[0].Attributes = 0;

    if (!::AdjustTokenPrivileges(hToken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES)NULL, 0))
         return FALSE;

    if (!::CloseHandle(hToken))
         return FALSE;

    return TRUE;
}
*/

/*=========================================================================
	bin <-> hex
=========================================================================*/
static char  *hexstr   =  "0123456789abcdef";
static WCHAR *hexstr_w = L"0123456789abcdef";

inline u_char hexchar2char(u_char ch)
{
	if (ch >= '0' && ch <= '9')
		return ch - '0';
	ch = toupper(ch);
	if (ch >= 'A' && ch <= 'Z')
		return ch - 'A' + 10;
	return 0;
}

BOOL hexstr2bin(const char *buf, BYTE *bindata, int maxlen, int *len)
{
	for (*len=0; buf[0] && buf[1] && *len < maxlen; buf+=2, (*len)++)
	{
		bindata[*len] = hexchar2char(buf[0]) << 4 | hexchar2char(buf[1]);
	}
	return	TRUE;
}

int bin2hexstr(const BYTE *bindata, int len, char *buf)
{
	for (const BYTE *end=bindata+len; bindata < end; bindata++)
	{
		*buf++ = hexstr[*bindata >> 4];
		*buf++ = hexstr[*bindata & 0x0f];
	}
	*buf = 0;
	return	len * 2;
}

int bin2hexstrW(const BYTE *bindata, int len, WCHAR *buf)
{
	for (const BYTE *end=bindata+len; bindata < end; bindata++)
	{
		*buf++ = hexstr_w[*bindata >> 4];
		*buf++ = hexstr_w[*bindata & 0x0f];
	}
	*buf = 0;
	return	len * 2;
}

/* little-endian binary to hexstr */
int bin2hexstr_bigendian(const BYTE *bindata, int len, char *buf)
{
	int		sv_len = len;
	while (len-- > 0)
	{
		*buf++ = hexstr[bindata[len] >> 4];
		*buf++ = hexstr[bindata[len] & 0x0f];
	}
	*buf = 0;
	return	sv_len * 2;
}

BOOL hexstr2bin_bigendian(const char *buf, BYTE *bindata, int maxlen, int *len)
{
	*len = 0;
	for (int buflen = (int)strlen(buf); buflen >= 2 && *len < maxlen; buflen-=2, (*len)++)
	{
		bindata[*len] = hexchar2char(buf[buflen-1]) | hexchar2char(buf[buflen-2]) << 4;
	}
	return	TRUE;
}

/*
	16進 -> long long
*/
_int64 hex2ll(char *buf)
{
	_int64	ret = 0;

	for ( ; *buf; buf++)
	{
		if (*buf >= '0' && *buf <= '9')
			ret = (ret << 4) | (*buf - '0');
		else if (toupper(*buf) >= 'A' && toupper(*buf) <= 'F')
			ret = (ret << 4) | (toupper(*buf) - 'A' + 10);
		else continue;
	}
	return	ret;
}


/*=========================================================================
	Debug
=========================================================================*/
#ifdef _DEBUG
void DebugA(char *fmt,...)
{
	char buf[8192];

	va_list	ap;
	va_start(ap, fmt);
	_vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	::OutputDebugStringA(buf);
}

void DebugW(WCHAR *fmt,...)
{
	WCHAR buf[8192];

	va_list	ap;
	va_start(ap, fmt);
	_vsnwprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	::OutputDebugStringW(buf);
}


#ifdef DEBUG_U8
void DebugU8(char *fmt,...)
{
	char buf[8192];

	va_list	ap;
	va_start(ap, fmt);
	_vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	WCHAR *wbuf = U8toW(buf, TRUE);
	::OutputDebugStringW(wbuf);
	delete [] wbuf;
}
#endif
#endif
/*=========================================================================
	例外情報取得
=========================================================================*/
static char *ExceptionTitle;
static char *ExceptionLogFile;
static char *ExceptionLogInfo;
#define STACKDUMP_SIZE		256
#define MAX_STACKDUMP_SIZE	8192

LONG WINAPI Local_UnhandledExceptionFilter(struct _EXCEPTION_POINTERS *info)
{
	static char			buf[MAX_STACKDUMP_SIZE];

	static HANDLE		hFile;
	static SYSTEMTIME	tm;
	static CONTEXT		*context;
	static DWORD		len, i, j;
	static char			*stack, *esp;

	hFile = ::CreateFileA(ExceptionLogFile, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0);
	::SetFilePointer(hFile, 0, 0, FILE_END);
	::GetLocalTime(&tm);
	context = info->ContextRecord;
#pragma warning(push)
#pragma warning(disable: 4477)
#pragma warning(disable: 4313)
	len = sprintf(buf,
#ifdef _WIN64
		"------ %s -----\r\n"
		" Date        : %d/%02d/%02d %02d:%02d:%02d\r\n"
		" Code/Addr   : %p / %p\r\n"
		" AX/BX/CX/DX : %p / %p / %p / %p\r\n"
		" SI/DI/BP/SP : %p / %p / %p / %p\r\n"
		" 08/09/10/11 : %p / %p / %p / %p\r\n"
		" 12/13/14/15 : %p / %p / %p / %p\r\n"
		"------- ｶﾑﾕｻﾐﾅﾏ｢ -----\r\n"
		, ExceptionTitle
		, tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond
		, info->ExceptionRecord->ExceptionCode, info->ExceptionRecord->ExceptionAddress
		, context->Rax, context->Rbx, context->Rcx, context->Rdx
		, context->Rsi, context->Rdi, context->Rbp, context->Rsp
		, context->R8,  context->R9,  context->R10, context->R11
		, context->R12, context->R13, context->R14, context->R15
#else
		"------ %s -----\r\n"
		" Date        : %d/%02d/%02d %02d:%02d:%02d\r\n"
		" Code/Addr   : %X / %p\r\n"
		" AX/BX/CX/DX : %08x / %08x / %08x / %08x\r\n"
		" SI/DI/BP/SP : %08x / %08x / %08x / %08x\r\n"
		"------- ｶﾑﾕｻﾐﾅﾏ｢ -----\r\n"
		, ExceptionTitle
		, tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond
		, info->ExceptionRecord->ExceptionCode, info->ExceptionRecord->ExceptionAddress
		, context->Eax, context->Ebx, context->Ecx, context->Edx
		, context->Esi, context->Edi, context->Ebp, context->Esp
#endif
		);

	::WriteFile(hFile, buf, len, &len, 0);

#ifdef _WIN64
		esp = (char *)context->Rsp;
#else
		esp = (char *)context->Esp;
#endif

	for (i=0; i < MAX_STACKDUMP_SIZE / STACKDUMP_SIZE; i++) {
		stack = esp + (i * STACKDUMP_SIZE);
		if (::IsBadReadPtr(stack, STACKDUMP_SIZE))
			break;
		len = 0;
		for (j=0; j < STACKDUMP_SIZE / sizeof(DWORD_PTR); j++)
			len += sprintf(buf + len, "%p%s", ((DWORD_PTR *)stack)[j],
							((j+1)%(32/sizeof(DWORD_PTR))) ? " " : "\r\n");
		::WriteFile(hFile, buf, len, &len, 0);
	}
#pragma warning(pop)

	len = sprintf(buf, "------------------------\r\n\r\n");
	::WriteFile(hFile, buf, len, &len, 0);
	::CloseHandle(hFile);

	sprintf(buf, ExceptionLogInfo, ExceptionLogFile);
	::MessageBoxA(0, buf, ExceptionTitle, MB_OK);

	//context->Esi++;
	////::SetErrorMode(SEM_NOALIGNMENTFAULTEXCEPT);
	//return	EXCEPTION_CONTINUE_EXECUTION;
	return	EXCEPTION_EXECUTE_HANDLER;
}

BOOL InstallExceptionFilter(char *title, char *info)
{
	char	buf[MAX_PATH];

	::GetModuleFileNameA(NULL, buf, sizeof(buf));
	strcpy(strrchr(buf, '.'), "_exception.log");
	ExceptionLogFile = strdup(buf);
	ExceptionTitle = strdup(title);
	ExceptionLogInfo = info;

	::SetUnhandledExceptionFilter(&Local_UnhandledExceptionFilter);

	return	TRUE;
}


/*
	nul文字を必ず付与する strncpy
*/
char *strncpyz(char *dest, const char *src, int num)
{
	char	*sv = dest;

	while (num-- > 0)
		if ((*dest++ = *src++) == '\0')
			return	sv;

	if (sv != dest)		// num > 0
		*(dest -1) = 0;
	return	sv;
}

/*
	大文字小文字を無視する strncmp
*/
int strncmpi(const char *str1, const char *str2, int num)
{
	for (int cnt=0; cnt < num; cnt++)
	{
		char	c1 = toupper(str1[cnt]), c2 = toupper(str2[cnt]);

		if (c1 == c2)
		{
			if (c1)
				continue;
			else
				return	0;
		}
		if (c1 > c2)
			return	1;
		else
			return	-1;
	}
	return	0;
}

#ifdef _USE_T_CHARCONV_
/*=========================================================================
	UCS2(W) - ANSI(A) 相互変換
=========================================================================*/
WCHAR *AtoW(const char *src, BOOL noStatic) {
	static	WCHAR	*_wbuf = NULL;

	WCHAR	*wtmp = NULL;
	WCHAR	*&wbuf = noStatic ? wtmp : _wbuf;

	if (wbuf) {
		delete [] wbuf;
		wbuf = NULL;
	}

	int		len;
	if ((len = AtoW(src, NULL, 0)) > 0) {
		wbuf = new WCHAR [len];
		AtoW(src, wbuf, len);
	}
	return	wbuf;
}
#endif

char *strdupNew(const char *_s)
{
	int		len = (int)strlen(_s) + 1;
	char	*s = new char [len];
	memcpy(s, _s, len);
	return	s;
}

WCHAR *wcsdupNew(const WCHAR *_s)
{
	int		len = (int)wcslen(_s) + 1;
	WCHAR	*s = new WCHAR [len];
	memcpy(s, _s, len * sizeof(WCHAR));
	return	s;
}


BOOL TIsWow64()
{
	static BOOL	once = FALSE;
	static BOOL	ret = FALSE;

	if (!once) {
		BOOL (WINAPI *pIsWow64Process)(HANDLE, BOOL *) = (BOOL (WINAPI *)(HANDLE, BOOL *))
				GetProcAddress(::GetModuleHandleA("kernel32"), "IsWow64Process");
		if (pIsWow64Process) {
			pIsWow64Process(::GetCurrentProcess(), &ret);
		}
		once = TRUE;
	}
    return ret;
}

BOOL TIsUserAnAdmin()
{
	static BOOL	once = FALSE;
	static BOOL	(WINAPI *pIsUserAnAdmin)(void);
	static BOOL	ret = FALSE;

	if (!once) {
		pIsUserAnAdmin = (BOOL (WINAPI *)(void))
			GetProcAddress(::GetModuleHandleA("shell32"), "IsUserAnAdmin");
		if (pIsUserAnAdmin) {
			ret = pIsUserAnAdmin();
		}
		once = TRUE;
	}
	return	ret;
}

BOOL TSetThreadLocale(int lcid)
{
	static BOOL	once = FALSE;
	static LANGID (WINAPI *pSetThreadUILanguage)(LANGID LangId);

	if (!once) {
		if (IsWinVista()) {	// ignore if XP
			pSetThreadUILanguage = (LANGID (WINAPI *)(LANGID LangId))
				GetProcAddress(::GetModuleHandleA("kernel32"), "SetThreadUILanguage");
		}
		once = TRUE;
	}

	if (pSetThreadUILanguage) {
		pSetThreadUILanguage(LANGIDFROMLCID(lcid));
	}
	return ::SetThreadLocale(lcid);
}

BOOL TChangeWindowMessageFilter(UINT msg, DWORD flg)
{
	static BOOL	once = FALSE;
	static BOOL	(WINAPI *pChangeWindowMessageFilter)(UINT, DWORD);
	static BOOL	ret = FALSE;

	if (!once) {
		pChangeWindowMessageFilter = (BOOL (WINAPI *)(UINT, DWORD))
			GetProcAddress(::GetModuleHandleA("user32"), "ChangeWindowMessageFilter");
		once = TRUE;
	}

	if (pChangeWindowMessageFilter) {
		ret = pChangeWindowMessageFilter(msg, flg);
	}

	return	ret;
}

