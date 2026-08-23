#pragma once


class DLinkedList
{
public:
	DLinkedList();
	~DLinkedList();

	struct Node
	{
		int x;
		int y;
		Node* next = nullptr;
		Node* prev = nullptr;
	};

	void PushFront(int x, int y);
	void PopBack();
	void Free();
	bool Contains(int x, int y);
	const Node* GetHead() const { return m_head; }
	int GetHeadX() const { return m_head->x; }
	int GetHeadY() const { return m_head->y; }
private:
	Node* m_head;
	Node* m_tail;
};
