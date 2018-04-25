#include <iostream>

#include "miscel.h"

using namespace std;

bool UDP::init_win_sock() {
	if (WSAStartup(WINSOCK_VERSION, &wsa) != 0) {
		cerr << " Windows Socket initialization error : " << WSAGetLastError() << endl;
		return false;
	}

	return true;
}

bool UDP::close_win_sock() {
	if (WSACleanup() != 0) {
		cerr << "Windows Socket Termination error : " << WSAGetLastError() << endl;
		return false;
	}
	return true;
}


bool UDP::init() {
	if ((sock = socket(server.sin_family, SOCK_DGRAM, 0)) == INVALID_SOCKET){
		cerr << "Socket creationn error : " << WSAGetLastError() << endl;
		return false;
	}

	if (bind(sock, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		cerr << "Binding error : " << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

int UDP::receive(char * buf, int buf_size) {
	int recv_len, slen;

	if((recv_len = recvfrom(sock, buf, buf_size, 0, (sockaddr*) &client, &slen)) == SOCKET_ERROR)
	{
		cerr << "Recieving error : " << WSAGetLastError() << endl;
		return -1;
	}

	return recv_len;
}

bool UDP::send(const char * buf, int buf_size) {
	if (sendto(sock, buf, buf_size, 0, (sockaddr*)&client.sin_addr, cli_len) == SOCKET_ERROR)
	{
		cerr << "Sending error : " << WSAGetLastError() << endl;
		return false;
	}

	return true;
}

bool UDP::close() {
	if (closesocket(sock) != 0) {
		cerr << "Socket closing error : " << WSAGetLastError() << endl;
		return false;
	}
	return true;
}