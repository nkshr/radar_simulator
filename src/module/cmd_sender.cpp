#include "cmd_sender.hpp"

#define FINISH "true && !m_once->get_status()"

CmdSender::CmdSender() {

}

bool CmdSender::process() {
	if (!m_send->get_status()) {
		return true;
	}

	m_tcp_sock.set_sending_target(m_rcvr_addr->get_string(), m_rcvr_port->get_value());

	int res = m_tcp_sock.connect_server();
	if (res == SOCKET_ERROR) {
		cerr << "connect failed with error : " << WSAGetLastError() << endl;
		return !m_once->get_status();
	}

	string smsg;
	res = m_tcp_sock.send_msg(smsg.c_str(), smsg.size());
	if (res == SOCKET_ERROR) {
		cerr << "send failed with error : " << WSAGetLastError() << endl;
		return !m_once->get_status();
	}

	res = m_tcp_sock.receive_msg(m_rmsg, m_rmsg_size);
	if (res == SOCKET_ERROR) {
		cerr << "receive failed with error : " << WSAGetLastError() << endl;
		return !m_once->get_status();
	}
	
	if (m_rmsg == cmd_err_str) {
		res = m_tcp_sock.receive_msg(m_rmsg, m_rmsg_size);
		if (res == SOCKET_ERROR) {
			cerr << "receive failed with error : " << WSAGetLastError() << endl;
			return !m_once->get_status();
		}
		cerr << m_rmsg << endl;
	}
	else if (m_rmsg == cmd_suc_str) {
		res = m_tcp_sock.receive_msg(m_rmsg, m_rmsg_size);
		if (res == SOCKET_ERROR) {
			cerr << "receive failed with error : " << WSAGetLastError() << endl;
			return !m_once->get_status();
		}
		cout << m_rmsg << endl;
	}
	else {
		cerr << "Invalid command result received. : " << m_rmsg << endl;
	}
	
	return !m_once->get_status();
}

bool CmdSender::init() {
	return true;
}