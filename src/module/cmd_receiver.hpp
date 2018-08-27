#pragma once

#include "module.hpp"
#include "../common/udp.hpp"

class CmdReceiver : public Module {
public:
	CmdReceiver();

	virtual bool init();

	virtual bool process();
private:
	TCPSock m_tcp_sock;
	
	CmdParser m_cmd_parser;

	MemBool* m_bdebug;
	MemBool* m_bupdate;

	char m_recv_msg[1024];
	int m_recv_msg_size;
	
	CmdProcMap m_cmd_procs;

	template <typename T>
	void register_cmd_proc(const string& name);
};
