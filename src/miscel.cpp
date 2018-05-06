#include <iostream>
#include <sstream>

#include "miscel.h"

using namespace std;

WSADATA UDP::m_wsa;
bool binit_win_sock = false;
bool bclose_win_sock = false;

UDP::UDP() {
	m_myself.sin_family = AF_INET;
	m_myself.sin_addr.s_addr = INADDR_ANY;
	m_myself.sin_port = htons(5000);

	m_to = m_myself;
	m_to_len = sizeof(m_to);

	m_from = m_myself;
	m_from_len = sizeof(m_from);

	set_timeout(10, 0);
}

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
	if ((m_sock = socket(m_myself.sin_family, SOCK_DGRAM, 0)) == INVALID_SOCKET){
		cerr << "Socket creationn error : " << WSAGetLastError() << endl;
		return false;
	}

	if (bind(m_sock, (sockaddr*)&m_myself, sizeof(m_myself)) == SOCKET_ERROR) {
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
		return -1;
	}
	else if (num_fds == SOCKET_ERROR) {
		cerr << "Error occured in select function() : " << WSAGetLastError() << endl;
		return -1;
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
	ssize = sendto(m_sock, buf, buf_size, 0, (sockaddr*)&m_to, m_to_len);
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

void UDP::set_myself(const char* addr, int port) {
	if (addr == NULL)
		m_myself.sin_addr.s_addr = INADDR_ANY;
	else
		m_myself.sin_addr.s_addr = inet_addr(addr);
	m_myself.sin_port = htons(port);
}

void UDP::set_sending_target(const char* addr, int port) {
	if (addr == NULL)
		m_to.sin_addr.s_addr = INADDR_ANY;
	else
		m_to.sin_addr.s_addr = inet_addr(addr);
	m_to.sin_port = htons(port);
}

void UDP::set_recieving_target(const char* addr, int port) {
	if (addr == NULL)
		m_from.sin_addr.s_addr = INADDR_ANY;
	else
		m_from.sin_addr.s_addr = inet_addr(addr);
	m_from.sin_port = htons(port);
}


void UDP::set_timeout(int sec, int usec) {
	m_timeout.tv_sec = sec;
	m_timeout.tv_usec = usec;
}

void CmdParser::parse(const string& buf) {
	vector<string> toks;
	split(buf, cmd_delims, toks);

	m_cmd = str_to_cmd(toks[0]);
	m_args.assign(toks.begin() + 1, toks.end());
}

Cmd CmdParser::get_cmd() const {
	return m_cmd;
}

const vector<string>& CmdParser::get_args() const {
	return m_args;
}

bool CmdSender::request(Cmd cmd, const vector<string>& args) {
	m_udp.send(m_smsg, config::buf_size]);
	m_udp.receive(m_rmsg, config::buf_size);

	if (!strcmp(m_rmsg, "success") == 0) {
		return false;
	}

	m_udp.receive(m_rmsg, config::buf_size);

	switch (cmd) {
	case LS:
		if (args[0] == "vtype") {
			split(m_rmsg, m_delims, m_vtypes);
		}
		else if (args[0] == "vname") {
			split(m_rmsg, m_delims, m_vnames);
		}
		else if (args[0] == "etype") {
			split(m_rmsg, m_delims, m_etypes);
		}
		else if (args[0] == "ename"){
			split(m_rmsg, m_delims, m_enames);
		}
		break;
	}
}

Cmd str_to_cmd(const string& str) {
	for (int i = 0; i < cmd_strs.size(); ++i) {
		if (str == cmd_strs[i]) {
			return static_cast<Cmd>(i);
		}
	}

	return Cmd::INVALID;
}

void split(const string &buf, const string& delims, vector<string>& toks) {
	toks.clear();
	stringstream ss(buf);
	string tok;
	for (char c : buf) {
		bool found = false;
		for (char d : delims) {
			if (c == d) {
				found = true;
				if(!tok.empty())
					toks.push_back(tok);
				tok.clear();
				break;
			}
		}
	
		if(!found)
			tok += c;
	}

	if (!tok.empty()) {
		toks.push_back(tok);
	}
}