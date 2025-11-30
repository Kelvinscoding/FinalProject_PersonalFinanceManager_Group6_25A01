#pragma once
template <typename T>

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
	~DynamicArray() {
		delete[] data;
		data = nullptr;
	}
	long long size(){
		return cnt;
	}
	long long length(){
		return cnt;
	}
};