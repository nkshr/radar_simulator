#include "cmd_receiver.hpp"

using namespace std;

//void CmdProcess::set_board(Board* board) {
//	m_board = board;
//}

CmdReceiver::CmdReceiver() {
	//register_port("debug", "", "y", MEM_TYPE::MT_INT, (Memory**)&m_bdebug);
	register_port("debug", "Flag for debugging CmdReceiver.", false, &m_bdebug);
	register_cmd_proc<CmdModule>("module");
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
	if (cmd_process->process(args)) {
		res = m_tcp_sock.send_msg(cmd_suc_str.c_str(), cmd_suc_str.size());
	}
	else {
		res = m_tcp_sock.send_msg(cmd_err_str.c_str(), cmd_err_str.size());
	}

	if (res != cmd_suc_str.size()) {
		cerr << "send failed with err : " << WSAGetLastError() << endl;
		return true;
	}

	const string& tmp = cmd_process->get_msg();
	res = m_tcp_sock.send_msg(tmp.c_str(), tmp.size());
	if (res != tmp.size()) {
		cerr << "send failed with err : " << WSAGetLastError() << endl;
		return true;
	}

	return true;
}

template <typename T>
void CmdReceiver::register_cmd_proc(const string& name) {
	CmdProcess* cmd_proc = static_cast<CmdProcess*>(new T(m_board));
	m_cmd_procs.insert(pair<string, CmdProcess*>(name, cmd_proc));
}