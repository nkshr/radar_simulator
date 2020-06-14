#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <WinSock2.h>

#include "common/miscel.hpp"

using namespace std;

int main(int argc, char ** argv) {
	const char* myself_addr = "127.0.0.1";
	const int myself_port = 8081;

	const char* target_addr = "127.0.0.1";
	const int target_port = 8080;

	const int timeout = 10;

	string smsg;

	WSADATA wsa;
	if (WSAStartup(WINSOCK_VERSION, &wsa) != 0) {
		cerr << " Windows Socket initialization error : " << WSAGetLastError() << endl;
		goto FAILURE;
	}

	SOCKET myself_sock;
	sockaddr_in myself;
	memset(&myself, 0, sizeof(sockaddr_in));
	myself.sin_family = AF_INET;
	myself.sin_addr.s_addr =inet_addr(myself_addr);
	myself.sin_port = htons(myself_port);

	myself_sock = socket(myself.sin_family, SOCK_STREAM, 0);
	if (myself_sock == INVALID_SOCKET) {
		cerr << "Socket creation error : " << WSAGetLastError() << endl;
		goto FAILURE;
	}

	sockaddr_in target;
	memset(&target, 0, sizeof(sockaddr_in));
	target.sin_family = AF_INET;
	target.sin_addr.s_addr = inet_addr(target_addr);
	target.sin_port = htons(target_port);

	//u_long flag_non_blocking = 1;
	//ioctlsocket(myself_sock, FIONBIO, &flag_non_blocking);

	int res;
	res = connect(myself_sock, (sockaddr*)&(target), sizeof(target));
	if (res == SOCKET_ERROR) {
		cerr << "connect failed with error : " << WSAGetLastError() << endl;
		goto FAILURE;
	}

	//timeval tv_timeout;
	//tv_timeout.tv_sec = timeout;
	//tv_timeout.tv_usec = 0;

	//fd_set wfds;
	//FD_ZERO(&wfds);
	//FD_SET(myself_sock, &wfds);

	//res = select(0, NULL, &wfds, NULL, &tv_timeout);
	//if (res == 0) {
	//	cerr << "Timeout in connect." << endl;
	//	goto FAILURE;
	//}

	//if (!FD_ISSET(myself_sock, &wfds)) {
	//	cerr << "Socket for client is not writable." << endl;
	//	goto FAILURE;
	//}


	for (int i = 1; i < argc; ++i) {
		smsg += string(argv[i]) + " ";
	}
	smsg += '\0';
	res = send(myself_sock, smsg.c_str(), smsg.size(), 0);
	if (res == SOCKET_ERROR) {
		cerr << "send failed with error : " << WSAGetLastError() << endl;
		goto FAILURE;
	}


	//res = shutdown(myself_sock, SD_SEND);
	//if (res == SOCKET_ERROR) {
	//	cerr << "shutdown failed with error : " << WSAGetLastError() << endl;
	//	closesocket(myself_sock);
	//	WSACleanup();
	//	return -1;
	//}

	char rmsg[1024];
	memset(rmsg, 0, sizeof(rmsg));
	int rmsg_size = sizeof(rmsg);
	res = recv(myself_sock, rmsg, rmsg_size, 0);
	if (res == SOCKET_ERROR) {
		cerr << "recv failed with error : " << WSAGetLastError() << endl;
		goto FAILURE;
	}
	rmsg[res] = '\0';

	if (rmsg == cmd_err_str) {
		res = recv(myself_sock, rmsg, rmsg_size, 0);
		if (res == SOCKET_ERROR) {
			cerr << "recv failed with error : " << WSAGetLastError() << endl;
			closesocket(myself_sock);
			WSACleanup();
			return -1;
		}
		rmsg[res] = '\0';

		cerr << rmsg << endl;
		closesocket(myself_sock);
		WSACleanup();
		return -1;
	}
	else if (rmsg == cmd_suc_str) {
		res = recv(myself_sock, rmsg, rmsg_size, 0);
		if (res == SOCKET_ERROR) {
			cerr << "recv failed with error : " << WSAGetLastError() << endl;
			goto FAILURE;
		}
		rmsg[res] = '\0';

		cout << rmsg << endl;
		goto SUCCESS;
	}

	cerr << "Invalid command result received. : " << rmsg << endl;

FAILURE:
	closesocket(myself_sock);
	WSACleanup();
	return -1;

SUCCESS:
	closesocket(myself_sock);
	WSACleanup();
	return 0;
}