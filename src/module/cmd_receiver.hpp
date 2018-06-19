#pragma once

#include "module.hpp"
#include "../common/udp.hpp"

class CmdProcess {
public:
	CmdProcess(Board* board);

	virtual bool operator()(vector<string> args) = 0;

	const string& get_msg() const;

protected:
	Board* m_board;

	string m_msg;
};

typedef map<const string, CmdProcess*> CmdProcMap;

class CmdModule : public CmdProcess {
	virtual bool operator()(vector<string> args);
};

class CmdReceiver : public Module {
public:
	CmdReceiver();

	virtual bool init();

	virtual bool process();
private:
	TCPServerSock m_tcp_sock;
	
	CmdParser m_cmd_parser;

	MemBool* m_bdebug;
	MemBool* m_bupdate;

	char m_recv_msg[1024];
	int m_recv_msg_size;
	
	string m_send_msg;

	CmdProcMap m_cmd_procs;
};
