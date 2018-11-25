#pragma once

#include <list>

#include <WinSock2.h>

#include "module.hpp"

using std::list;

struct TimeSyncPacket {
	unsigned int version;
	unsigned char type;
	long long rts; //receive timestamp
	long long xts; //transmit timestamp
	unsigned int checksum;
};


class TimeSyncServer : public  Module{
private:	
	bool m_bupdate;

	int m_port;

	sockaddr_in m_server;

	SOCKET m_sock;
	
public:
	TimeSyncServer(Board * board);
	~TimeSyncServer();
	virtual bool init();
	virtual bool process();
	virtual bool finish();

};

class TimeSyncClient : public Module {
private:
	bool m_bupdate;
	int m_port;
	int m_max_delay;
	int m_max_offset;
	sockaddr_in m_server;
	SOCKET m_sock;

	struct OffsetAndDelay {
		long long offset;
		long long delay;
	};

	list<OffsetAndDelay> m_oad_list;

public:
	TimeSyncClient(Board * board);
	~TimeSyncClient();
	virtual bool init();
	virtual bool process();
	virtual bool finish();
};