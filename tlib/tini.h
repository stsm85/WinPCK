
#ifndef TINI_H
#define TINI_H

#ifdef _USE_T_INI_

#include "tlist.h"

#define MAX_KEYARRAY	30


class TIniKey : public TListObj {
protected:
	char	*key;	// null means comment line
	char	*val;

public:
	TIniKey(const char *_key=NULL, const char *_val=NULL) {
		key = val = NULL; if (_key || _val) Set(_key, _val);
	}
	~TIniKey() { free(key); free(val); }

	void Set(const char *_key=NULL, const char *_val=NULL) {
		if (_key) { free(key); key=strdup(_key); }
		if (_val) { free(val); val=strdup(_val); }
	}
	const char *Key() { return key; }
	const char *Val() { return val; }
};

class TIniSection : public TListObj, public TList {
protected:
	char	*name;

public:
	TIniSection() { name = NULL; }
	~TIniSection() {
		free(name);
		for (TIniKey *key; (key = (TIniKey *)TopObj()); ) {
			DelObj(key);
			delete key;
		}
	}

	void Set(const char *_name=NULL) {
		if (_name) { free(name); name=strdup(_name); }
	}
	TIniKey *SearchKey(const char *key_name) {
		for (TIniKey *key = (TIniKey *)TopObj(); key; key = (TIniKey *)NextObj(key)) {
			if (key->Key() && strcmpi(key->Key(), key_name) == 0) return key;
		}
		return	NULL;
	}
	BOOL AddKey(const char *key_name, const char *val) {
		TIniKey	*key = key_name ? SearchKey(key_name) : NULL;
		if (!key) {
			key = new TIniKey(key_name, val);
			AddObj(key);
		}
		else {
			key->Set(NULL, val);
		}
		return	TRUE;
	}
	BOOL DelKey(const char *key_name) {
		TIniKey	*key = SearchKey(key_name);
		if (!key) return FALSE;
		DelObj(key);
		delete key;
		return	TRUE;
	}
	const char *Name() { return name; }
};

class TInifile: public TList {
protected:
	char		*ini_file;
	TIniSection	*cur_sec;
	TIniSection	*root_sec;
	FILETIME	ini_ft;
	int			ini_size;
	HANDLE		hMutex;

	BOOL Strip(const char *s, char *d=NULL, const char *strip_chars=" \t\r\n",
		const char *quote_chars="\"\"");
	BOOL Parse(const char *buf, BOOL *is_section, char *name, char *val);
	BOOL GetFileInfo(LPTSTR fname, FILETIME *ft, int *size);///
	TIniSection *SearchSection(const char *section);
	BOOL WriteIni();
#ifdef _USE_T_CRYPT_
	BOOL Lock();
	void UnLock();
#endif

public:
	TInifile(const char *ini_name=NULL);
	~TInifile();
	void Init(const char *ini_name=NULL);
	void UnInit();
	void SetSection(const char *section);
	BOOL StartUpdate();
	BOOL EndUpdate();
	BOOL SetStr(const char *key, const char *val);
	DWORD GetStr(const char *key, char *val, int max_size, const char *default_val="");
	BOOL SetInt(const char *key, int val);
	BOOL SetInt64(const char *key, __int64 val);
	BOOL DelSection(const char *section);
	BOOL DelKey(const char *key);
	int GetInt(const char *key, int default_val=-1);
	__int64 GetInt64(const char *key, __int64 default_val=-1);
	const char *GetIniFileName(void) { return	ini_file; }
};

#endif

#endif