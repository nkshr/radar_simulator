#pragma once

#include <WinSock2.h>

#include "config.h"

struct UDP {
	SOCKET sock;
	sockaddr_in server, client;
	int cli_len, recv_len;

	static WSADATA wsa;
	int port;

	static bool init_win_sock();
	static bool close_win_sock();

	bool init();
	int receive(char* buf, int buf_size);
	bool send(const char* buf, int buf_size);

	bool close();
};