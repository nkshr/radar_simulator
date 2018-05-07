#pragma once
#include <vector>
#include <string>

#include <WinSock2.h>

#include "config.h"

using namespace std;

class UDP {
private:
	SOCKET m_sock;
	fd_set m_read_fds;
	sockaddr_in m_myself, m_to, m_from;
	int m_to_len, m_from_len;
	timeval m_timeout;

	static WSADATA m_wsa;
	static bool binit_win_sock;
	static bool bclose_win_sock;

public:
	UDP();

	static bool init_win_sock();
	static bool close_win_sock();

	bool init();
	bool close();

	int receive(char* buf, int buf_size);
	int send(const char* buf, int buf_size);
	int send_back(const char* buf, int buf_size);

	void set_myself(const char* addr, int port);
	void set_sending_target(const char* addr, int port);
	void set_recieving_target(const char* addr, int port);
	void UDP::set_timeout(int sec, int usec);
};

enum Cmd {
	VERTEX,
	EDGE,
	SET,
	RUN,
	STOP,
	CLOSE,
	LS,
	END,
	INVALID,
};

const static string cmd_error_str = "cmd_error";
const static string cmd_success_str = "cmd_success";

Cmd str_to_cmd(const string& str);
string cmd_to_str(const Cmd& cmd);

void split(const string& buf, const string& delimes, vector<string>& toks);

const static vector<string> cmd_strs = {"vertex", "edge", "set",  "run", "stop", "close", "ls"};

class CmdParser {
public:
	void parse(const string& buf);
	Cmd get_cmd() const;

	const vector<string>& get_args() const;

private:
	Cmd m_cmd;
	vector<string> m_args;
};

const static string cmd_delims = " ";

class CmdSender : public UDP{
public:

	CmdSender() {};
	
	/*char* get_vertex_type() const;
	char* get_vertex_id() const;*/
	const vector<string>& get_vtypes() const;
	const vector<string>& get_vnames() const;
	const vector<string>& get_etypes() const;
	const vector<string>& get_enames() const;


	bool request(Cmd cmd, const vector<string>& args);

	const string& get_err_msg() const;

private:
	string m_err_msg;
	string m_delims;

	char m_rmsg[config::buf_size];

	vector<string> m_vtypes;
	vector<string> m_vnames;
	
	vector<string> m_etypes;
	vector<string> m_enames;

	vector<string> m_cmd_strs;
};

class CmdReceiver : public UDP, public CmdParser{
public:
	CmdReceiver() {};

private:
	char m_rmsg[config::buf_size];

	int send_err_msg(const string& msg);

	CmdParser m_cp;
	UDP m_udp;
};

struct string_comparator {
	bool operator()(const char* str1, const char* str2) {
		if (strcmp(str1, str2) < 0) {
			return true;
		}
		return false;
	}
};
