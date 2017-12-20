
#ifndef TREGISTRY_H
#define TREGISTRY_H

#define MAX_KEYARRAY	30

#ifdef _USE_T_REG_

class TRegistry {
protected:
	HKEY	topKey;
	int		openCnt;
	//StrMode	strMode;
	HKEY	hKey[MAX_KEYARRAY];

public:
	TRegistry(LPCTSTR company, LPTSTR appName=NULL/*, StrMode mode=BY_UTF8*/);
	TRegistry(HKEY top_key/*, StrMode mode=BY_UTF8*/);
	~TRegistry();

	void	ChangeTopKey(HKEY topKey);

	BOOL	ChangeApp(LPCTSTR company, LPTSTR appName=NULL);

	BOOL	OpenKey(LPCTSTR subKey, BOOL createFlg=FALSE);

	BOOL	CreateKey(LPCTSTR subKey) { return OpenKey(subKey, TRUE); }

	BOOL	CloseKey(void);

	BOOL	GetInt(LPCTSTR key, int *val);

	BOOL	SetInt(LPCTSTR key, int val);

	BOOL	GetLong(LPCTSTR key, long *val);

	BOOL	SetLong(LPCTSTR key, long val);

	BOOL	GetStr(LPCTSTR key, LPTSTR str, int size_byte);

	BOOL	SetStr(LPCTSTR key, LPCTSTR str);

	BOOL	GetByte(LPCTSTR key, BYTE *data, int *size);

	BOOL	SetByte(LPCTSTR key, const BYTE *data, int size);

	BOOL	DeleteKey(LPCTSTR str);

	BOOL	DeleteValue(LPCTSTR str);

	BOOL	EnumKey(DWORD cnt, LPTSTR buf, int size);

	BOOL	EnumValue(DWORD cnt, LPTSTR buf, int size, DWORD *type=NULL);

	BOOL	DeleteChildTree(LPCTSTR subkey=NULL);

};

#endif

#endif