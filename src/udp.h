#pragma once
#include <string>

#include <WinSock2.h>

using namespace std;

class UDP {
private:
	SOCKET m_sock;
	fd_set m_read_fds;

	sockaddr_in m_myself, m_to, m_from;
	int m_to_len, m_from_len;

	timeval m_timeout;

	static WSADATA m_wsa;

	char* m_sbuf;
	int m_sbuf_size;

	char* m_rbuf;
	int m_rbuf_size;

	int m_packet_size;
	int m_max_dseg_size;

	static bool binit_win_sock;
	static bool bclose_win_sock;

	void set_sbuf(int seq, const char* data, int data_size);

public:
	UDP();
	~UDP();

	static bool init_win_sock();
	static bool finish_win_sock();

	bool init();
	bool close();

	int _receive(char* packet, int packet_size);
	int _send(const char* packet, int packet_size);
	int _send_back(const char* packet, int packet_size);

	bool send(const char* data, int data_size);

	void set_myself(const string& addr, int port);
	void set_sending_target(const string& addr, int port);
	void set_recieving_target(const string& addr, int port);
	void set_timeout(int sec, int usec);
	void set_dseq_size(int sz);

};
