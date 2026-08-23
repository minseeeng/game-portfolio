#pragma once
#include<iostream>


// 데이터를 담는 노드를 표현하는게 필요
// 구조체로 표현 멤버로는 데이터를 나타내는 데이터, next와 prev
// 다음 노드나 이전 노드를 가리켜야 하기에 Node* 자료형으로 통일


class DLinkedList
{
public:
	DLinkedList()
	{
		m_head = nullptr;
		m_tail = nullptr;
	}
	~DLinkedList() { Free(); }
	
	struct Node
	{
		int data = 0;
		Node* next = nullptr;
		Node* prev = nullptr;
	};

	//새로 들어갈 노드를 newNode라고 칭한다
	//맨앞 노드는 항상 head, push_front는 맨앞에다가 데이터를 넣는다 즉, prev가 nullptr
	//만약 head가 없으면 newNode가 head가 됨, tail도 존재하기 때문에 head가 없으면 tail도 newNode를 가리킨다
	//만약 head가 있으면 newNode는 head를  next로 가리키고 head는 prev로 newNode를 가리킨다, 이때 데이터가 있다면 tail은 누군가를 이미 뒤에서 가리키고 있기에 별도 처리x
	//그리고 newNode는 새로운 head로 임명
	void Push_Front(int data)
	{
		Node* newNode = new Node();
		newNode->data = data;

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


	// 새로 들어갈 노드를 newNode라고 칭한다
	// 맨뒤에 있는 노드는 tail인 상태 즉 next가 nullptr이다
	// 만약 tail이 nullptr이라면 새로 들어간 노드가 tail이자 head가 됨
	// 만약 tail이 있다면 newNode는 tail을 prev로 가리키고, tail은 newNode를 next로 가리킴 마지막으로 newNode는 tail이 됨
	// 이때 데이터가 있다면 head는 누군가를 이미 앞에서 가리키고 있기에 별도 처리x
	void Push_Back(int data)
	{
		Node* newNode = new Node();
		newNode->data = data;

		if (m_tail == nullptr)
		{
			m_tail = newNode;
			m_head = newNode;
			return;
		}

		newNode->prev = m_tail;
		m_tail->next = newNode;
		m_tail = newNode;
	}

	
	// 특정 데이터를 찾아서 지워야 한다. 우선 search함수를 통해 해당 data가 있는지 확인, 없으면 종료 지워야 할 노드는 node로 칭한다
	// 만약 리스트가 없으면 즉 node==nullptr이면 종료
	// 지울 노드가 한개뿐이라면 지우고 head랑 tail도 nullptr처리
	// 만약 node==m_head라면 head를 head->next로 만들고 head->prev를 nullptr로 그리고 node는 delete
	// 만약 node==m_tail라면 tail를 tail->prev로 만들고 tail->next를 nullptr로한다 node는 delete
	// 중간노드를 지운다면 node->prev->next를 node->next로 node->next->prev를 node->prev로 그리고 node는 삭제
	void Erase(int data)
	{
		Node* node = Search(data);

		if (node == nullptr)
			return;

		if (node == m_head && node == m_tail)
		{
			delete node;
			m_head = nullptr;
			m_tail = nullptr;
		}
		else if (node == m_head)
		{
			m_head = m_head->next;
			m_head->prev = nullptr;
			delete node;
		}
		else if (node == m_tail)
		{
			m_tail = m_tail->prev;
			m_tail->next = nullptr;
			delete node;
		}
		else
		{
			node->prev->next = node->next;
			node->next->prev = node->prev;
			delete node;
		}
	}

	// 찾고자 하는 data를 가준으로 노드를 반환
	// 현재 위치를 cur이라고 선언하고 cur은 head부터 시작해 다음 노드 다음 노드를 거쳐 데이터에 맞는 노드를 반환해준다
	// 만약 nullptr까지 찾았는데 없으면 아무것도 반환하지 않는다, 또한 head가 nullptr이면 아무것도 반환하지 않는다
	Node* Search(int data)
	{
		Node* cur = m_head;

		if (cur == nullptr)
			return nullptr;

		while (cur != nullptr)
		{
			if (cur->data == data)
				break;

			cur = cur->next;
		}

		if (cur == nullptr)
			return nullptr;
		else
			return cur;
	}

	
	// 현재 연결되어 있는 노드들은 하나하나 해제 시켜야 함
	// 처음부터 해제 시켜야 하기에 현재 위치를 cur로 하고 시작은 head부터, tail의 next는 nullptr이기에 cur이 nullptr이 아닐떄까지 계속해서 순회하며 각 노드를 해제
	// 다음 노드를 미리 저장해놓고 현재 노드를 지운다음 cur에 다음 노드를 대입해서 안전하게 해제한다
	void Free()
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

	void Print_List()
	{
		Node* cur = m_head;
		while (cur != nullptr)
		{
			printf(" %d -> ", cur->data);
			cur = cur->next;
		}
	}
private:
	//list의 시작점은 head
	Node* m_head;
	//list의 끝점은 tail
	Node* m_tail;
};



class SLinkedList
{
public:
	SLinkedList() { m_head = nullptr; }
	~SLinkedList() { Free(); }

	struct Node
	{
		int data = 0;
		Node* next = nullptr;
	};


	//새로 들어갈 노드를 newNode라고 칭한다
	//맨앞 노드는 항상 head, push_front는 맨앞에다가 데이터를 넣는다
	//만약 head가 없으면 newNode가 head가 됨
	//만약 head가 있으면 newNode는 head를 next로 가리킨다
	//그리고 newNode는 새로운 head로 임명
	void Push_Front(int data)
	{
		Node* newNode = new Node();
		newNode->data = data;

		if (m_head == nullptr)
		{
			m_head = newNode;
			return;
		}

		newNode->next = m_head;
		m_head = newNode;
	}


	// 새로 들어갈 노드를 newNode라고 칭한다
	// 맨앞 노드부터 들어가서 존재하는지 확인, 만약 없으면 헤드에 newNode를 넣고 종료
	// 만약 head에 이미 node가 없다면 다음 노드를 확인해 가면서 끝지점을 찾는다, next가 존재한다는건 다음노드가 있다는 의미
	// 만약 next 가 nullptr이면 그 부분이 마지막 노드, 마지막 노드의 next부분에 새노드를 넣고 종료
	void Push_Back(int data)
	{
		Node* newNode = new Node();
		newNode->data = data;

		if (m_head == nullptr)
		{
			m_head = newNode;
			return;
		}

		Node* cur= m_head;
		while (cur->next != nullptr)
		{
			cur = cur->next;
		}
		
		cur->next = newNode;
	}


	// 지워야 할 노드는 node로 칭한다, 특정 데이터를 찾아서 지워야 한다. 우선 search함수를 통해 해당 data가 있는지 확인, 없으면 종료 
	// 만약 리스트가 없으면 즉 node==nullptr이면 종료
	// 지울 노드가 한개뿐이라면 지우고 head nullptr처리
	// 만약 node==m_head라면 head를 head->next로 만들고 node는 delete
	// 만약 노드 사이에 있는 노드를 지워야 한다면, 처음부터 앞노드를 기억할 prev 변수와, head부터 시작할 cur 변수를 생성
	// 반복문 시작시 prev=cur로 하고 cur는 erase를 할 노드를 찾을때까지 반복해서 돈다
	// 만약 노드를 찾았으면 prev는 지울 노드의 전 노드에 있기에 prev->next = cur->next로 이어준다
	// 마지막으로 지울 노드를 제거
	void Erase(int data)
	{
		Node* node = Search(data);

		if (node == nullptr)
			return;

		if (node == m_head && node->next==nullptr)
		{
			delete node;
			m_head = nullptr;
		}
		else if (node == m_head)
		{
			m_head = m_head->next;
			delete node;
		}
		else
		{
			Node* prev = nullptr;
			Node* cur = m_head;
			while (cur != node)
			{
				prev = cur;
				cur = cur->next;
			}
			prev->next = cur->next;
			delete node;
		}
	}

	// 찾고자 하는 data를 가준으로 노드를 반환
	// 현재 위치를 cur이라고 선언하고 cur은 head부터 시작해 다음 노드 다음 노드를 거쳐 데이터에 맞는 노드를 반환해준다
	// 만약 nullptr까지 찾았는데 없으면 아무것도 반환하지 않는다, 또한 head가 nullptr이면 아무것도 반환하지 않는다
	Node* Search(int data)
	{
		Node* cur = m_head;

		if (cur == nullptr)
			return nullptr;

		while (cur != nullptr)
		{
			if (cur->data == data)
				break;

			cur = cur->next;
		}

		if (cur == nullptr)
			return nullptr;
		else
			return cur;
	}


	// 현재 연결되어 있는 노드들은 하나하나 해제 시켜야 함
	// 처음부터 해제 시켜야 하기에 현재 위치를 cur로 하고 시작은 head부터, cur이 nullptr이 아닐떄까지 계속해서 순회하며 각 노드를 해제
	// 다음 노드를 미리 저장해놓고 현재 노드를 지운다음 cur에 다음 노드를 대입해서 안전하게 해제한다
	void Free()
	{
		Node* cur = m_head;
		while (cur != nullptr)
		{
			Node* next = cur->next;
			delete cur;
			cur = next;
		}
		m_head = nullptr;
	}

	void Print_List()
	{
		Node* cur = m_head;
		while (cur != nullptr)
		{
			printf(" %d -> ", cur->data);
			cur = cur->next;
		}
	}
private:
	//list의 시작점은 head
	Node* m_head;
};