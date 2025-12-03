#pragma once
#include <initializer_list>
template <typename T>

/// <summary>
/// This header file is written to create a DynamicArray. 
/// Basic operation: 
/// 1. Declare a DynamicArray: 
///		DynamicArray<type> <ArrayName>; (default, initial capacity = 10)
///		Or: 
///		DynamicArray<type> <ArrayName> = {Element1, Element2...};
///		(If the number of elements < 10, set to 10)
/// 2. Add an element: 
///		<ArrayName>.push_back(<element>);
///		If the DynamicArray's capacity is full, it doubles and then add the element.
/// 3. Get the size: 
///		<ArrayName>.size();
///		Or:
///		<ArrayName>.length();
/// 4. Access element: 
///		<ArrayName>[<index>];
/// 5. Remove element: 
///		<ArrayName>.remove(<index>);
/// 6. Memory management: 
///		The DynamicArray will automatically be removed (see the ~DynamicArray() line).
///		No need for using delete[] each time declaring a DynamicArray.
/// </summary>

class DynamicArray {
private:
	T* data;
	long long capacity;
	long long cnt;
public:
	DynamicArray() {
		capacity = 10;
		cnt = 0;
		data = new T[10];
	}

	DynamicArray(std::initializer_list<T> list) {
		cnt = list.size();
		capacity = cnt;
		if (capacity < 10) capacity = 10;
		data = new T[capacity];
		long long i = 0;
		for (const T& item : list) {
			data[i] = item;
			i++;
		}
	}

	void resize(long long newCapac) {
		T* temp = new T[newCapac];
		for (long long i = 0; i < cnt; ++i) {
			temp[i] = data[i];
		}
		delete[] data;
		data = temp;
		capacity = newCapac;
	}

	void push_back(const T& item) {
		if (cnt == capacity) {
			resize(capacity * 2);
		}
		data[cnt] = item;
		cnt++;
	}

	T& operator[] (long long idx) {
		return data[idx];
	}

	void remove(long long index) {
		if (index < 0 || index >= cnt) return;
		for (long long i = index; i < cnt - 1; i++) {
			data[i] = data[i + 1];
		}
		cnt--;
	}

	long long size() {
		return cnt;
	}

	long long length() {
		return cnt;
	}

	~DynamicArray() {
		delete[] data;
		data = nullptr;
	}
};