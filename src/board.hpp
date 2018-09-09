#pragma once
#include <map>
#include <string>
#include <mutex>

#include <WinSock2.h>

#include "common/clock.hpp"
#include "common/miscel.hpp"

#include "memory/memory.hpp"

using std::string;
using std::map;
using std::mutex;


class Board;
class Module;

void foo(const char*s);

typedef Module* (Board::*ModCreator)();
typedef map<const string, Module*> ModMap;
typedef map<const string, ModCreator> ModCreatorMap;

typedef Memory* (Board::*MemCreator)();
typedef map<const string, Memory*> MemMap;
typedef map<const string, MemCreator> MemCreatorMap;


class Board {
private:
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

	bool m_brun;
	bool m_bdebug;
	mutex m_lock;

	ModMap m_modules;

	ModCreatorMap m_mod_creators;

	MemMap m_memories;

	MemCreatorMap m_mem_creators;

	sockaddr_in m_myself;

	SOCKET m_myself_sock;

	template<typename T>
	Module* create_module();

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
		CmdProcess(Board* board);

		virtual bool process(vector<string>& args) = 0;

		Board* board;

		string msg;

		string help;
	};

	typedef map<const string, CmdProcess*> CmdProcMap;

	struct CmdModule : public CmdProcess {
		CmdModule(Board* board) : CmdProcess(board) {
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
		CmdSet(Board* board) : CmdProcess(board) {
		};
		virtual bool process(vector<string>& args);
	};

	struct CmdGet : public CmdProcess {
		CmdGet(Board* board) : CmdProcess(board) {
		};
		virtual bool process(vector<string>& args);
	};

	struct CmdLsMod : public CmdProcess {
		CmdLsMod(Board* board) : CmdProcess(board) {
		};
		virtual bool process(vector<string>& args);
	};

	struct CmdLsPort : public CmdProcess {
		CmdLsPort(Board* board) : CmdProcess(board) {
		};
		virtual bool process(vector<string>& args);
	};

	struct CmdRun : public CmdProcess {
		CmdRun(Board* board) : CmdProcess(board) {
		};
		virtual bool process(vector<string>& args);
	};

	struct CmdFinish : public CmdProcess {
		CmdFinish(Board* board) : CmdProcess(board) {
		};
		virtual bool process(vector<string>& args);
	};

	struct CmdShutdown : public CmdProcess {
		CmdShutdown(Board* board) : CmdProcess(board) {
		};
		virtual bool process(vector<string>& args);
	};

	struct CmdPing : public CmdProcess {
		CmdPing(Board* board) : CmdProcess(board) {
		};
		virtual bool process(vector<string>& args);
	};

	struct CmdMemory : public CmdProcess {
		CmdMemory(Board* board) : CmdProcess(board) {
		};
		virtual bool process(vector<string>& args);
	};

	struct CmdLsMem : public CmdProcess {
		CmdLsMem(Board* board) : CmdProcess(board) {
			help = "ls <target>\n";
			help += "list arbitrary object.";
		};
		virtual bool process(vector<string>& args);
	};

	struct CmdConnect : public CmdProcess {
		CmdConnect(Board* board) : CmdProcess(board) {
		}
		virtual bool process(vector<string>& args);
	};

	struct CmdStop : public CmdProcess {
		CmdStop(Board* board) : CmdProcess(board) {
		}
		virtual bool process(vector<string>& args);
	};

	CmdProcMap m_cmd_procs;


public:
	Board();

	bool init();
	void run();

	void remove();
	//void listen();
	

	bool set_data(const string& module, const string& port, const string& value);
	bool get_data(const string& module, const string& port, string& data);

	bool create_module(const string& type, const string& name);
	bool create_memory(const string& type, const string& name);

	bool init_all();

	bool run_module(const string& vname);
	void run_all_modules();
	
	//void run(const vector<char*>& vertetxes);

	void stop_all_modules();
	bool stop_module(const string& name);

	void finish();

	void lock();
	void unlock();


	bool connect(const string& mname0, const string& pname0,
		const string& mname1, const string& pname1);
	
	mutex* get_lock();
};