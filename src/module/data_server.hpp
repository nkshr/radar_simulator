#pragma once
#include <list>

#include "../common/miscel.hpp"

#include "module.hpp"

using std::map;

class DataServer : public Module {
private:
	enum PGN {
		PGN_READ,
		PGN_WRITE,
		PGN_REQUEST,
	};
	int m_packet_size;

	MemBool* m_bdebug;
	MemBool* m_bupdate;

	MemInt* m_port;
	Memory* m_mem;

	sockaddr_in m_server;
	map<ULONG, sockaddr_in> m_clients;

	SOCKET m_server_sock;
	//SOCKET m_tcp_sock;
	char m_buf[1024];
	const int m_buf_size = sizeof(m_buf);

	timeval m_timeout;

	fd_set m_rfds;

public:
	DataServer();
	
	virtual bool process();
	virtual bool init() { return true; };

	bool setup_udp();
};