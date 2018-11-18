#include <iostream>

#include "time_sync.hpp"

using std::cout;
using std::endl;

#define VERSION 1
#define SERVER 0x0000
#define CLIENT 0xFFFF

char* type2str(char type) {
	switch (type) {
	case SERVER:
		return "SERVER";
	case CLIENT:
		return  "CLIENT";
	}
}

TimeSync::TimeSync() {

}

TimeSync::~TimeSync() {
}

bool TimeSync::init() {

	return true;
}

bool TimeSync::process() {
	return true;
}

bool TimeSync::finish() {
	return true;
}

TimeSyncServer::TimeSyncServer() {
	register_bool("update", "update port number(default n)", false, &m_bupdate);
	register_int("port", "port number for network time protocol.(default).", 9090, &m_port);

	m_server.sin_family = AF_INET;
	m_server.sin_addr.s_addr = INADDR_ANY;
	m_server.sin_port = htons(m_port);
}

TimeSyncServer::~TimeSyncServer() {

}
bool TimeSyncServer::init() {
	m_sock = socket(m_server.sin_family, SOCK_DGRAM, 0);
	if (m_sock == INVALID_SOCKET) {
		cerr << "Socket creation error : " << WSAGetLastError() << endl;
		closesocket(m_sock);
		WSACleanup();
		return false;
	}


	int res = bind(m_sock, (sockaddr*)&m_server, sizeof(m_server));
	if (res == SOCKET_ERROR) {
		cerr << "bind failed with error : " << WSAGetLastError() << endl;
		return false;
	}

	return true;
}

bool TimeSyncServer::process() {
	if (m_bupdate) {
		int res = bind(m_sock, (sockaddr*)&m_server, sizeof(m_server));
		if (res == SOCKET_ERROR) {
			cerr << "bind failed with error : " << WSAGetLastError() << endl;
			return false;
		}

		m_bupdate = false;
	}


	sockaddr_in client;
	int client_len = sizeof(client);
	TimeSyncPacket pack;

	int res = recvfrom(m_sock, (char*)&pack, sizeof(pack), 0,
		(struct sockaddr *) &client, &client_len);
	TimeSyncPacket spack;
	spack.version = VERSION;
	spack.rts = get_time();

	if (res == SOCKET_ERROR) {
		cerr << "recvfrom failed with error : " << WSAGetLastError() << endl;
		return true;
	}

	//analyze message
	if (res != sizeof(pack)) {
		cerr << "Size of received received packet was invalid." << endl;
		return false;
	}

	if (m_bdebug) {
		cout << "version : " << pack.version << endl;
		cout << "type : " << type2str(pack.type) << endl;
		cout << "transmit timestamp : " << pack.xts << endl;
		cout << "checksum : " << pack.checksum << endl;
	}

	if (pack.type != VERSION) {
		cerr << "Received version " << pack.version << " is not supported." << endl;
		return false;
	}

	if (pack.type != CLIENT) {
		cerr << "Invalid message was received." << endl;
		return false;
	}

	if (!check_checksum((char*)&pack, sizeof(pack) - 1, pack.checksum)) {
		cerr << "checksum of reception packet is invalid." << endl;
		return true;
	}

	spack.xts = get_time();
	spack.checksum = calc_checksum((char*)&pack, sizeof(pack)-1);

	res = sendto(m_sock, (char*)&spack, sizeof(spack), 0, (struct sockaddr*) &client, client_len);

	if (res != sizeof(spack)) {
		cerr << "sendto failed with error : " << WSAGetLastError() << endl;
	}

	return true;
}

bool TimeSyncServer::finish() {
	return true;
}

TimeSyncClient::TimeSyncClient() : Module() {
	register_bool("update", "update port number(default n)", false, &m_bupdate);
	register_int("port", "port number for network time protocol.(default).", 9090, &m_port);

	m_server.sin_family = AF_INET;
	m_server.sin_addr.s_addr = INADDR_ANY;
	m_server.sin_port = htons(m_port);

}

TimeSyncClient::~TimeSyncClient() {
}

bool TimeSyncClient::init() {
	m_sock = socket(m_server.sin_family, SOCK_DGRAM, IPPROTO_UDP);
	if (m_sock == INVALID_SOCKET) {
		cerr << "Socket creation error : " << WSAGetLastError() << endl;
		closesocket(m_sock);
		WSACleanup();
		return false;
	}

	return true;
}

bool TimeSyncClient::process() {
	long long t0;//client's timestamp of the request packet transmission
	long long t1;//server's timestamp of the request packet reception
	long long t2;//server's timestamp of the response packet transmission
	long long t3;//client's timestamp of the response pacekt reception

	TimeSyncPacket pack;
	pack.xts = t0 = get_time();
	int res = sendto(m_sock, (char*)&pack, sizeof(pack), 0, (struct sockaddr*) &m_server, sizeof(m_server));
	if (res !=  sizeof(pack)) {
		cerr << "sendto  failed with error : " << WSAGetLastError() << endl;
	}
	
	sockaddr_in si;
	int si_len;

	res = recvfrom(m_sock, (char*)&pack, sizeof(pack), 0, (struct sockaddr *)&si, &si_len);

	t3 = get_time();

	if (res == SOCKET_ERROR) {
		cerr << "recvfrom failed with error : " << WSAGetLastError() << endl;
		return true;
	}

	if (res != sizeof(pack)) {
		cerr << "Size of received packet was invalid." << endl;
		return true;
	}

	t1 = pack.rts;
	t2 = pack.xts;

	if (!check_checksum((char*)&pack, sizeof(pack)-1, pack.checksum)) {
		cerr << "checksum of reception packet is invalid." << endl;
		return true;
	}

	long long offset = ((t1 + t2)-(t0 + t3)) / 2;
	long long delay = t1 - t0 + t3 - t2;

	if (delay < m_max_delay) {
		if (offset < m_max_offset) {
			set_time(offset + get_time());
		}
	}

	return  true;
}

bool TimeSyncClient::finish() {
	return true;
}
