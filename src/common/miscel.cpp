#include <iostream>
#include <sstream>

#include "config.hpp"
#include "miscel.hpp"

using namespace std;

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

	if (m_udp._receive(m_rmsg, config::buf_size) <= 0) {
		return false;
	}


	if (m_rmsg == cmd_err_str) {
		m_cmd_success = false;
		if (m_udp._receive(m_rmsg, config::buf_size) <= 0) {
			return false;
		}
		m_emsg = m_rmsg;
		return true;
	}
	else if (m_rmsg == cmd_suc_str) {
		m_cmd_success = true;
	}

	if (!(m_udp._receive(m_rmsg, config::buf_size) > 0)) {
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

CMD str_to_cmd(const string& str) {
	for (int i = 0; i < cmd_strs.size(); ++i) {
		if (str == cmd_strs[i]) {
			return static_cast<CMD>(i);
		}
	}

	return CMD_INVALID;
}
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

//CmdServer::CmdServer() {
//}
//
// bool CmdServer::init() {
//	return m_udp.init();
//}
//
//void CmdServer::set_server(const string& addr, int port) {
//	m_udp.set_myself(addr, port);
//}
//
//bool CmdServer::send_error(const string& msg) {
//	if(m_udp._send_back(cmd_error_str.c_str(), cmd_error_str.size()) < 0)
//		return false;
//
//	if (m_udp._send_back(msg.c_str(), msg.size()) < 0) {
//		return false;
//	}
//	return true;
//}
//
//bool CmdServer::send_success(const string& msg) {
//	if (m_udp._send_back(cmd_success_str.c_str(), cmd_success_str.size()) != cmd_success_str.size())
//		return false;
//
//	if (m_udp._send_back(msg.c_str(), msg.size()) != msg.size()) {
//		return false;
//	}
//	return true;
//}
//
//bool CmdServer::listen() {
//	if (m_udp._receive(m_rmsg, sizeof(m_rmsg)) <= 0) {
//		return false;
//	}
//	
//	m_cp.parse(m_rmsg);
//
//	if (m_cp.cmd.empty()) {
//		m_udp._send_back(cmd_error_str.c_str(), cmd_error_str.size());
//		return false;
//	}
//
//	return true;
//}
//
//const string& CmdServer::get_cmd() const {
//	return m_cp.cmd;
//}
//
//const vector<string>& CmdServer::get_args() const {
//	return m_cp.args;
//}


template <typename T>
T* create_instance() {
	return new T;
}

Mutex::Mutex(mutex* lock) : m_lock(lock) {
	m_lock->lock();
}

Mutex::~Mutex() {
	m_lock->unlock();
}

