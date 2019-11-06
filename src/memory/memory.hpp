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
	virtual string get_data();

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

class MemImage : public Memory {
public:
	MemImage();
	
	void set_image(Image * img);
	
	Image* get_image();
	
	long long get_time();
	
	bool empty();

	long long m_wref_count;
	long long m_rref_count;

private:
	vector<Image*> m_imgs;
	vector<Image*>::iterator m_imgs_rit;
	vector<Image*>::iterator m_imgs_wit;
};