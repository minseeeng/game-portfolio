#pragma once
#include<iostream>
#include<string>

// strlen 함수를 직접 작성하시오
// 1. strcpy, strcat, strcmp
// 4. string




// 문자열의 길이 (이 문자열)
//int Strlen(char* start)
//{
//	int cnt = 0;
//	while (*start != '\0')
//	{
//		cnt++;
//		*start++;
//	}
//
//	return cnt;
//}


//문자열의 길이를 재야한다
//문자를 받아올 매개변수가 필요
//매개변수를 가리키는 또 하나의 임시 변수가 필요하다
//임시변수로 매개변수의 첫번째 문자를 가리킨 상태부터 문자열 끝을 만날때까지 증가
//널을 만나면 루프가 끝나고, 임시변수의 위치에서 매개변수 시작주소 위치 자리를 뺀 값을 반환
int Strlen(const char* text)
{
	const char* temp = text;

	while (*temp != '\0')
	{
		++temp;
	}

	return temp - text;
}



// 두개의 문자열이 존재
// 오른쪽의 문자열을 왼쪽 문자열로 복사해야함
// 오른쪽 문자열이 널문자를 만날때까지 복사를 진행
// 첫번째 자리부터 옮기고 자리를 각각 하나씩 증가(오른쪽 문자열 부터 증가해야함)
// 오른쪽 문자열이 널을 만나면 루프가 종료됨
// 왼쪽 문자열에 문자만 옮겨진 상태이기에 널문자를 삽입
// 왼쪽 문자열을 반환
char Strcpy(char* dest, const char* src)
{
	while (*src != '\0')
	{
		*dest = *src;
		src++;
		dest++;
	}

	*dest = '\0';

	return *dest;
}

// 두개의 문자열이 존재
// 왼쪽의 문자열에 오른쪽 문자열을 붙여야함
// 왼쪽 문자열의 끝에 오른쪽 문자열을 붙여야 하기 때문에 왼쪽 문자열을 널 문자 만날때까지 루프반복
// 왼쪽 문자가 널 문자 만나고 루프 나오면 오른쪽 문자열을 왼쪽 문자열 널문자 위치부터 하나씩 대입
// 오른쪽 문자가 널문자를 만나면 다 대입햇다는것으로 인지하고 탈출
// 이 모든 과정들이 있기전 왼쪽 문자열을 가리키던 임시변수 값을 반환 (그러면 모두가 이어져있기에 처음부터 끝까지 반환 가능)
char* Strcat(char * dest, const char* src)
{
	char* target = dest;

	while (*dest != '\0')
	{
		dest++;
	}
	
	while (*src != '\0')
	{
		*dest = *src;
		dest++;
		src++;
	}

	*dest = '\0';

	return target;
}



// 두개의 문자열이 있음, 왼쪽과 오른쪽의 문자열을 비교
// 각각 첫번째 주소부터 시작해 각 문자가 같다면 하나씩 증가
// 만약 둘중에 하나라도 널문자를 만나면 루프종료, 두 문자가 다르다면 루프 종료
// 루프를 빠져나온다면 각 위치를 비교해서 아스키 코드 값을 보고 분기에 따라 결과 반환
int Strcmp(const char* str1, const char* str2)
{
	while (*str1 == *str2 && *str1!='\0')
	{
		str1++;
		str2++;
	}

	if (*str1 == *str2)
		return 0;
	else if (*str1 > *str2)
		return 1;
	else
		return - 1;
}

//문자열은 무엇을 하고 뭐가 필요한가
//해당 문자열의 크기, 저장공간의 크기, 해당 문자열을 가리키는 변수

class String
{
public:
	// 빈 문자열로 초기화된다면, 아래 멤버변수처럼 아무것도 가리키지 않거나, _size및 _capacity가 책정도지 않은 상태
	String(){}

	// 문자열로 초기화하며 생성될 때 해당 함수가 실행
	// 선언된 문자열 길이만큼 _size 책정
	//_data 멤버변수를 통해 힙 할당 선언, 널문자를 포함해야 하기에 기존 크기 보다 + 1
	// 이미 공간이 할당된 _data에 들어온 문자열 옮기기
	String(const char* src)
	{
		_size = Strlen(src);
		_data = new char[_size + 1];
		Strcpy(_data, src);
	}

	//선언된 _data들이 다 안전하게 해제할수있도록 소멸자에서 힙할당 해제
	~String() { delete[] _data; }

	
	// 복사 생성자 함수 생성, 같은 힙에 주소 공유를 방지
	// other의 사이즈 및, 메모리를 따로 할당시킨다
	// 내용만 베껴서 새로 할당한 공간에 데이터를 복사
	String(const String& other)
	{
		_size = other._size;
		_data = new char[_size + 1];
		Strcpy(_data, other._data);
	}

	// 이미 각각 메모리를 들고 있는 상태에서 대입 연산자가 실행될때 나오는 함수
	// 만약 자기자신이 있다면, 그대로 반환
	// 기존 메모리를 먼저 해제한다, 왜냐하면 오른쪽 연산자는 이미 값과 메모리를 가지고 있기 때문
	// 복사생성자와 같이 사이즈, 데이터 크기를 책정해 메모리 할당 및 새로 할당한 공간에 데이터를 복사
	String& operator=(const String&other) 
	{
		if (this == &other)
			return *this;

		if (_data)
			delete[] _data;

		_size = other._size;
		_data = new char[_size + 1];
		Strcpy(_data, other._data);

		return *this;
	}



	// 왼쪽 문자열과 오른쪽 문자열을 잇는 함수
	// 왼쪽과 오른쪽 문자열을 합친 값을 담을 버퍼가 필요
	// 버퍼에 얼마나 담아야 할지 모르니 각 문자열들의 크기가 필요함
	// 각 문자열의 크기를 확인
	// 문자열의 크기를 확인했으니 담을 버퍼의 크기 + 널 문자를 넣은 사이즈 +1 까지 미리 책정해서 버퍼를 만듦
	// 왼쪽 문자열부터 strcpy를 이용해 버퍼에 담음
	// 그 후 오른쪽 문자열까지 strcat을 이용해 버퍼에 담음
	// 버퍼는 char* 형 이기에 String으로 감싸서 반환
	String operator+(const String& other) const
	{

		int newsize = _size + other._size + 1;
		char* temp = new char[newsize + 1];

		Strcpy(temp, _data);
		Strcat(temp, other._data);

		String result(temp);
		delete[] temp;

		return String(result);
	}


	//두 문자열이 같은지 확인하는 함수
	// 기존에 구현해둔 strcmp 함수를 통해 두 문자열을 비교
	// 만약 같다면 0이 반환, 아니라면 다른 값이 반환 됨
	// 반환값에 따라 true/false를 반환하도록 반환형을 bool로 설정
	// 따라서 strcmp에서 반환된 값이 0이면 함수 전체에서 true를 나머지 값이면 false를 반환
	bool operator==(const String& other) const
	{
		if (Strcmp(_data, other._data) == 0)
			return true;
		else
			return false;
	}

	//현재 
	const char* c_str() const { return _data; }   // 내부 버퍼 반환
	int length() const { return _size; }

private:
	char* _data = nullptr;
	int _size = 0;
	int _capacity;
};