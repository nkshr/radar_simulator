#include "cmd_receiver.hpp"

CmdProcess::CmdProcess(Board* board) {
	m_board = board;
}

const string& CmdProcess::get_msg() const {
	return m_msg;
}

bool CmdModule::operator()(vector<string> args) {
	if (args.size() < 1) {
		m_msg = "Too few arguments.\n";
		m_msg += "module <type> <name0> <name1>...";
		return false;
	}

	string& type = args[0];
	string& name = args[1];
	if (!m_board->create_module(type, name)) {
		m_msg = "Couldn't create module " + type + " " + name + ".";
		return false;
	}

	return true;
}



CmdReceiver::CmdReceiver(){
	register_port("debug", "", "y", MemType::MT_INT, (Memory**)&m_bdebug);
}

bool CmdReceiver::init() {
	return true;
}

bool CmdReceiver::process() {
	int res = m_tcp_sock.listen_msg();
	if (res == SOCKET_ERROR) {
		cerr << "listen failed with error : " << WSAGetLastError() << endl;
		return  true;
	}

	res = m_tcp_sock.accept_client();
	if (res == INVALID_SOCKET) {
		cerr << "accept failed with error : " << WSAGetLastError() << endl;
		return true;
	}

	res = m_tcp_sock.receive_msg(m_recv_msg, m_recv_msg_size);

	if (m_bdebug->get_status() && res > 0) {
		cerr << res << " bytes recieved." << endl;
	}

	vector<string> toks;
	split(m_recv_msg, " \n", toks);

	//CMD cmd = str_to_cmd(toks[0]);
	string& cmd = toks[0];

	vector<string> args;
	if (toks.size() > 1) {
		args.assign(toks.begin() + 1, toks.end());
	}

	CmdProcess* cmd_process = m_cmd_procs.find(cmd)->second;
	if ((*cmd_process)(args)) {
		m_send_msg = cmd_suc_str;
	}
	else {
		m_send_msg = cmd_err_str;
	}

	res = m_tcp_sock.send_msg(m_send_msg.c_str(), m_send_msg.size());
	if (res != m_send_msg.size()) {
		cerr << "send failed with err : " << WSAGetLastError() << endl;
		return true;
	}

	m_send_msg = cmd_process->get_msg();
	res = m_tcp_sock.send_msg(m_send_msg.c_str(), m_send_msg.size());
	if (res != m_send_msg.size()) {
		cerr << "send failed with err : " << WSAGetLastError() << endl;
		return true;
	}

	return true;
}