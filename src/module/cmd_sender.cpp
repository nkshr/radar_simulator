#include "cmd_sender.hpp"

#define FINISH "true && !m_once->get_status()"

CmdSender::CmdSender() {
	register_port("send", "Flag for sending command.", false, &m_send);
	register_port("cmd", "<command> <arg0> <arg1> ...", "", &m_cmd);
	register_port("addr", "String for containing receiver address.", "127.0.0.1", &m_rcvr_addr);
	register_port("port", "Value for receiver port.", 8080, &m_rcvr_port);
}

bool CmdSender::process() {
	if (!m_send->get_status()) {
		return true;
	}

	m_tcp_sock.set_sending_target(m_rcvr_addr->get_string(), m_rcvr_port->get_value());

	int res = m_tcp_sock.connect_server();
	if (res == SOCKET_ERROR) {
		cerr << "connect failed with error : " << WSAGetLastError() << endl;
		return true;
	}

	string smsg;
	res = m_tcp_sock.send_msg(smsg.c_str(), smsg.size());
	if (res == SOCKET_ERROR) {
		cerr << "send failed with error : " << WSAGetLastError() << endl;
		return true;
	}

	res = m_tcp_sock.receive_msg(m_rmsg, m_rmsg_size);
	if (res == SOCKET_ERROR) {
		cerr << "receive failed with error : " << WSAGetLastError() << endl;
		return true;
	}
	
	if (m_rmsg == cmd_err_str) {
		res = m_tcp_sock.receive_msg(m_rmsg, m_rmsg_size);
		if (res == SOCKET_ERROR) {
			cerr << "receive failed with error : " << WSAGetLastError() << endl;
			return true;
		}
		cerr << m_rmsg << endl;
	}
	else if (m_rmsg == cmd_suc_str) {
		res = m_tcp_sock.receive_msg(m_rmsg, m_rmsg_size);
		if (res == SOCKET_ERROR) {
			cerr << "receive failed with error : " << WSAGetLastError() << endl;
			return true;
		}
		cout << m_rmsg << endl;
	}
	else {
		cerr << "Invalid command result received. : " << m_rmsg << endl;
	}
	
	return true;
}

bool CmdSender::init() {
	return true;
}