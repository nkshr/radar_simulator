#include <iostream>

#include "time_sync.hpp"

using std::cout;
using std::endl;

#define REQUEST 0x0000
#define REPLY 0xFFFF

TimeSyncServer::TimeSyncServer() : Module() {
	register_bool("update", "update port number(default n)", false, &m_bupdate);
	register_int("port", "port number for network time protocol.(default).", 9090, &m_port);

	m_server.sin_family = AF_INET;
	m_server.sin_addr.s_addr = INADDR_ANY;
	m_server.sin_port = htons(m_port);
}

TimeSyncServer::~TimeSyncServer() {

}
bool TimeSyncServer::init() {
	m_server_sock = socket(m_server.sin_family, SOCK_DGRAM, 0);
	if (m_server_sock == INVALID_SOCKET) {
		cerr << "Socket creation error : " << WSAGetLastError() << endl;
		closesocket(m_server_sock);
		WSACleanup();
		return false;
	}


	int res = bind(m_server_sock, (sockaddr*)&m_server, sizeof(m_server));
	if (res == SOCKET_ERROR) {
		cerr << "bind failed with error : " << WSAGetLastError() << endl;
		return false;
	}

	return true;
}

bool TimeSyncServer::process() {
	if (m_bupdate) {
		int res = bind(m_server_sock, (sockaddr*)&m_server, sizeof(m_server));
		if (res == SOCKET_ERROR) {
			cerr << "bind failed with error : " << WSAGetLastError() << endl;
			return false;
		}

		m_bupdate = false;
	}

	long long cur_time = m_clock.get_system_time();

	sockaddr_in client_sock;
	int client_sock_len;
	const int msg_size = 1024;
	char msg[msg_size];

	int res = recvfrom(m_server_sock, msg, msg_size, 0,
		(struct sockaddr *) &client_sock, &client_sock_len);
	if (res == SOCKET_ERROR) {
		cerr << "recvfrom failed with error : " << WSAGetLastError() << endl;
		return true;
	}

	cout << msg << " received." << endl;

	//analyze message
	if (msg_size != 1) {
		cerr << "Size of received message was invalid." << endl;
		return false;
	}

	if (msg[0] != REQUEST) {
		cerr << "Invalid message was received." << endl;
		return false;
	}

	msg[0] = REPLY;

	memcpy((void*)&msg[1], (void*)m_clock.get_system_time(), sizeof(long long));
	res = send(m_server_sock, msg, msg_size, 0);

	if (res != msg_size) {
		cerr << "send failed with error : " << WSAGetLastError() << endl;
	}

	return true;
}

bool TimeSyncServer::finish() {
	return true;
}

TimeSyncClient::TimeSyncClient() : Module() {
	register_int("port", "port number for network time protocol.(default).", 9090, &m_port);
}

TimeSyncClient::~TimeSyncClient() {
	
}

bool TimeSyncClient::init() {
	return true;
}

bool TimeSyncClient::process() {
	return  true;
}

bool TimeSyncClient::finish() {
	return true;
}
