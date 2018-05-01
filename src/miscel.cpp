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

	FD_ZERO(&m_read_fds);
	FD_SET(m_sock, &m_read_fds);

	return true;
}

int UDP::receive(char * buf, int buf_size) {
	int num_fds = select(NULL, &m_read_fds, NULL, NULL, &m_timeout);

	if (num_fds == 0) {
		cerr << "Time limit(" << m_timeout.tv_sec << "sec " << m_timeout.tv_usec << "usec) " << "expired." << endl;
		return false;
	}
	else if (num_fds == SOCKET_ERROR) {
		cerr << "Error occured in select function() : " << WSAGetLastError() << endl;
		return false;
	}

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

void UDP::set_timeout(int sec, int usec) {
	m_timeout.tv_sec = sec;
	m_timeout.tv_usec = usec;
}

bool CmdParser::parse() {
	char* delims = " \n";
	char* tok = strtok(m_buf, delims);

	if (!set_cmd(tok)) {
		cerr << "Unrecognized cmd : " << tok << endl;
		return false;
	}

	m_args.clear();
	while (true) {
		tok = strtok(NULL, delims);
		m_args.push_back(tok);
	}

	return true;
}

void CmdParser::encode(char* &buf, int& buf_size) {
	buf_size = 2 + sizeof(Cmd) + 2 * (m_args.size() + m_args_size); 
	buf = new char[buf_size];

	memcpy(static_cast<void*>(buf), static_cast<void*>("\c"), 2);

	char* ptr = buf + 2;
	const int cmd_size = sizeof(Cmd);

	memcpy(static_cast<void*>(ptr), static_cast<void*>(&m_cmd), cmd_size);
	ptr += cmd_size;

	for (int i = 0; i < m_args.size(); ++i) {
		memcpy(static_cast<void*>(ptr), static_cast<void*>(m_args[i]), strlen(m_args[i]));
	}
}

int CmdParser::get_buf_size() const {
	return config::buf_size;
}

char* CmdParser::get_buf(){
	return m_buf;
}

Cmd CmdParser::get_cmd() const {
	return m_cmd;
}
bool CmdParser::set_cmd(char* cmd) {
	for (int i = 0; i < CMD_END; ++i) {
		if (strcmp(cmd, m_cmds[i]) == 0) {
			m_cmd = static_cast<Cmd>(i);
			return true;
		}
	}
	
	return false;
}

const vector<char*>& CmdParser::get_args() const {
	return m_args;
}
