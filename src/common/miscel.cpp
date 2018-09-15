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

//Mutex::Mutex(mutex* lock) : m_lock(lock) {
//	m_lock->lock();
//}
//
//Mutex::~Mutex() {
//	m_lock->unlock();
//}

string bool_to_str(bool status) {
	string str;
	if (status)
		str = "yes";
	else
		str = "no";
	return str;
}

bool str_to_bool(const string& str, bool& status) {
	if (str == "y" || str == "yes") {
		status = true;
	}
	else if (str == "n" || str == "no") {
		status = false;
	}
	else {
		return false;
	}

	return true;
}

//Maximul buffer size is 8224 bytes. 
 int32_t calc_checksum(char* buf, int buf_size) {
	int32_t sum = 0;
	for (int i = 0; i < buf_size; ++i) {
		sum += (int32_t)buf[i];
	}

	return sum - 1;
}

 bool check_checksum(char* buf, int buf_size, int32_t checksum) {
	 int32_t sum = 0;
	 for (int i = 0; i < buf_size; ++i) {
		 sum += (int32_t)buf[i];
	 }
	 sum -= 1;
	 if (sum == checksum)
		 return true;
	 else
		 return false;
 }