#include <memory>
#include "AllocMemPool.h"
#include <assert.h>

CAllocMemPool::CAllocMemPool(size_t _PoolSize) :
	m_EachPoolSize(_PoolSize)
{
	m_pFirstNode = new_Node();
}

CAllocMemPool::~CAllocMemPool()
{
	Node* pNode = m_pFirstNode;
	Node* pNextNode = nullptr;
	// 顺序释放
	while (pNode)
	{
		pNextNode = pNode->next;
		free(pNode);
		pNode = pNextNode;
	}
}

// 申请节点
CAllocMemPool::Node* CAllocMemPool::new_Node()
{
	Node* pointer = reinterpret_cast<Node*>(malloc(m_EachPoolSize));
	//pointer->Node::Node();
	memset(pointer, 0, m_EachPoolSize);

	return pointer;
}


void* CAllocMemPool::Alloc(size_t size, uint32_t align) {
	if (size > (m_EachPoolSize - sizeof(Node))) {
#ifdef _DEBUG
		assert(!"<AllocOnlyMPool<EachPoolSize>::Alloc>:: Alloc too big");
#endif
		return nullptr;
	}
	// 获取空闲位置
	auto* now_pos = m_pFirstNode->buffer + m_pFirstNode->allocated;
	// 获取对齐后的位置
	auto aligned = (reinterpret_cast<size_t>(now_pos)& (align - 1));
	if (aligned) aligned = align - aligned;
	now_pos += aligned;
	// 增加计数
	m_pFirstNode->allocated += size + aligned;
	// 检查是否溢出
	if (m_pFirstNode->allocated > (m_EachPoolSize - sizeof(Node))) {
		Node* node = new_Node();
		if (!node) return nullptr;
		node->next = m_pFirstNode;
		m_pFirstNode = node;
		// 递归(仅一次)
		return Alloc(size, align);
	}
	// 记录上次释放位置
	m_pFirstNode->last_allocated = now_pos;
	return now_pos;
}

// 释放内存 
void CAllocMemPool::Free(void* address) {
	// 上次申请的就这样了
	if (address && m_pFirstNode->last_allocated == address) {
		m_pFirstNode->allocated =
			(m_pFirstNode->last_allocated - m_pFirstNode->buffer);
		m_pFirstNode->last_allocated = nullptr;
	}
}

