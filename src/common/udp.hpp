#pragma once
#include <string>

#include <WinSock2.h>

using namespace std;

class Sock {
public:
	static bool init_win_sock();
	static bool finish_win_sock();

	Sock();

	virtual bool init();
	int close();

	void set_myself(const string& addr, int port);
	void set_sending_target(const string& addr, int port);
	void set_recieving_target(const string& addr, int port);
	void set_timeout(int sec, int usec);

protected:
	SOCKET m_sock;
	fd_set m_read_fds;

	sockaddr_in m_myself, m_to, m_from;
	int m_to_len, m_from_len;

	timeval m_timeout;

	int m_sock_type;

	static WSADATA m_wsa;

};

class UDPSock : public Sock{
public:
	struct Packet {
		int pack_size;
		int seq;
		
		char* data;
		int data_size;
	};

	UDPSock();
	~UDPSock();

	bool _send(const char* packet, int packet_size);
	bool _send_back(const char* packet, int packet_size);
	int _receive(char* buf, int buf_size);

	bool _send(const Packet& pack);
	bool _receive(Packet& pack);

	bool send(const char* data, int data_size);
	bool receive(char* buf, int buf_size, int& data_size, int& seq);

	void set_dseg_size(int sz);

private:
	int m_max_dseg_size;

	char* m_sbuf;
	char* m_rbuf;

	int m_sbuf_size;
	int m_rbuf_size;

	static bool binit_win_sock;
	static bool bclose_win_sock;
};

class TCPServerSock : public Sock{
public:
	TCPServerSock();
	int listen_msg();
	int accept_client();

	bool  shake_hands();
	int receive_msg(char* buf, int buf_size);
	int send_msg(const char* buf, int buf_size);
	int shutdown_client();

private:
	SOCKET m_client_sock;
	sockaddr_in m_myself, m_to, m_from;
};
