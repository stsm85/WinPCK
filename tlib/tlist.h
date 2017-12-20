
#ifndef TLIST_H
#define TLIST_H

#ifdef _USE_T_INI_
#ifndef _USE_T_LIST_
#define _USE_T_LIST_
#endif
#endif

#ifdef _USE_T_LIST_

struct TListObj {
	TListObj *prior, *next;
};

class TList {
protected:
	TListObj	top;

public:
	TList(void);
	void		Init(void);
	void		AddObj(TListObj *obj);
	void		DelObj(TListObj *obj);
	TListObj	*TopObj(void);
	TListObj	*NextObj(TListObj *obj);
	BOOL		IsEmpty() { return	top.next == &top; }
	void		MoveList(TList *from_list);
};

#define FREE_LIST	0
#define USED_LIST	1
#define	RLIST_MAX	2
class TRecycleList {
protected:
	char	*data;
	TList	list[RLIST_MAX];

public:
	TRecycleList(int init_cnt, int size);
	~TRecycleList();
	TListObj *GetObj(int list_type);
	void PutObj(int list_type, TListObj *obj);
};

#endif
#endif