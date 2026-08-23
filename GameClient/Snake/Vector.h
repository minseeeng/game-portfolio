#pragma once
// 5.vector

template <typename T>
class Vector
{
public:
	Vector() {}
	~Vector() { delete[] _data; }

	void Push_back(const T& value)
	{
		if (_size == _capacity)
		{
			int newcapacity = _capacity * 2;

			if (newcapacity == _capacity)
				newcapacity++;
			
			reserve(newcapacity);
		}

		_data[_size] = value;
		_size++;
	}

	void reserve(int capacity)
	{
		if (_capacity >= capacity)
			return;

		_capacity = capacity;

		T* newdata = new T[_capacity];

		for (int i = 0; i < _size; i++)
			newdata[i] = _data[i];

		if (_data)
			delete[] _data;

		_data = newdata;
	}

	void clear()
	{
		if (_data)
		{
			delete[] _data;
			_data = new T[_capacity];
		}

		_size = 0;
	}

	T& operator[](int index) { return _data[index]; }
	int size() { return _size; }
	int capacity() { return _capacity; }

private:
	T* _data = nullptr;
	int _size = 0;
	int _capacity = 0;
};