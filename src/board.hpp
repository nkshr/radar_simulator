#pragma once
#include <map>
#include <string>
#include <mutex>

#include <WinSock2.h>

#include "common/clock.hpp"
#include "common/miscel.hpp"

#include "memory/memory.hpp"

#pragma comment(lib, "ws2_32.lib")

using std::string;
using std::map;
using std::mutex;

class MainProcess;
class SubProcess;
class Board;
class Module;

void foo(const char*s);

typedef Module* (SubProcess::*ModCreator)(const string& name, Board * board);
typedef map<const string, Module*> ModMap;
typedef map<const string, ModCreator> ModCreatorMap;

typedef Memory* (SubProcess::*MemCreator)();
typedef map<const string, Memory*> MemMap;
typedef map<const string, MemCreator> MemCreatorMap;


//class MainProcess {
//private:
//	mutex m_lock;
//	Clock m_clock;
//	bool m_bfinish;
//
//public:
//	void set_time(long long t);
//	long long get_time();
//	Clock *get_clock();
//	void process();
//	mutex& get_lock();
//	void finish();
//};

class SubProcess {
private:
	Board * m_board;

	enum CMD {
		MODULE,
		SET,
		GET,
		LSMOD,
		LSPORT,
		RUN,
		FINISH,
		PING,
		MEMORY,
		CMD_END,
	};

	struct Cmd {
		string cmd_str;
		string synopsis;
		string help;
	};

	//bool m_brun;
	//bool m_bdebug;
	//bool m_bfinish_cmd_server;
	//bool m_bfinish_main;
	bool m_bfinish;

	//mutex m_lock_cmd_server;
	//mutex m_lock_main;
	//mutex m_lock_clock;
	//mutex m_lock_print;
	mutex m_lock;

	///////////////////////////////////////
	/*Members below must be caleed with m_lock_cmd_server.*/
	ModMap m_modules;

	ModCreatorMap m_mod_creators;

	MemMap m_memories;

	MemCreatorMap m_mem_creators;

	sockaddr_in m_myself;

	SOCKET m_myself_sock;

	char m_recv_msg[1024];
	int m_recv_msg_size;
	////////////////////////////////////////

	template<typename T>
	Module* create_module(const string& name, Board * board);

	template<typename T>
	Memory* create_memory();

	template<typename T>
	void register_module(const string& type);

	template<typename T>
	void register_memory(const string& type);

	template<typename T>
	void register_cmd_proc(const string& name);

	//member for command process
	struct CmdProcess {
	protected:
		SubProcess* m_sub_proc;

	public:
		CmdProcess(SubProcess* board);

		bool _process(vector<string>& args) {
			std::lock_guard<mutex> lock(m_sub_proc->m_lock);
			return process(args);
		}

		virtual bool process(vector<string>& args) = 0;

		string msg;

		string help;
	};

	typedef map<const string, CmdProcess*> CmdProcMap;

	struct CmdModule : public CmdProcess {
		CmdModule(SubProcess* board) : CmdProcess(board) {
			help = "module <type> <name0> <name1>...\n";
			help += "Create named modules as the type specified by <type>.";
			for (ModCreatorMap::iterator it = board->m_mod_creators.begin(); it != board->m_mod_creators.end(); ++it) {
				help += it->first + '\n';
			}
			help[help.size() - 1] = '\0';
		};

		virtual bool process(vector<string>& args);
	};

	struct CmdSet : public CmdProcess {
		CmdSet(SubProcess* board) : CmdProcess(board) {
		};
		virtual bool process(vector<string>& args);
	};

	struct CmdGet : public CmdProcess {
		CmdGet(SubProcess* board) : CmdProcess(board) {
		};
		virtual bool process(vector<string>& args);
	};

	struct CmdLsMod : public CmdProcess {
		CmdLsMod(SubProcess* board) : CmdProcess(board) {
		};
		virtual bool process(vector<string>& args);
	};

	struct CmdLsPort : public CmdProcess {
		CmdLsPort(SubProcess* board) : CmdProcess(board) {
		};
		virtual bool process(vector<string>& args);
	};

	struct CmdRun : public CmdProcess {
		CmdRun(SubProcess* board) : CmdProcess(board) {
		};
		virtual bool process(vector<string>& args);
	};

	struct CmdFinish : public CmdProcess {
		CmdFinish(SubProcess* board) : CmdProcess(board) {
		};
		virtual bool process(vector<string>& args);
	};

	struct CmdShutdown : public CmdProcess {
		CmdShutdown(SubProcess* board) : CmdProcess(board) {
		};
		virtual bool process(vector<string>& args);
	};

	struct CmdPing : public CmdProcess {
		CmdPing(SubProcess* board) : CmdProcess(board) {
		};
		virtual bool process(vector<string>& args);
	};

	struct CmdMemory : public CmdProcess {
		CmdMemory(SubProcess* board) : CmdProcess(board) {
		};
		virtual bool process(vector<string>& args);
	};

	struct CmdLsMem : public CmdProcess {
		CmdLsMem(SubProcess* board) : CmdProcess(board) {
			help = "ls <target>\n";
			help += "list arbitrary object.";
		};
		virtual bool process(vector<string>& args);
	};

	struct CmdConnect : public CmdProcess {
		CmdConnect(SubProcess* board) : CmdProcess(board) {
		}
		virtual bool process(vector<string>& args);
	};

	struct CmdStop : public CmdProcess {
		CmdStop(SubProcess* board) : CmdProcess(board) {
		}
		virtual bool process(vector<string>& args);
	};

	CmdProcMap m_cmd_procs;

	Clock m_clock;

	bool set_data(const string& module, const string& port, const string& value);
	bool get_data(const string& module, const string& port, string& data);

	bool create_module(const string& type, const string& name);
	bool create_memory(const string& type, const string& name);

public:
	SubProcess(Board * board);

	bool init();

	void process();

	void finish();
};

class Board {
public:
	Board();

	bool init();

	void run();

	void finish();

	mutex& get_lock_print();
	mutex& get_lock_main_proc();
	mutex& get_lock_sub_proc();

	Clock * get_clock();

	mutex m_lock_print;

	void finish_main_proc();

	void set_time(long long t);
	long long get_time();
	void process();
	mutex& get_lock();

private:
	mutex m_lock;
	Clock m_clock;

	bool m_bfinish_main;

	SubProcess m_sub_proc;
	void main_process();

	double m_cf;
};
