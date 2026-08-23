#include "List.h"

DLinkedList::DLinkedList()
{
	m_head = nullptr;
	m_tail = nullptr;
}


DLinkedList::~DLinkedList()
{
	Free(); 
}

void DLinkedList::PushFront(int x, int y)
{
	
	Node* newNode = new Node();
	newNode->x = x;
	newNode->y = y;

	if (m_head == nullptr)
	{
		m_head = newNode;
		m_tail = newNode;
		return;
	}

	newNode->next = m_head;
	m_head->prev = newNode;
	m_head = newNode;
	
}


void DLinkedList::PopBack()
{

	if (m_tail == nullptr)
		return;

	if (m_tail == m_head)
	{
		delete m_tail;
		m_tail = nullptr;
		m_head = nullptr;
		return;
	}

	Node* cur = m_tail;
	m_tail = m_tail->prev;
	delete cur;
	m_tail->next = nullptr;

}


void DLinkedList::Free()
{
	Node* cur = m_head;
	while (cur != nullptr)
	{
		Node* next = cur->next;
		delete cur;
		cur = next;
	}
	m_head = nullptr;
	m_tail = nullptr;
}


bool DLinkedList::Contains(int x, int y)
{
	Node* cur = m_head;
	while (cur != nullptr)
	{
		if (cur->x == x && cur->y == y)
			return true;

		cur = cur->next;
	}
	return false;
}