#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <mutex>
#include <utility>
#include <functional>
#include <map>

#include <WinSock2.h>

#include "config.hpp"

using std::string;
using std::vector;
using std::cerr;
using std::endl;
using std::shared_ptr;
using std::mutex;
using std::make_pair;
using std::map;

const static string cmd_err_str = "err";
const static string cmd_suc_str = "suc";

#define PRINT_DMSG(msg) std::cout << msg << << " " << __FILEW__ <<  " " << __LINE__ << std::endl;

void split(const string& buf, const string& delimes, vector<string>& toks);

const static vector<string> cmd_strs = {"module", "signal", "set",  "run", "stop", "close", "ls"};
enum CMD {
	CMD_MODULE,
	CMD_PORT_SET,
	CMD_MOD_LS,
	CMD_INVALID
};

CMD str_to_cmd(const string& str);

struct CmdParser {
	string cmd;
	vector<string> args;
	void parse(const string& buf);
};

const static string cmd_delims = " ";

struct string_comparator {
	bool operator()(const char* str1, const char* str2) {
		if (strcmp(str1, str2) < 0) {
			return true;
		}
		return false;
	}
};

class debug_msg {
public:
	debug_msg(const string& scope) : m_scope(scope){
		cerr << "Entering " << m_scope << endl;
	}

	~debug_msg() {
		cerr << "Exiting " << m_scope  << endl;
	}

private:
	string m_scope;

};

template <typename T>
T* create_instance();

string bool_to_str(bool status);

bool str_to_bool(const string& str, bool& status);

int32_t calc_checksum(char* buf, int buf_size);

bool check_checksum(char* buf, int buf_size, int32_t checksum);

char * load_text(const char *fname);

enum IMG_FMT {
	IFMT_RGB,
	IFMT_RGBA,
	IFMT_GRAY,
};

const static map<string, IMG_FMT> img_fmt_map{
	make_pair("rgb", IFMT_RGB),
	make_pair("rgba", IFMT_RGBA),
	make_pair("gray", IFMT_GRAY)
};

const vector<string> img_fmt_strs{
	"rgb",
	"rgba",
	"gray"
};

class Image {
private:
	unsigned m_w;
	unsigned m_h;
	unsigned m_d;
	unsigned m_num_pixs;
	
	//shared_ptr<unsigned char> m_pixs;
	
	shared_ptr<unsigned char>  m_pixs;

	long long m_t;

public:
	Image();
	Image(unsigned width, unsigned height, unsigned depth,
		unsigned char * pixs,long long time = 0);
	
	Image(const Image &img);

	~Image();

	int get_width();
	int get_height();
	int get_depth();
	unsigned char* get_pixels();

	Image copy();

	bool empty();

	long long get_time();
	
	void set_time(long long t);

	void destroy();

	void transpose();

	void reverse_rows();
	void reverse_cols();

	//Image& operator=(Image & img);

	Image move();
};

Image imread(string &img_name, IMG_FMT fmt = IFMT_RGB);


string to_time_string(long long t);