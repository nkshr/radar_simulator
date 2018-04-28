#include <iostream>

#include "miscel.h"

using namespace std;

WSADATA UDP::m_wsa;

bool UDP::init_win_sock() {
	if (WSAStartup(WINSOCK_VERSION, &m_wsa) != 0) {
		cerr << " Windows Socket initialization error : " << WSAGetLastError() << endl;
		return false;
	}

	return true;
}

bool UDP::close_win_sock() {
	if (WSACleanup() != 0) {
		cerr << "Windows Socket termination error : " << WSAGetLastError() << endl;
		return false;
	}
	return true;
}


bool UDP::init() {
	if ((m_sock = socket(m_server.sin_family, SOCK_DGRAM, 0)) == INVALID_SOCKET){
		cerr << "Socket creationn error : " << WSAGetLastError() << endl;
		return false;
	}

	if (bind(m_sock, (sockaddr*)&m_server, sizeof(m_server)) == SOCKET_ERROR) {
		cerr << "Binding error : " << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

int UDP::receive(char * buf, int buf_size) {
	int recv_len;
	if((recv_len = recvfrom(m_sock, buf, buf_size, 0, (sockaddr*) &m_from, &m_from_len)) == SOCKET_ERROR)
	{
		cerr << "Recieving error : " << WSAGetLastError() << endl;
		return -1;
	}

	return recv_len;
}

int UDP::send(const char * buf, int buf_size) {
	int ssize;
	ssize = sendto(m_sock, buf, buf_size, 0, (sockaddr*)&m_to.sin_addr, m_to_len);
	if (ssize == SOCKET_ERROR)
	{
		cerr << "Sending error : " << WSAGetLastError() << endl;
		return -1;
	}

	return ssize;
}



bool UDP::close() {
	if (closesocket(m_sock) != 0) {
		cerr << "Socket closing error : " << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

void UDP::set_port(int port) {
	m_server.sin_port = htons(port);
}

void UDP::set_addr(const char* addr) {
	m_to.sin_addr.S_un.S_addr = inet_addr(addr);
}