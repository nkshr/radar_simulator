#include "memory.hpp"

using std::unique_lock;


string Memory::get_data() {
	return string();
}

//MemInt::MemInt(int value) {
//	m_value = value;
//}

//bool MemInt::set_data(const string& value) {
//	m_value = stoi(value);
//	return true;
//}
//
//bool Memory::is_rom() const {
//	return m_brom;
//}

void Memory::lock() {
	m_lock.lock();
}

void Memory::unlock() {
	m_lock.unlock();
}

void MemInt::set_value(int value) {
	m_value = value;
}

int MemInt::get_value() {
	//Mutex lock(&m_lock);
	return m_value;
}

//bool MemBool::set_data(const string& value) {
//	if (value == "y" || value == "yes") {
//		m_status = true;
//	}
//	else if (value == "n" || value == "no") {
//		m_status = false;
//	}
//	else {
//		return false;
//	}
//	return true;
//}

void MemBool::set_status(bool status) {
	m_status = status;
}

bool MemBool::get_status() {
	return m_status;
}

string MemBool::get_data() {
	if (m_status)
		return "y";
	return "n";
}

//bool MemString::set_data(const string& data) {
//	m_string = data;
//	return true;
//}
//
//void MemString::set_string(const string& str) {
//	set_data(str);
//}

MemImage::MemImage():m_wref_count(0), m_rref_count(0) {
	m_imgs.resize(10, nullptr);
	m_imgs_wit = m_imgs.begin();
	m_imgs_rit = m_imgs.begin();
}

string& MemString::get_string() {
	return m_string;
}

void MemImage::set_image(Image * img) {
	lock();
	
	(*m_imgs_wit) = img;

	++m_imgs_wit;

	if (m_imgs_wit == m_imgs.end())
		m_imgs_wit = m_imgs.begin();

	if (m_imgs_wit == m_imgs_rit) {
		delete *m_imgs_rit;
		m_imgs_rit++;

		if (m_imgs_rit == m_imgs.end())
			m_imgs_rit = m_imgs.begin();
	}

	unlock();
}

Image * MemImage::get_image(){
	lock();
	
	if (m_imgs_rit == m_imgs_wit) {
		unlock();
		return nullptr;
	}

	Image * img;
	img = *m_imgs_rit;

	++m_imgs_rit;

	if (m_imgs_rit == m_imgs.end())
		m_imgs_rit = m_imgs.begin();

	unlock();

	return img;
}

long long MemImage::get_time() {
	lock();
	Image * img = m_imgs.front();
	unlock();
	return img->get_time();
}

bool MemImage::empty() {
	return m_imgs_rit == m_imgs_wit ? true : false;
}