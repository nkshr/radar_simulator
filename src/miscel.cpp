#include <iostream>
#include <sstream>

#include "miscel.h"

using namespace std;

WSADATA UDP::m_wsa;
bool binit_win_sock = false;
bool bclose_win_sock = false;

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
	if ((m_sock = socket(m_myself.sin_family, SOCK_DGRAM, 0)) == INVALID_SOCKET){
		cerr << "Socket creationn error : " << WSAGetLastError() << endl;
		return false;
	}

	if (bind(m_sock, (sockaddr*)&m_myself, sizeof(m_myself)) == SOCKET_ERROR) {
		cerr << "Binding error : " << WSAGetLastError() << endl;
		return false;
	}

	return true;
}

int UDP::receive(char* packet, int packet_size) {
	debug_msg dmsg("_receive");

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
	if((recv_len = recvfrom(m_sock, packet, packet_size, 0, (sockaddr*) &m_from, &m_from_len)) == SOCKET_ERROR)
	{
		cerr << "Recieving error : " << WSAGetLastError() << endl;
		return -1;
	}

	return recv_len;
}

int UDP::send(const char* packet, int packet_size) {
	int ssize;
	ssize = sendto(m_sock, packet, packet_size, 0, (sockaddr*)&m_to, m_to_len);
	if (ssize == SOCKET_ERROR)
	{
		cerr << "Sending error : " << WSAGetLastError() << endl;
		return -1;
	}

	return ssize;
}

int UDP::send_back(const char* packet, int packet_size) {
	int ssize;
	ssize = sendto(m_sock, packet, packet_size, 0, (sockaddr*)&m_from, m_from_len);
	if (ssize == SOCKET_ERROR)
	{
		cerr << "Sending error : " << WSAGetLastError() << endl;
		return -1;
	}
	else if(ssize != packet_size){
		cerr << "Sent packet size is not equal to size expected." << endl;
	}

	return ssize;
}

bool UDP::_send(const char* data, int data_size) {
	int data_idx = 0;
	bool success = false;
	while (!success) {
		while (1) {
			if (data_idx == data_size)
				break;

			int res = data_size - data_idx;
			int packet_size = m_packet_size < res ? packet_size : res;
			set_buf("send", data_idx, &(data[data_idx]), packet_size);

			int sent_size = send(m_sbuf, m_sbuf_size);
			if (sent_size < 0) {
				return false;
			}

			data_idx += sent_size;
		}

		int recv_size = receive(m_rbuf, m_rbuf_size);
		if (recv_size < 0)
			return false;

		if (data_size != atoi(m_rbuf))
			cerr << "Sending data was failed." << endl;
		else
			success = true;
	}

	return true;
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

void CmdParser::parse(const string& buf) {
	cout << buf << endl;
	vector<string> toks;
	split(buf, cmd_delims, toks);

	cmd = toks[0];
	args.assign(toks.begin() + 1, toks.end());
}


bool CmdClient::init() {
	return m_udp.init();
}

bool CmdClient::request(const string& cmd, const vector<string>& args) {
	string smsg = cmd;
	for (vector<string>::const_iterator it = args.begin(); it != args.end(); ++it) {
		smsg += " " + (*it);
	}

	smsg += '\0';

	if (m_udp.send(smsg.c_str(), smsg.size()) != smsg.size()) {
		return false;
	}

	if (m_udp.receive(m_rmsg, config::buf_size) <= 0) {
		return false;
	}


	if (m_rmsg == cmd_error_str) {
		m_cmd_success = false;
		if (m_udp.receive(m_rmsg, config::buf_size) <= 0) {
			return false;
		}
		m_emsg = m_rmsg;
		return true;
	}
	else if (m_rmsg == cmd_success_str) {
		m_cmd_success = true;
	}

	if (!(m_udp.receive(m_rmsg, config::buf_size) > 0)) {
		return false;
	}

	if (cmd == "ls") {
		split(m_rmsg, m_delims, m_ls_results);
	}
}

const string& CmdClient::get_get_result() const {
	return m_get_result;
}
const vector<string>& CmdClient::get_ls_results() const {
	return m_ls_results;
}


const char* CmdClient::get_error_msg() const {
	return m_rmsg;
}

bool CmdClient::is_cmd_success() const {
	return m_cmd_success;
}

void CmdClient::set_server(const string& addr, int port) {
	m_udp.set_sending_target(addr, port);
}

void CmdClient::set_client(const string& addr, int port) {
	m_udp.set_myself(addr, port);
}

//Cmd str_to_cmd(const string& str) {
//	for (int i = 0; i < cmd_strs.size(); ++i) {
//		if (str == cmd_strs[i]) {
//			return static_cast<Cmd>(i);
//		}
//	}
//
//	return Cmd::INVALID;
//}
//
//string cmd_to_str(const Cmd& cmd) {
//	for (int i = 0; i < Cmd::END; ++i) {
//		if (static_cast<int>(cmd) == i) {
//			return cmd_strs[i];
//		}
//	}
//	return "";
//}

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

CmdServer::CmdServer() {
}

 bool CmdServer::init() {
	return m_udp.init();
}

void CmdServer::set_server(const string& addr, int port) {
	m_udp.set_myself(addr, port);
}

bool CmdServer::send_error(const string& msg) {
	if(m_udp.send_back(cmd_error_str.c_str(), cmd_error_str.size()) < 0)
		return false;

	if (m_udp.send_back(msg.c_str(), msg.size()) < 0) {
		return false;
	}
	return true;
}

bool CmdServer::send_success(const string& msg) {
	if (m_udp.send_back(cmd_success_str.c_str(), cmd_success_str.size()) != cmd_success_str.size())
		return false;

	if (m_udp.send_back(msg.c_str(), msg.size()) != msg.size()) {
		return false;
	}
	return true;
}

bool CmdServer::listen() {
	if (m_udp.receive(m_rmsg, sizeof(m_rmsg)) <= 0) {
		return false;
	}
	
	m_cp.parse(m_rmsg);

	if (m_cp.cmd.empty()) {
		m_udp.send_back(cmd_error_str.c_str(), cmd_error_str.size());
		return false;
	}

	return true;
}

const string& CmdServer::get_cmd() const {
	return m_cp.cmd;
}

const vector<string>& CmdServer::get_args() const {
	return m_cp.args;
}