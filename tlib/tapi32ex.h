/* @(#)Copyright (C) 1996-2010 H.Shirouzu		tapi32ex.h	Ver0.99 */
/* ========================================================================
	Project  Name			: Win32 Lightweight  Class Library Test
	Module Name				: Main Header
	Create					: 2005-04-10(Sun)
	Update					: 2010-05-09(Sun)
	Copyright				: H.Shirouzu
	Reference				: 
	======================================================================== */

#ifndef TAPI32EX_H
#define TAPI32EX_H

#if _USE_T_CHARCONV_
#define CP_UTF8                   65001       // UTF-8 translation
inline int AtoW(const char *src, WCHAR *dst, int bufsize, int max_len=-1) {
	return	::MultiByteToWideChar(CP_ACP, 0, src, max_len, dst, bufsize);
}
inline int WtoA(const WCHAR *src, char *dst, int bufsize, int max_len=-1) {
	return	::WideCharToMultiByte(CP_ACP, 0, src, max_len, dst, bufsize, 0, 0);
}


WCHAR *AtoW(const char *src, BOOL noStatic=FALSE);
#endif


#if _MSC_VER < 1200

#define MIDL_INTERFACE(x) interface

#ifndef __IShellLinkW_INTERFACE_DEFINED__
#define __IShellLinkW_INTERFACE_DEFINED__

EXTERN_C const IID IID_IShellLinkW;

MIDL_INTERFACE("000214F9-0000-0000-C000-000000000046")
IShellLinkW : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE
    	GetPath(LPWSTR pszFile, int cch, WIN32_FIND_DATAW *pfd, DWORD fFlags) = 0;
};

#endif 	/* __IShellLinkW_INTERFACE_DEFINED__ */

#define EN_LINK					0x070b
typedef struct _enlink
{
    NMHDR nmhdr;
    UINT msg;
    WPARAM wParam;
    LPARAM lParam;
    CHARRANGE chrg;
} ENLINK;

#define EM_GETTEXTEX			(WM_USER + 94)
typedef struct _gettextex
{
	DWORD	cb;				// Count of bytes in the string				
	DWORD	flags;			// Flags (see the GT_XXX defines			
	UINT	codepage;		// Code page for translation (CP_ACP for sys default,
						    //  1200 for Unicode, -1 for control default)	
	LPCSTR	lpDefaultChar;	// Replacement for unmappable chars			
	LPBOOL	lpUsedDefChar;	// Pointer to flag set when def char used	
} GETTEXTEX;

#endif

#ifndef EM_SETTEXTEX
#define EM_SETTEXTEX			(WM_USER + 97)
// EM_SETTEXTEX info; this struct is passed in the wparam of the message 
typedef struct _settextex
{
	DWORD	flags;			// Flags (see the ST_XXX defines)			
	UINT	codepage;		// Code page for translation (CP_ACP for sys default,
						    //  1200 for Unicode, -1 for control default)	
} SETTEXTEX;

// Flags for the SETEXTEX data structure 
#define ST_DEFAULT		0
#define ST_KEEPUNDO		1
#define ST_SELECTION	2
#define ST_NEWCHARS 	4

// Flags for the GETEXTEX data structure 
#define GT_DEFAULT		0
#define GT_USECRLF		1
#define GT_SELECTION	2
#define GT_RAWTEXT		4
#define GT_NOHIDDENTEXT	8

#define EM_SETLANGOPTIONS		(WM_USER + 120)
#define EM_GETLANGOPTIONS		(WM_USER + 121)
#define IMF_DUALFONT			0x0080
#endif

#if _MSC_VER < 1200

typedef struct _browseinfoA {
    HWND        hwndOwner;
    LPCITEMIDLIST pidlRoot;
    LPSTR        pszDisplayName;        // Return display name of item selected.
    LPCSTR       lpszTitle;                     // text to go in the banner over the tree.
    UINT         ulFlags;                       // Flags that control the return stuff
    BFFCALLBACK  lpfn;
    LPARAM       lParam;                        // extra info that's passed back in callbacks
    int          iImage;                        // output var: where to return the Image index.
} BROWSEINFOA, *PBROWSEINFOA, *LPBROWSEINFOA;

typedef struct _browseinfoW {
    HWND        hwndOwner;
    LPCITEMIDLIST pidlRoot;
    LPWSTR       pszDisplayName;        // Return display name of item selected.
    LPCWSTR      lpszTitle;                     // text to go in the banner over the tree.
    UINT         ulFlags;                       // Flags that control the return stuff
    BFFCALLBACK  lpfn;
    LPARAM       lParam;                        // extra info that's passed back in callbacks
    int          iImage;                        // output var: where to return the Image index.
} BROWSEINFOW, *PBROWSEINFOW, *LPBROWSEINFOW;

#endif

#define BFFM_SETSELECTIONW (WM_USER + 103)
#define LVM_INSERTITEMW (LVM_FIRST + 77)
DEFINE_GUID(IID_IShellLinkW, 0x000214F9, \
	0x0000, 0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);

#ifndef BIF_NEWDIALOGSTYLE

#define BIF_EDITBOX				0x0010
#define BIF_VALIDATE			0x0020
#define BIF_NEWDIALOGSTYLE		0x0040
#define BIF_USENEWUI			(BIF_NEWDIALOGSTYLE | BIF_EDITBOX)

#define BIF_BROWSEINCLUDEURLS	0x0080
#define BIF_UAHINT				0x0100
#define BIF_NONEWFOLDERBUTTON	0x0200
#define BIF_NOTRANSLATETARGETS	0x0400

#define BIF_BROWSEINCLUDEFILES	0x4000
#define BIF_SHAREABLE			0x8000

#endif

#ifdef _USE_T_CRYPT_

// CryptoAPI for VC4
#ifndef MS_DEF_PROV
typedef unsigned long HCRYPTPROV;
typedef unsigned long HCRYPTKEY;
typedef unsigned long HCRYPTHASH;
typedef unsigned int ALG_ID;
#define ALG_TYPE_RSA			(2 << 9)
#define ALG_TYPE_BLOCK			(3 << 9)
#define ALG_CLASS_DATA_ENCRYPT	(3 << 13)
#define ALG_CLASS_HASH			(4 << 13)
#define ALG_CLASS_KEY_EXCHANGE	(5 << 13)
#define ALG_SID_RSA_ANY			0
#define ALG_TYPE_ANY			0
#define ALG_SID_RC2				2
#define ALG_SID_MD5				3
#define ALG_SID_SHA				4
#define CALG_RSA_KEYX			(ALG_CLASS_KEY_EXCHANGE|ALG_TYPE_RSA|ALG_SID_RSA_ANY)
#define CALG_RC2				(ALG_CLASS_DATA_ENCRYPT|ALG_TYPE_BLOCK|ALG_SID_RC2)
#define CALG_MD5				(ALG_CLASS_HASH|ALG_TYPE_ANY|ALG_SID_MD5)
#define CALG_SHA				(ALG_CLASS_HASH|ALG_TYPE_ANY|ALG_SID_SHA)
#define HP_ALGID 1
#define HP_HASHVAL 2
#define HP_HASHSIZE 4
#define CRYPT_EXPORTABLE		0x00000001
#define PROV_RSA_FULL			1
#define PROV_DSS				3
#define MS_DEF_PROV				"Microsoft Base Cryptographic Provider v1.0"
#define MS_ENHANCED_PROV		"Microsoft Enhanced Cryptographic Provider v1.0"
#define MS_DEF_DSS_PROV			"Microsoft Base DSS Cryptographic Provider"
#define CUR_BLOB_VERSION		0x02
#define SIMPLEBLOB				0x1
#define PUBLICKEYBLOB			0x6
#define PRIVATEKEYBLOB          0x7
#define CRYPT_NEWKEYSET			0x00000008
#define CRYPT_DELETEKEYSET      0x00000010
#define CRYPT_MACHINE_KEYSET    0x00000020
#define AT_KEYEXCHANGE			1
#define AT_SIGNATURE			2
#define KP_EFFECTIVE_KEYLEN		19	// for CryptSetKeyParam
#ifndef NTE_BAD_KEY
#define NTE_BAD_KEY				0x80090003L
#endif

typedef struct _CRYPTOAPI_BLOB {
	DWORD   cbData;
	BYTE    *pbData;
} DATA_BLOB, *PDATA_BLOB;

#endif

#ifndef CRYPTPROTECT_VERIFY_PROTECTION
typedef struct _CRYPTPROTECT_PROMPTSTRUCT {
	DWORD cbSize;
	DWORD dwPromptFlags;
	HWND hwndApp;
	LPCWSTR szPrompt;
} CRYPTPROTECT_PROMPTSTRUCT, *PCRYPTPROTECT_PROMPTSTRUCT;

#define CRYPTPROTECT_UI_FORBIDDEN		0x1
#define CRYPTPROTECT_LOCAL_MACHINE		0x4
#define CRYPTPROTECT_CRED_SYNC			0x8
#define CRYPTPROTECT_AUDIT				0x10
#define CRYPTPROTECT_VERIFY_PROTECTION	0x40
#endif

extern BOOL (WINAPI *pCryptAcquireContext)(HCRYPTPROV *, LPCTSTR, LPCTSTR, DWORD, DWORD);//////
extern BOOL (WINAPI *pCryptDestroyKey)(HCRYPTKEY);
extern BOOL (WINAPI *pCryptGetKeyParam)(HCRYPTKEY, DWORD, BYTE *, DWORD *, DWORD);
extern BOOL (WINAPI *pCryptSetKeyParam)(HCRYPTKEY, DWORD, BYTE *, DWORD);
extern BOOL (WINAPI *pCryptExportKey)(HCRYPTKEY, HCRYPTKEY, DWORD, DWORD, BYTE *, DWORD *);
extern BOOL (WINAPI *pCryptGetUserKey)(HCRYPTPROV, DWORD, HCRYPTKEY *);
extern BOOL (WINAPI *pCryptEncrypt)(HCRYPTKEY, HCRYPTHASH, BOOL, DWORD, BYTE *, DWORD *, DWORD);
extern BOOL (WINAPI *pCryptGenKey)(HCRYPTPROV, ALG_ID, DWORD, HCRYPTKEY *);
extern BOOL (WINAPI *pCryptGenRandom)(HCRYPTPROV, DWORD, BYTE *);
extern BOOL (WINAPI *pCryptImportKey)
				(HCRYPTPROV, CONST BYTE *, DWORD, HCRYPTKEY, DWORD, HCRYPTKEY *);
extern BOOL (WINAPI *pCryptDecrypt)(HCRYPTKEY, HCRYPTHASH, BOOL, DWORD, BYTE *, DWORD *);
extern BOOL (WINAPI *pCryptCreateHash)(HCRYPTPROV, ALG_ID, HCRYPTKEY, DWORD, HCRYPTHASH *);
extern BOOL (WINAPI *pCryptHashData)(HCRYPTHASH, BYTE *, DWORD, DWORD);
extern BOOL (WINAPI *pCryptSignHash)(HCRYPTHASH, DWORD, LPCTSTR, DWORD, BYTE *, DWORD *);//////
extern BOOL (WINAPI *pCryptDestroyHash)(HCRYPTHASH);
extern BOOL (WINAPI *pCryptGetHashParam)(HCRYPTHASH, DWORD, BYTE *, DWORD *, DWORD);
extern BOOL (WINAPI *pCryptSetHashParam)(HCRYPTHASH, DWORD, const BYTE *, DWORD);

//extern BOOL (WINAPI *pCryptVerifySignature)
//				(HCRYPTHASH, CONST BYTE *, DWORD, HCRYPTKEY, LPCTSTR, DWORD);//////
//extern BOOL (WINAPI *pCryptReleaseContext)(HCRYPTPROV, DWORD);
//extern BOOL (WINAPI *pCryptProtectData)
//				(DATA_BLOB* pDataIn, LPCWSTR szDataDescr, DATA_BLOB* pOptionalEntropy,
//				 PVOID pvReserved, CRYPTPROTECT_PROMPTSTRUCT* pPromptStruct, DWORD dwFlags,
//				 DATA_BLOB* pDataOut);
//extern BOOL (WINAPI *pCryptUnprotectData)
//				(DATA_BLOB* pDataIn, LPWSTR* ppszDataDescr, DATA_BLOB* pOptionalEntropy,
//				 PVOID pvReserved, CRYPTPROTECT_PROMPTSTRUCT* pPromptStruct, DWORD dwFlags,
//				 DATA_BLOB* pDataOut);

#define SHA1_SIZE 20
#define MD5_SIZE  16


/* NTDLL */
//typedef LONG NTSTATUS, *PNTSTATUS;

//typedef struct _IO_STATUS_BLOCK {
//  union {
//    NTSTATUS Status;
//    PVOID    Pointer;
//  };
//  unsigned long *Information;
//} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;
//
//typedef enum _FILE_INFORMATION_CLASS {
//  FileDirectoryInformation = 1,
//  FileFullDirectoryInformation,
//  FileBothDirectoryInformation,
//  FileBasicInformation,
//  FileStandardInformation,
//  FileInternalInformation,
//  FileEaInformation,
//  FileAccessInformation,
//  FileNameInformation,
//  FileRenameInformation,
//  FileLinkInformation,
//  FileNamesInformation,
//  FileDispositionInformation,
//  FilePositionInformation,
//  FileFullEaInformation,
//  FileModeInformation,
//  FileAlignmentInformation,
//  FileAllInformation,
//  FileAllocationInformation,
//  FileEndOfFileInformation,
//  FileAlternateNameInformation,
//  FileStreamInformation,
//  FilePipeInformation,
//  FilePipeLocalInformation,
//  FilePipeRemoteInformation,
//  FileMailslotQueryInformation,
//  FileMailslotSetInformation,
//  FileCompressionInformation,
//  FileObjectIdInformation,
//  FileCompletionInformation,
//  FileMoveClusterInformation,
//  FileQuotaInformation,
//  FileReparsePointInformation,
//  FileNetworkOpenInformation,
//  FileAttributeTagInformation,
//  FileTrackingInformation,
//  FileIdBothDirectoryInformation,
//  FileIdFullDirectoryInformation,
//  FileValidDataLengthInformation,
//  FileShortNameInformation,
//  FileIoCompletionNotificationInformation,
//  FileIoStatusBlockRangeInformation,
//  FileIoPriorityHintInformation,
//  FileSfioReserveInformation,
//  FileSfioVolumeInformation,
//  FileHardLinkInformation,
//  FileProcessIdsUsingFileInformation,
//  FileNormalizedNameInformation,
//  FileNetworkPhysicalNameInformation,
//  FileIdGlobalTxDirectoryInformation,
//  FileIsRemoteDeviceInformation,
//  FileAttributeCacheInformation,
//  FileNumaNodeInformation,
//  FileStandardLinkInformation,
//  FileRemoteProtocolInformation,
//  FileMaximumInformation 
//} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;
//
//#define BACKUP_SPARSE_BLOCK 0x00000009

//typedef struct _FILE_STREAM_INFORMATION {
//  ULONG         NextEntryOffset;
//  ULONG         StreamNameLength;
//  LARGE_INTEGER StreamSize;
//  LARGE_INTEGER StreamAllocationSize;
//  WCHAR         StreamName[1];
//} FILE_STREAM_INFORMATION, *PFILE_STREAM_INFORMATION;
//
//extern NTSTATUS (WINAPI *pNtQueryInformationFile)(HANDLE FileHandle,
//				PIO_STATUS_BLOCK IoStatusBlock, PVOID FileInformation, ULONG Length,
//				FILE_INFORMATION_CLASS FileInformationClass);

// ListView extended define for VC4
#ifndef LVM_SETEXTENDEDLISTVIEWSTYLE
#define LVM_SETEXTENDEDLISTVIEWSTYLE	(LVM_FIRST + 54)
#define LVM_GETEXTENDEDLISTVIEWSTYLE	(LVM_FIRST + 55)
#define LVM_SETCOLUMNORDERARRAY			(LVM_FIRST + 58)
#define LVM_GETCOLUMNORDERARRAY			(LVM_FIRST + 59)
#define LVS_EX_GRIDLINES				0x00000001
#define LVS_EX_HEADERDRAGDROP			0x00000010
#define LVS_EX_FULLROWSELECT			0x00000020
#define LVS_SHOWSELALWAYS				0x0008
#define LVM_GETHEADER					0x101F
#define EM_AUTOURLDETECT				(WM_USER + 91)
#endif

// ListView extended define for VC4 & VC5
#ifndef LVM_SETSELECTIONMARK
#define LVM_SETSELECTIONMARK			(LVM_FIRST + 67)
#define LVN_GETINFOTIPW					(LVN_FIRST-58)
#endif

#if _MSC_VER <= 1200
#define LVN_ENDSCROLL					(LVN_FIRST-81)
#endif

u_int MakeHash(const void *data, int size, DWORD iv=0);

class TDigest {
protected:
	HCRYPTPROV	hProv;
	HCRYPTHASH	hHash;
	_int64		updateSize;

public:
	enum Type { SHA1, MD5 /*, SHA1_LOCAL */ } type;

	TDigest();
	~TDigest();
	BOOL Init(Type _type=SHA1);
	BOOL Reset();
	BOOL Update(void *data, int size);
	BOOL GetVal(void *data);
	int  GetDigestSize() { return type == MD5 ? MD5_SIZE : SHA1_SIZE; }
	void GetEmptyVal(void *data);
};

BOOL TLibInit_AdvAPI32();
//BOOL TLibInit_Crypt32();
//BOOL TLibInit_Ntdll();
BOOL TGenRandom(void *buf, int len);

#endif
#endif
