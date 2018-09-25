#pragma once

#include <WinSock2.h>

#include "module.hpp"

class TimeSyncServer : public  Module{
private:	
	int m_port;

	sockaddr_in m_myself;

	SOCKET m_myself_sock;
	
public:
	TimeSyncServer();
	~TimeSyncServer();
	virtual bool init();
	virtual bool process();
	virtual bool finish();

};

class TimeSyncClient : public Module {
private:
	int m_port;
	sockaddr_in m_myself;
	SOCKET m_myself_sock;

public:
	TimeSyncClient();
	~TimeSyncClient();
	virtual bool init();
	virtual bool process();
	virtual bool finish();
};