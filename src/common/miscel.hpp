#pragma once
#include <vector>
#include <string>
#include <iostream>

#include "config.h"
#include "udp.h"

using namespace std;

//enum Cmd {
//	VERTEX,
//	EDGE,
//	SET,
//	START,
//	STOP,
//	CLOSE,
//	LS,
//	END,
//	INVALID,
//};

const static string cmd_error_str = "cmd_error";
const static string cmd_success_str = "cmd_success";

//Cmd str_to_cmd(const string& str);
//string cmd_to_str(const Cmd& cmd);

void split(const string& buf, const string& delimes, vector<string>& toks);

const static vector<string> cmd_strs = {"vertex", "edge", "set",  "run", "stop", "close", "ls"};

struct CmdParser {
	string cmd;
	vector<string> args;
	void parse(const string& buf);
};

const static string cmd_delims = " ";

class CmdClient {
public:
	CmdClient() {};
	bool init();

	const string& get_get_result() const;
	const vector<string>& get_ls_results() const;

	bool request(const string& cmd, const vector<string>& args);

	void set_server(const string& addr, int port);
	void set_client(const string& addr, int port);

	bool is_cmd_success() const;

	const char* get_error_msg() const;

private:
	bool m_cmd_success;

	string m_emsg;
	string m_get_result;

	string m_delims;

	char m_rmsg[config::buf_size];

	vector<string> m_ls_results;

	UDP m_udp;
};

class CmdServer {
public:
	CmdServer();
	bool init();
	bool listen();

	bool send_error(const string& msg);
	bool send_success(const string& msg);

	void set_server(const string& addr, int port);

	const string& get_cmd() const;
	const vector<string>& get_args() const;

private:
	char m_rmsg[config::buf_size];




	UDP m_udp;
	CmdParser m_cp;
};

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
