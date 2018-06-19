#include <iostream>

#include <WS2tcpip.h>

#include "udp.hpp"

using namespace std;

WSADATA UDPSock::m_wsa;

bool Sock::init_win_sock() {
	if (WSAStartup(WINSOCK_VERSION, &m_wsa) != 0) {
		cerr << " Windows Socket initialization error : " << WSAGetLastError() << endl;
		return false;
	}

	return true;
}

bool Sock::finish_win_sock() {
	if (WSACleanup() != 0) {
		cerr << "Windows Socket termination error : " << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

Sock::Sock() {
	memset(&m_myself, 0, sizeof(sockaddr_in));
	m_myself.sin_family = AF_INET;
	m_myself.sin_addr.s_addr = INADDR_ANY;
	m_myself.sin_port = htons(8080);

	m_to = m_myself;
	m_to_len = sizeof(m_to);

	m_from = m_myself;
	m_from_len = sizeof(m_from);

	set_timeout(10, 0);
}

bool Sock::init() {
	if ((m_sock = socket(m_myself.sin_family, m_sock_type, 0)) == INVALID_SOCKET) {
		cerr << "Socket creationn error : " << WSAGetLastError() << endl;
		return false;
	}

	if (bind(m_sock, (sockaddr*)&m_myself, sizeof(m_myself)) == SOCKET_ERROR) {
		cerr << "Binding error : " << WSAGetLastError() << endl;
		return false;
	}

	return true;
}

int Sock::close() {
	return closesocket(m_sock);
}

void Sock::set_myself(const string& addr, int port) {
	if (addr.empty())
		m_myself.sin_addr.s_addr = INADDR_ANY;
	else
		m_myself.sin_addr.s_addr = inet_addr(addr.c_str());
	m_myself.sin_port = htons(port);
}

void Sock::set_sending_target(const string& addr, int port) {
	if (addr.empty())
		m_to.sin_addr.s_addr = inet_addr("127.0.0.1");
	else
		m_to.sin_addr.s_addr = inet_addr(addr.c_str());
	m_to.sin_port = htons(port);
}

void Sock::set_recieving_target(const string& addr, int port) {
	if (addr.empty())
		m_from.sin_addr.s_addr = INADDR_ANY;
	else
		m_from.sin_addr.s_addr = inet_addr(addr.c_str());
	m_from.sin_port = htons(port);
}


void Sock::set_timeout(int sec, int usec) {
	m_timeout.tv_sec = sec;
	m_timeout.tv_usec = usec;
}

UDPSock::UDPSock() {
	m_sock_type = SOCK_DGRAM;
}

UDPSock::~UDPSock() {
	delete m_sbuf;
	delete m_rbuf;
}

bool UDPSock::_send(const Packet& pack) {
	const int isize = sizeof(int);

	if (m_sbuf_size < pack.pack_size) {
		delete m_sbuf;
		m_sbuf = new char[pack.pack_size];
	}

	char* tmp = m_sbuf;
	memcpy((void*)tmp, (void*)(pack.data_size), isize);
	tmp += isize;

	memcpy((void*)tmp, (void*)(&pack.seq), isize);
	tmp += isize;

	memcpy((void*)tmp, (void*)pack.data, pack.data_size);

	return _send(m_sbuf, pack.pack_size);
}

bool UDPSock::_receive(Packet& pack) {
	int rsize = _receive(m_rbuf, m_rbuf_size);
	if (rsize < 0)
		return false;

	pack.pack_size = *((int*)m_rbuf);
	int isize = sizeof(int);

	if (pack.pack_size != rsize) {
		cerr << "Received packet size " << rsize
			<< " is not equal to size expected " << pack.pack_size
			<< "." << endl;
		return false;
	}

	if (pack.pack_size > isize * 2) {
		cerr << "Packet size " << pack.pack_size << " specified in packet is invalid." << endl;
		return false;
	}

	char* tmp = isize + m_rbuf;
	pack.seq = *((int*)m_rbuf);
	tmp += isize;

	pack.data = tmp;
	pack.data_size = pack.pack_size - isize * 2;
	return true;
}

int UDPSock::_receive(char* buf, int buf_size) {
	FD_ZERO(&m_read_fds);
	FD_SET(m_sock, &m_read_fds);

	int num_fds = select(NULL, &m_read_fds, NULL, NULL, &m_timeout);

	if (num_fds == 0) {
		cerr << "Time limit(" << m_timeout.tv_sec << "sec " << m_timeout.tv_usec << "usec) " << "expired." << endl;
		return -1;
	}
	else if (num_fds == SOCKET_ERROR) {
		cerr << "Error occured in select function() : " << WSAGetLastError() << endl;
		return -1;
	}

	int recv_len;
	if ((recv_len = recvfrom(m_sock, buf, buf_size, 0, (sockaddr*)&m_from, &m_from_len)) == SOCKET_ERROR)
	{
		cerr << "Recieving error : " << WSAGetLastError() << endl;
		return -1;
	}

	return recv_len;
}

bool UDPSock::_send(const char* packet, int packet_size) {
	int ssize;
	ssize = sendto(m_sock, packet, packet_size, 0, (sockaddr*)&m_to, m_to_len);
	if (ssize == SOCKET_ERROR)
	{
		cerr << "Sending error : " << WSAGetLastError() << endl;
		return false;
	}
	else if (ssize != packet_size) {
		cerr << "Sent packet size is not equal to size expected." << endl;
		return false;
	}


	return true;
}

bool UDPSock::_send_back(const char* packet, int packet_size) {
	int ssize;
	ssize = sendto(m_sock, packet, packet_size, 0, (sockaddr*)&m_from, m_from_len);
	if (ssize == SOCKET_ERROR)
	{
		cerr << "Sending error : " << WSAGetLastError() << endl;
		return false;
	}
	else if (ssize != packet_size) {
		cerr << "Sent packet size is not equal to size expected." << endl;
		return false;
	}

	return true;
}


bool UDPSock::send(const char* data, int data_size) {
	int num_packs = 1+(data_size / m_max_dseg_size);
	if ((data_size % m_max_dseg_size) != 0) {
		num_packs++;
	}

	Packet pack;
	pack.pack_size = (sizeof(int) << 1) + pack.pack_size;
	pack.seq = 0;
	pack.data = (char*)(&num_packs);
	pack.data_size = sizeof(int);

	if(!_send(pack))
		return false;

	pack.data = const_cast<char*>(data);

	for (int s = 1; s < num_packs; ++s) {
		int dseg_size;
		int res = data - pack.data;

		if (res < m_max_dseg_size) {
			dseg_size = res;
		}
		else {
			dseg_size = m_max_dseg_size;
		}

		pack.seq = s;
		pack.data_size = dseg_size;

		if (!_send(pack))
			return false;

		pack.data += pack.data_size;		
	}

	return true;
}

bool UDPSock::receive(char* buf, int buf_size, int& data_size, int& seq) {
	Packet pack;
	if (!_receive(pack)) {
		return false;
	}

	int num_packs = (*(int*)pack.data);
	if (!(num_packs > 0)) {
		cerr << "Specified number of packets is invalid." << endl;
		return false;
	}

	int prev_seq = 1;
	int rdata_size = 0;
	char* tmp = buf;
	for (;;) {
		if (!_receive(pack)) {
			return false;
		}


		if (pack.seq == ++prev_seq) {
			cerr << "Packets is not sequential." << endl;
			return false;
		}

		rdata_size += pack.data_size;
		if (rdata_size > buf_size) {
			cerr << "Received data it too big for buffer." << endl;
			return false;
		}

		memcpy(tmp, pack.data, pack.data_size);
		tmp += pack.data_size;

		if (seq == num_packs - 1)
			return true;
	}
}

void UDPSock::set_dseg_size(int sz) {
	m_max_dseg_size = sz;
}

TCPServerSock::TCPServerSock() {
	m_sock_type = SOCK_STREAM;
}

int TCPServerSock::listen_msg() {
	return listen(m_sock, SOMAXCONN);
}

int TCPServerSock::accept_client() {
	m_client_sock = accept(m_sock, NULL, NULL);
	return m_client_sock;
}

bool TCPServerSock::shake_hands() {
	int res = listen(m_sock, SOMAXCONN);
	if (res == SOCKET_ERROR) {
		cerr << "listen failfcled with error : " << WSAGetLastError() << endl;
		return false;
	}
	return true;

	m_client_sock = accept(m_sock, NULL, NULL);
	if (m_client_sock == INVALID_SOCKET) {
		cerr << "accept failed with error : " << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

int TCPServerSock::receive_msg(char* buf, int buf_size) {
	return recv(m_client_sock, buf, buf_size, 0);
}

int TCPServerSock::send_msg(const char* buf, int buf_size) {
	return send(m_client_sock, buf, buf_size, 0);
}

int TCPServerSock::shutdown_client() {
	return shutdown(m_client_sock, SD_SEND);
}

