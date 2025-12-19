#pragma once
#include <initializer_list>
#include <string>
#include <iostream>
template <typename T>

/// <summary>
/// HEADER FILE FOR DYNAMIC ARRAY
/// This header file is written to create a DynamicArray. 
/// Basic operations: 
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
/// 6. Memomy management: 
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
		for (long long i = 0; i < cnt; ++i) {
			data[i] = list[i];
		}
	}

	DynamicArray(const DynamicArray<T>& other) {
		cnt = other.cnt;
		capacity = other.capacity;
		data = new T[capacity];
		for (long long i = 0; i < cnt; i++) {
			data[i] = other.data[i];
		}
	}

	
	DynamicArray<T>& operator=(const DynamicArray<T>& other) {
		if (this != &other) {
			delete[] data;

			cnt = other.cnt;
			capacity = other.capacity;
			data = new T[capacity];
			for (long long i = 0; i < cnt; i++) {
				data[i] = other.data[i];
			}
		}
		return *this;
	}

	std::string generateNewId(std::string prefix) const{
		long long maxVal = 0;
		for (long long i = 0; i < cnt; i++) {
			std::string id = data[i].getId();
			if (id.find(prefix) == 0) {
				try {
					std::string numPart = id.substr(prefix.length());
					long long val = std::stoll(numPart);
					if (val > maxVal) maxVal = val;
				}
				catch (...) {}
			}
		}
		return prefix + std::to_string(maxVal + 1);
	}

	void resize(long long newCapac) {
		if (newCapac < cnt) { return; }
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

	const T& operator[] (long long idx) const {
		return data[idx];
	}

	void remove(long long index) {
		if (index < 0 || index >= cnt) return;
		for (long long i = index; i < cnt - 1; i++) {
			data[i] = data[i + 1];
		}
		cnt--;
	}

	~DynamicArray() {
		delete[] data;
		data = nullptr;
	}

	long long size() const {
		return cnt;
	}

	long long length() const {
		return cnt;
	}
};