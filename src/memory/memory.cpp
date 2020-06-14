#include "memory.hpp"

using namespace std;
//using std::unique_lock;
//using std::lock_guard;

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

bool Memory::init() {
	return true;
}

bool Memory::finish() {
	return true;
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

MemRefFrames::MemRefFrames() {
}


bool MemRefFrames::init() {
	vector<RefFrame>::iterator beg = m_data.begin();
	vector<RefFrame>::iterator end = m_data.end();

	int size_vec_info_3d = m_data.size() - 1;

	for (vector<RefFrame>::iterator it = beg; it != end; ++end) {
		it->vec_info_3d.resize(size_vec_info_3d);
	}

	return true;
}