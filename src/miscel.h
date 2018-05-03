#pragma once
#include <vector>

#include <WinSock2.h>

#include "config.h"

using namespace std;

class UDP {
private:
	SOCKET m_sock;
	fd_set m_read_fds;
	sockaddr_in m_server, m_to, m_from;
	int m_to_len, m_from_len;
	timeval m_timeout;

	static WSADATA m_wsa;

public:
	static bool init_win_sock();
	static bool close_win_sock();

	bool init();
	bool close();

	int receive(char* buf, int buf_size);
	int send(const char* buf, int buf_size);

	void set_port(int port);
	void set_addr(const char* addr);
	void set_timeout(int sec, int usec);
};

enum Cmd {
	VERTEX,
	EDGE,
	VSET,
	ESET,
	RUN,
	STOP,
	CLOSE,
	CMD_END
};

class CmdParser {
public:

	CmdParser() {};
	bool parse();
	void encode(char* &buf, int& buf_size);
	bool decode(char* buf);

	int get_buf_size() const;
	char* get_buf();
	
	Cmd get_cmd() const;
	char* get_vertex_type() const;
	char* get_vertex_id() const;
	const vector<char*>& get_args() const;

	void set_buf(char* buf, int buf_size);
	bool set_cmd(char* cmd);

private:
	char* m_buf;
	char* encoded_data;

	int m_buf_size;
	int m_args_size;
	vector<char*> m_args;
	vector<char*> m_cmds;
	Cmd m_cmd;
};

struct string_comparator {
	bool operator()(const char* str1, const char* str2) {
		if (strcmp(str1, str2) < 0) {
			return true;
		}
		return false;
	}
};
