#include <iostream>

#include "time_sync.hpp"

using std::cout;
using std::endl;

#define REQUEST 0x0000;
#define REPLY 0xFFFF;

TimeSync::TimeSync() : Module(), m_bserver(false) {
	register_bool("server", "flag which determimns if this module is master of time synqhronization. ", true, &m_bserver);
	register_int("port", "port number for network time protocol.(default).", 9090, &m_port);
}

TimeSync::~TimeSync() {

}
bool TimeSync::init() {
	m_myself_sock = socket(m_myself.sin_family, SOCK_STREAM, 0);
	if (m_myself_sock == INVALID_SOCKET) {
		cerr << "Socket creation error : " << WSAGetLastError() << endl;
		closesocket(m_myself_sock);
		WSACleanup();
		return false;
	}

	int res = bind(m_myself_sock, (sockaddr*)&m_myself, sizeof(m_myself));
	if (res == SOCKET_ERROR) {
		cerr << "bind failed with error : " << WSAGetLastError() << endl;
		return false;
	}

	return true;
}

bool TimeSync::process() {
	if (m_bserver)
		return server_process();
	else
		return client_process();
}

bool TimeSync::finish() {
	return true;
}

bool TimeSync::server_process() {

	long long cur_time = m_clock.get_system_time();
	int res = listen(m_myself_sock, SOMAXCONN);
	if (res == SOCKET_ERROR) {
		cerr << "listen failed with error : " << WSAGetLastError() << endl;
		return true;
	}

	SOCKET target_sock = accept(m_myself_sock, NULL, NULL);
	if (target_sock == INVALID_SOCKET) {
		cerr << "accept failed with error : " << WSAGetLastError() << endl;
		return true;
	}

	const int msg_size = 1024;
	char msg[msg_size];


	res = recv(target_sock, msg, msg_size, 0);
	if (res == SOCKET_ERROR) {
		cerr << "recv failed with error : " << WSAGetLastError() << endl;
		return true;
	}

	cout << msg << " received." << endl;

	//analyze message
	if (msg_size != 1) {
		cerr << "Size of received message is invalid." << endl;
		return false;
	}

	msg[0] = REPLY;

	memcpy((void*)&msg[1], (void*)m_clock.get_system_time(), sizeof(long long));
	res = send(target_sock, msg, msg_size, 0);

	if (res != msg_size) {
		cerr << "send failed with error : " << WSAGetLastError() << endl;
	}

	return true;
}

bool TimeSync::client_process() {
	return true;
}

