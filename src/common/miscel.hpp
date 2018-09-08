#pragma once
#include <vector>
#include <string>
#include <iostream>
//#include <mutex>

#include "config.hpp"

using std::string;
using std::vector;
using std::cerr;
using std::endl;

const static string cmd_err_str = "err";
const static string cmd_suc_str = "suc";

#define PRINT_DMSG(msg) std::cout << msg << << " " << __FILEW__ <<  " " << __LINE__ << std::endl;
//string cmd_to_str(const Cmd& cmd);

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

//class CmdServer {
//public:
//	CmdServer();
//	bool init();
//	bool listen();
//
//	bool send_error(const string& msg);
//	bool send_success(const string& msg);
//
//	void set_server(const string& addr, int port);
//
//	const string& get_cmd() const;
//	const vector<string>& get_args() const;
//
//private:
//	char m_rmsg[config::buf_size];
//
//
//
//
//	UDPSock m_udp;
//	CmdParser m_cp;
//};

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
//class Mutex {
//public:
//	Mutex(mutex* lock);
//	~Mutex();
//private:
//	mutex* m_lock;
//};

bool str_to_bool(const string& str, bool& status);