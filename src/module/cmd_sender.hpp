#pragma once
#include "module.hpp"

class CmdSender : public Module {
private:
	TCPSock m_tcp_sock;
	MemBool* m_once;
	MemBool* m_send;
	MemString* m_cmd;
	MemString* m_rcvr_addr;
	MemInt* m_rcvr_port;
	char m_rmsg[1024];
	int m_rmsg_size;

public:
	CmdSender();

	virtual bool process();
	virtual bool init();
};