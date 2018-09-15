#pragma once

#include <WinSock2.h>

#include "module.hpp"

class TimeSync : public  Module{
private:
	bool m_bserver;
	
	int m_port;

	sockaddr_in m_myself;

	SOCKET m_myself_sock;
	
public:
	TimeSync();
	~TimeSync();
	virtual bool init();
	virtual bool process();
	virtual bool finish();

	bool server_process();
	bool client_process();
};