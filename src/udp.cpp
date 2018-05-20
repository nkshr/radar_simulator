#include <iostream>

#include "udp.h"

using namespace std;

WSADATA UDP::m_wsa;

void UDP::set_spack(int seq, const char* data, int  data_size) {
	const int int_sz = sizeof(int);
	m_spack_size = (int_sz << 1) + data_size;
	if (m_spack_buf_size < m_spack_size) {
		delete m_spack_buf;
		m_spack_buf = new char[m_spack_size];
	}

	char* packet = m_spack_buf;

	memcpy((void*)packet, (void*)(&m_rpack_buf_size), int_sz);
	packet += int_sz;

	memcpy((void*)packet, (void*)(&seq), int_sz);
	packet += int_sz;

	memcpy((void*)packet, (void*)data, data_size);
}

UDP::UDP() {
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

UDP::~UDP() {
	delete m_spack_buf;
	delete m_rpack_buf;
}

bool UDP::init_win_sock() {
	if (WSAStartup(WINSOCK_VERSION, &m_wsa) != 0) {
		cerr << " Windows Socket initialization error : " << WSAGetLastError() << endl;
		return false;
	}

	return true;
}

bool UDP::finish_win_sock() {
	if (WSACleanup() != 0) {
		cerr << "Windows Socket termination error : " << WSAGetLastError() << endl;
		return false;
	}
	return true;
}


bool UDP::init() {
	if ((m_sock = socket(m_myself.sin_family, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
		cerr << "Socket creationn error : " << WSAGetLastError() << endl;
		return false;
	}

	if (bind(m_sock, (sockaddr*)&m_myself, sizeof(m_myself)) == SOCKET_ERROR) {
		cerr << "Binding error : " << WSAGetLastError() << endl;
		return false;
	}

	return true;
}

int UDP::_receive(char* buf, int buf_size) {
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

int UDP::_send(const char* packet, int packet_size) {
	int ssize;
	ssize = sendto(m_sock, packet, packet_size, 0, (sockaddr*)&m_to, m_to_len);
	if (ssize == SOCKET_ERROR)
	{
		cerr << "Sending error : " << WSAGetLastError() << endl;
	}
	else if (ssize != packet_size) {
		cerr << "Sent packet size is not equal to size expected." << endl;
	}


	return ssize;
}

int UDP::_send_back(const char* packet, int packet_size) {
	int ssize;
	ssize = sendto(m_sock, packet, packet_size, 0, (sockaddr*)&m_from, m_from_len);
	if (ssize == SOCKET_ERROR)
	{
		cerr << "Sending error : " << WSAGetLastError() << endl;
	}
	else if (ssize != packet_size) {
		cerr << "Sent packet size is not equal to size expected." << endl;
	}

	return ssize;
}


bool UDP::send(const char* data, int data_size) {
	int num_packs = data_size / m_max_dseg_size;
	if ((data_size % m_max_dseg_size) != 0) {
		num_packs++;
	}

	const char* dfrag = data;
	for (int s = 0; s < num_packs; ++s) {
		int dseg_size;
		int res = data - dfrag;
		if (res < m_max_dseg_size) {
			dseg_size = res;
		}
		else {
			dseg_size = m_max_dseg_size;
		}

		if ((s + 1) == num_packs)
			set_spack(-s, dfrag, dseg_size);
		else
			set_spack(s, dfrag, dseg_size);

		int sent_size = _send(m_spack_buf, m_spack_buf_size);
		if (sent_size != m_spack_buf_size) {
			return false;
		}

		dfrag += sent_size;
	}

	return true;
}

bool UDP::receive(char* buf, int buf_size, int& data_size, int& seq) {
	const int isize = sizeof(int);
	int prev_seq = -1;
	char* prpack = m_rpack_buf;
	char* pbuf = buf;
	int num_packs;
	data_size = 0;
	for (;;) {
		int rsize = _receive(prpack, m_rpack_buf_size);
		if (rsize < 0)
			return false;

		int pack_size = (*(int*)prpack);
		prpack += isize;
		if (pack_size != rsize) {
			cerr << "Received packet size " << rsize 
				<< " is not equal to size expected " << pack_size 
				<< "."<< endl;
			return false;
		}

		int seq = (*(int*)prpack);
		prpack += isize;

		if (seq == ++prev_seq) {
			cerr << "Packets is not sequential." << endl;
			return false;
		}

		int dseg_size = pack_size - isize * 2;

		if (dseg_size + data_size > buf_size) {
			cerr << "Received data it too big." << endl;
			return false;
		}

		if (seq == 0) {
			num_packs = (*(int*)prpack);
		}
		else {
			memcpy(pbuf, prpack, dseg_size);
		}

		pbuf += dseg_size;
		data_size += dseg_size;

		if (seq == num_packs - 1)
			return true;
	}
}

bool UDP::close() {
	if (closesocket(m_sock) != 0) {
		cerr << "Socket closing error : " << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

void UDP::set_myself(const string& addr, int port) {
	if (addr.empty())
		m_myself.sin_addr.s_addr = INADDR_ANY;
	else
		m_myself.sin_addr.s_addr = inet_addr(addr.c_str());
	m_myself.sin_port = htons(port);
}

void UDP::set_sending_target(const string& addr, int port) {
	if (addr.empty())
		m_to.sin_addr.s_addr = inet_addr("127.0.0.1");
	else
		m_to.sin_addr.s_addr = inet_addr(addr.c_str());
	m_to.sin_port = htons(port);
}

void UDP::set_recieving_target(const string& addr, int port) {
	if (addr.empty())
		m_from.sin_addr.s_addr = INADDR_ANY;
	else
		m_from.sin_addr.s_addr = inet_addr(addr.c_str());
	m_from.sin_port = htons(port);
}


void UDP::set_timeout(int sec, int usec) {
	m_timeout.tv_sec = sec;
	m_timeout.tv_usec = usec;
}
