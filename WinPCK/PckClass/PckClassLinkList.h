#pragma once

template<typename Type>
class LinkList
{
private:
	Type * head;//头指针  
	Type *tail;//尾指针
	Type* AllocMemory();
public:
	LinkList();
	~LinkList();

	BOOL insertNext();
	Type *last();
	Type *first();

};

template<typename Type>
LinkList<Type>::LinkList()
{
	head = AllocMemory();
	tail = head;
	head->next = NULL;
}

template<typename Type>
Type* LinkList<Type>::AllocMemory()
{
	Type*		lpMallocNode;

	if(NULL == (lpMallocNode = (Type*)malloc(sizeof(Type)))) {
		return lpMallocNode;
	}
	//初始化内存
	memset(lpMallocNode, 0, sizeof(Type));

	return lpMallocNode;

}

template<typename Type>
BOOL LinkList<Type>::insertNext()
{
	tail->next = AllocMemory();
	tail = tail->next;
	tail->next = NULL;
	return (NULL != tail);
}

template<typename Type>
Type* LinkList<Type>::last()
{
	return tail;
}

template<typename Type>
Type* LinkList<Type>::first()
{
	return head;
}

template<typename Type>
LinkList<Type>::~LinkList()
{
	Type* Fileinfo;

	while(head != NULL) {
		Fileinfo = head->next;
		free(head);
		head = Fileinfo;
	}
	head = NULL;
}