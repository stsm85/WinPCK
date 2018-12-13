#pragma once

/*
source code from:
https://blog.csdn.net/dustpg/article/details/38756241
*/

class CAllocMemPool
{
	// 节点
	struct Node {
		// 后节点
		Node*               next = nullptr;
		// 已分配数量
		size_t              allocated = 0;
		// 上次分配位置
		BYTE*               last_allocated = nullptr;
		// 缓冲区
		BYTE                buffer[0];
	};

public:
	CAllocMemPool(size_t _PoolSize);
	~CAllocMemPool();

	// 申请内存
	void*               Alloc(size_t size, UINT32 align = sizeof(size_t));
	// 释放内存
	void                Free(void* address);

private:

	// 申请节点
	__forceinline Node* new_Node();
	// 首节点
	Node*	m_pFirstNode;

	size_t	m_EachPoolSize;
};

