#pragma once
// 5. 싱글톤
//    이 프로그램에서 하나밖에 없는 클래스 인스턴스

class CSingleton
{
	// 아무도 접근 못하게 private으로 차단
private :
	CSingleton(){}
	~CSingleton(){}
	//클래스에는 속하지만 private에 접근 가능하게 만들어야 함
public:
	static CSingleton& GetInstance()
	{
		static CSingleton instance; //static 지역 변수는 프로그램 전체에서 한번만 호출하기에 하나임을 보장
		return instance;
	}
	CSingleton(const CSingleton&) = delete;             // 복사 생성 금지
	CSingleton& operator=(const CSingleton&) = delete; // 복사 대입 금지
};
