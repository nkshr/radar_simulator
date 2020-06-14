#include <iostream>
#include <sstream>

#include <lodepng/lodepng.h>

#include "config.hpp"
#include "miscel.hpp"

using namespace std;

void CmdParser::parse(const string& buf) {
	cout << buf << endl;
	vector<string> toks;
	split(buf, cmd_delims, toks);

	cmd = toks[0];
	args.assign(toks.begin() + 1, toks.end());
}

CMD str_to_cmd(const string& str) {
	for (int i = 0; i < cmd_strs.size(); ++i) {
		if (str == cmd_strs[i]) {
			return static_cast<CMD>(i);
		}
	}

	return CMD_INVALID;
}

void split(const string &buf, const string& delims, vector<string>& toks) {
	toks.clear();
	stringstream ss(buf);
	string tok;
	for (char c : buf) {
		bool found = false;
		for (char d : delims) {
			if (c == d) {
				found = true;
				if(!tok.empty())
					toks.push_back(tok);
				tok.clear();
				break;
			}
		}
	
		if(!found)
			tok += c;
	}

	if (!tok.empty()) {
		toks.push_back(tok);
	}
}

template <typename T>
T* create_instance() {
	return new T;
}

string bool_to_str(bool status) {
	string str;
	if (status)
		str = "yes";
	else
		str = "no";
	return str;
}

bool str_to_bool(const string& str, bool& status) {
	if (str == "y" || str == "yes") {
		status = true;
	}
	else if (str == "n" || str == "no") {
		status = false;
	}
	else {
		return false;
	}

	return true;
}

//Maximul buffer size is 8224 bytes. 
 int32_t calc_checksum(char* buf, int buf_size) {
	int32_t sum = 0;
	for (int i = 0; i < buf_size; ++i) {
		sum += (int32_t)buf[i];
	}

	return sum - 1;
}

bool check_checksum(char* buf, int buf_size, int32_t checksum) {
	int32_t sum = 0;
	for (int i = 0; i < buf_size; ++i) {
		sum += (int32_t)buf[i];
	}
	sum -= 1;
	if (sum == checksum)
		return true;
	else
		return false;
}

char * load_text(const char *fname) {
	FILE * pf = fopen(fname, "rb");
	if (!pf) {
		return NULL;
	}

	fseek(pf, 0, SEEK_END);
	int size = ftell(pf);
	rewind(pf);

	char *txt = (char*)malloc(size + 1);
	fread(txt, 1, size, pf);
	txt[size] = '\0';
	fclose(pf);
	return txt;
}
 

int Image::get_width() {
	return m_w;
}

int Image::get_height() {
	return m_h;
}

int Image::get_depth() {
	return m_d;
}

unsigned char* Image::get_pixels() {
	return m_pixs.get();
}

Image::Image() : m_w(0), m_h(0), m_d(0), m_num_pixs(0) {
}

Image::Image(unsigned width, unsigned height, unsigned depth,
	unsigned char * pixs, long long time) : m_w(width), m_h(height), m_d(depth),
	m_pixs(pixs, default_delete<unsigned char[]>()), m_t(time) {
	m_num_pixs = m_w * m_h * m_d;
}

Image::Image(const Image & img) {
	m_w = img.m_w;
	m_h = img.m_h;
	m_d = img.m_d;
	m_t = img.m_t;
	m_num_pixs = img.m_num_pixs;
	m_pixs = img.m_pixs;
}

Image::~Image() {
}

Image Image::copy() {
	Image img;
	img.m_w = m_w;
	img.m_h = m_h;
	img.m_d = m_d;
	img.m_t = m_t;
	img.m_num_pixs = m_num_pixs;
	
	//img.m_pixs.reset(new unsigned char[m_num_pixs], default_delete<unsigned char[]>());
	unsigned char * pixs = new unsigned char[m_num_pixs];

	memcpy((void*)pixs, (void*)m_pixs.get(), m_num_pixs);
	return img;
}

bool Image::empty() {
	if (m_num_pixs == 0)
		return true;
	else
		return false;
}

long long Image::get_time() {
	return m_t;
}

void Image::set_time(long long t) {
	m_t = t;
}

void Image::destroy() {
	m_pixs.reset((unsigned char*)nullptr, default_delete<unsigned char[]>());
	m_w = m_h = m_d = m_num_pixs = 0;
}

void Image::transpose() {
	unsigned char * temp = new unsigned char [m_num_pixs];

	unsigned char * pixs = m_pixs.get();
	for (unsigned i = 0; i < m_num_pixs; ++i) {
		unsigned x = i % m_w;
		unsigned y = i / m_w;
		unsigned j = y + x * m_h;
		temp[j] = pixs[i];
	}

	m_pixs.reset(temp, default_delete<unsigned char[]>());
	int temp2 = m_w;
	m_w = m_h;
	m_h = temp2;
}

void  Image::reverse_rows() {
	unsigned half_num_pixs = m_num_pixs / 2;
	unsigned char * pixs = m_pixs.get();

	for (unsigned i = 0; i < half_num_pixs; ++i) {
		unsigned x = i % m_w;
		unsigned y = i / m_w;
		unsigned j = x  + m_w *(m_h- y -1);
		unsigned temp = pixs[j];
		pixs[j] = pixs[i];
		pixs[i] = temp;
	}
}

void Image::reverse_cols() {
	unsigned half_width = m_w / 2;
	unsigned char * pixs = m_pixs.get();

	for (unsigned i = 0; i < m_num_pixs; ++i) {
		unsigned x = i % m_w;
		if (x < half_width) {
			unsigned y = i / m_w;
			unsigned j = m_w - x - 1 + m_w * y;
			unsigned temp = pixs[j];
			pixs[j] = pixs[i];
			pixs[i] = temp;
		}
	}
}

//Image& Image::operator=(Image & img) {
//	m_w = img.m_w;
//	m_h = img.m_h;
//	m_d = img.m_d;
//	m_t = img.m_t;
//	m_pixs = img.m_pixs;
//	return *this;
//}

Image Image::move() {
	Image img(*this);
	*this = Image(0, 0, 0, nullptr, 0);
	return img;
}

Image imread(string &img_name, IMG_FMT fmt) {
	unsigned char * pixs;
	unsigned w;
	unsigned h;
	unsigned d;
	unsigned error;
	switch (fmt) {
	case IFMT_RGB:
	case IFMT_RGBA:
	case IFMT_GRAY:
		error = lodepng_decode_file(&pixs, &w, &h, img_name.c_str(), LCT_GREY, 8);
		break;
	}
	if (error) {
		cerr << "Error : " << lodepng_error_text(error) << endl;
		return Image();
	}
	else {
		d = 1;
	}

	return Image(w, h, d, pixs);
}

mutex lock_time;
string to_time_string(long long t) {
	lock_guard<mutex> lock(lock_time);

	time_t tt(t / 1000000000);
	string str(ctime(&tt));
	str[str.size()-1] = '\0';
	return str;
}

