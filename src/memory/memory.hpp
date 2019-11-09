#pragma once

#include <string>
#include <mutex>
#include <vector>
#include <iterator>
#include  <queue>

#include "../common/miscel.hpp"

using std::string;
using std::mutex;
using std::vector;
using std::iterator;
using std::queue;

class Memory {
public:
	//virtual bool set_data(const string& value) = 0;
	//virtual void write(char* buf, int buf_size);
	//virtual bool read(const char* buf, int buf_size);
	//virtual string get_data();

	void lock();
	void unlock();

protected:
	mutex m_lock;

private:
	string m_name;
	bool m_brom;
};

class MemInt : public Memory {
public:
	//MemInt(int vlaue);
	//virtual bool set_data(const string& value);
	void set_value(int value);

	int get_value();

private:
	int m_value;
};

class MemBool : public Memory {
public:
	//virtual bool set_data(const string& value);
	virtual string get_data();

	void set_status(bool status);
	bool get_status();
private:
	bool m_status;
};

class MemString : public Memory {
public:
	//virtual bool set_data(const string& value);
	void set_string(const string& str);
	string& get_string();

private:
	string m_string;
};

//template <typename  T>
//class MemRingBuf;


template <typename T>
class MemRingBuf : public Memory {
public:
	MemRingBuf();

	void set_data(T data);

	T get_data();

	bool empty();

private:
	vector<T> m_data;
	typename vector<T>::iterator m_data_rit;
	typename vector<T>::iterator m_data_wit;
};

template <typename T>
MemRingBuf<T>::MemRingBuf() {
	lock_guard<mutex> lock(m_lock);
	m_data.resize(10, nullptr);
	m_data_wit = m_data.begin();
	m_data_rit = m_data.begin();
}

template <typename T>
T MemRingBuf<T>::get_data() {
	lock_guard<mutex> lock(m_lock);

	if (m_data_rit == m_data_wit) {
		return nullptr;
	}

	T data;
	data = *m_data_rit;

	++m_data_rit;

	if (m_data_rit == m_data.end())
		m_data_rit = m_data.begin();


	return data;
}


template <typename T>
void MemRingBuf<T>::set_data(T data) {
	lock_guard<mutex> lock(m_lock);

	(*m_data_wit) = data;

	++m_data_wit;

	if (m_data_wit == m_data.end())
		m_data_wit = m_data.begin();

	if (m_data_wit == m_data_rit) {
		delete *m_data_rit;
		m_data_rit++;

		if (m_data_rit == m_data.end())
			m_data_rit = m_data.begin();
	}

}

template <typename T>
bool MemRingBuf<T>::empty() {
	return true;// m_data_rit = m_data_wit ? true : false;
}
//class MemImage : public Memory {
//public:
//	MemImage();
//	
//	void set_image(Image * img);
//	
//	Image* get_image();
//	
//	long long get_time();
//	
//	bool empty();
//
//	long long m_wref_count;
//	long long m_rref_count;
//
//private:
//	vector<Image*> m_imgs;
//	vector<Image*>::iterator m_imgs_rit;
//	vector<Image*>::iterator m_imgs_wit;
//};

typedef MemRingBuf<Image*> MemImage;
