#pragma once

#include <WinSock2.h>

#include "config.h"

class UDP {
private:
	SOCKET m_sock;
	sockaddr_in m_server, m_to, m_from;
	int m_to_len, m_from_len;

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
};