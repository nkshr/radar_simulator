#include <map>
#include "data_server.hpp"

using  std::pair;

DataServer::DataServer() {
	register_port("debug", "Flag for debugging data server.", false, &m_bdebug);
	register_port("update", "Flag for updating connection settings.", false, &m_bupdate);
	register_port("port", "Port number.", 8082, &m_port);
	memset(&m_server, 0, sizeof(m_server));
	m_server.sin_family = AF_INET;
	m_server.sin_addr.s_addr = INADDR_ANY;
	m_server.sin_port = htons(8080);
}

bool DataServer::process() {
	if (m_bupdate->get_status()) {
		m_server.sin_port = htons(m_port->get_value());
		m_server_sock = socket(m_server.sin_family, SOCK_STREAM, 0);
		if (m_server_sock == INVALID_SOCKET) {
			cerr << "Socket creation error : " << WSAGetLastError() << endl;
			closesocket(m_server_sock);
			WSACleanup();
			return false;
		}

		int res = bind(m_server_sock, (sockaddr*)&m_server, sizeof(m_server));
		if (res == SOCKET_ERROR) {
			cerr << "bind failed with error : " << WSAGetLastError() << endl;
			return false;
		}
	}

	//reception
	FD_ZERO(&m_rfds);
	FD_SET(m_server_sock, &m_rfds);

	int num_fds = select(NULL, &m_rfds, NULL, NULL, &m_timeout);

	if (num_fds == 0) {
		cerr << "Time limit(" << m_timeout.tv_sec << "sec " << m_timeout.tv_usec << "usec) " << "expired." << endl;
		return true;
	}
	else if (num_fds == SOCKET_ERROR) {
		cerr << "select failed with error. : " << WSAGetLastError() << endl;
		return true;
	}

	sockaddr_in client;
	int client_len;
	int res = recvfrom(m_server_sock, m_buf, m_buf_size, 0, (sockaddr*)&client, &client_len);
	if (res == SOCKET_ERROR) {
		cerr << "Recieving error : " << WSAGetLastError() << endl;
		return true;
	}

	PGN pgn;
	memcpy(&pgn, m_buf, sizeof(PGN));

	if (pgn == PGN_REQUEST) {
		Ship ship = m_ship->get_ship();
		memcpy(m_buf, &ship, sizeof(ship));
	}

	return true;
}

bool DataServer::setup_udp() {
	m_server.sin_port = htons(m_port->get_value());
	m_server_sock = socket(m_server.sin_family, SOCK_STREAM, 0);
	return true;
}