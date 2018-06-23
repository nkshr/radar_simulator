#pragma once

#include "module.hpp"
#include "../common/udp.hpp"

class CmdProcess {
public:
	CmdProcess(Board* board);

	virtual bool process(vector<string> args) = 0;

	const string& get_msg() const;
	const string& get_name() const;

	//static void set_board(Board* board);

protected:
	Board* m_board;

	string m_msg;

	string m_name;

	string m_disc;
};

//Board* CmdProcess::m_board = nullptr;

typedef map<const string, CmdProcess*> CmdProcMap;

class CmdModule : public CmdProcess {
public:
	CmdModule(Board* board);
	virtual bool process(vector<string> args);
};

class CmdSet : public CmdProcess {
public:
	virtual bool process(vector<string> args);
};

class CmdLsMod : public CmdProcess {
public:
	virtual bool process(vector<string> args);
};

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
