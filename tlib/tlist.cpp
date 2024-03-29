
/* ========================================================================
	Project  Name			: Win32 Lightweight  Class Library Test
	Module Name				: List Class
	Create					: 1996-06-01(Sat)
	Update					: 2009-03-09(Mon)
	Copyright				: H.Shirouzu
	Reference				: 
	======================================================================== */
#include "tlibpch.h"
#include "tlib.h"
#ifdef _USE_T_LIST_
static char *tlist_id =
"@(#)Copyright (C) 1996-2009 H.Shirouzu		tlist.cpp	Ver0.97";

#include "tlist.h"
/*
	TList class
*/

TList::TList(void)
{
	Init();
}

void TList::Init(void)
{
	top.prior = top.next = &top;
}

void TList::AddObj(TListObj * obj)
{
	obj->prior = top.prior;
	obj->next = &top;
	top.prior->next = obj;
	top.prior = obj;
}

void TList::DelObj(TListObj * obj)
{
//	if (!obj->next || !obj->prior || obj->next != &top && obj->prior != &top) {
//		Debug("DelObj(%p) (%p/%p)\n", obj, obj->next, obj->prior);
//	}
	if (obj->next) {
		obj->next->prior = obj->prior;
	}
	if (obj->prior) {
		obj->prior->next = obj->next;
	}
	obj->next = obj->prior = NULL;
}

TListObj* TList::TopObj(void)
{
//	if (top.next != &top && top.next->next != &top && top.next->prior != &top) {
//		Debug("TopObj(%p) \n", top.next);
//	}
	return	top.next == &top ? NULL : top.next;
}

TListObj* TList::NextObj(TListObj *obj)
{
	return	obj->next == &top ? NULL : obj->next;
}

void TList::MoveList(TList *from_list)
{
	if (from_list->top.next != &from_list->top) {	// from_list is not empty
		if (top.next == &top) {	// empty
			top = from_list->top;
			top.next->prior = top.prior->next = &top;
		}
		else {
			top.prior->next = from_list->top.next;
			from_list->top.next->prior = top.prior;
			from_list->top.prior->next = &top;
			top.prior = from_list->top.prior;
		}
		from_list->Init();
	}
}

/*
	TRecycleList class
*/
TRecycleList::TRecycleList(int init_cnt, int size)
{
	data = new char [init_cnt * size];
	memset(data, 0, init_cnt * size);

	for (int cnt=0; cnt < init_cnt; cnt++) {
		TListObj *obj = (TListObj *)(data + cnt * size);
		list[FREE_LIST].AddObj(obj);
	}
}

TRecycleList::~TRecycleList()
{
	delete [] data;
}

TListObj *TRecycleList::GetObj(int list_type)
{
	TListObj	*obj = list[list_type].TopObj();

	if (obj)
		list[list_type].DelObj(obj);

	return	obj;
}

void TRecycleList::PutObj(int list_type, TListObj *obj)
{
	list[list_type].AddObj(obj);
}

#endif