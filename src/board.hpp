#pragma once
#include <map>
#include <string>
#include <mutex>

#include "common/clock.hpp"
#include "common/miscel.hpp"

#include "memory/memory.hpp"

using std::string;
using std::map;
using std::mutex;


class Board;
class Module;
class CmdProcess;

void foo(const char*s);

typedef Module* (Board::*ModCreator)();
typedef map<const string, Module*> ModMap;
typedef map<const string, ModCreator> ModCreatorMap;
typedef map<const string, CmdProcess*> CmdProcMap;
typedef map<const string, Memory*> MemMap;


class Board {
public:
	Board();

	bool init();
	void run();

	void remove();
	//void listen();
	

	bool set_data(const string& module, const string& port, const string& value);
	bool get_data(const string& module, const string& port, string& data);

	bool create_module(const string& type, const string& name);
	
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

	vector<string> get_module_names() const;
	vector<string> get_module_types() const;
	ModMap& get_modules();
	
	mutex* get_lock();

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
		CMD_END
	};

	string cmd_strs[CMD::CMD_END] = { "module", "set", "get", "lsmod", "lsport", "run", "finish", "ping"};

	bool m_brun;
	bool m_bdebug;
	mutex m_lock;

	ModMap m_modules;

	ModCreatorMap m_mcreators;

	CmdProcMap m_cmd_procs;

	sockaddr_in m_myself;

	SOCKET m_myself_sock;

	template<typename T>
	Module* create_module();

	template<typename T>
	void register_module(const string& type);

	template<typename T>
	void register_cmd_proc(const string& name);

	//CmdServer m_cmd_server;
	bool cmd_module(vector<string>& args, string& msg);
	bool cmd_set(vector<string>& args, string& msg);
	bool cmd_get(vector<string>& args, string& msg);
	bool cmd_lsmod(vector<string>& args, string& msg);
	bool cmd_lsport(vector<string>& args, string& msg);
	bool cmd_run(vector<string>& args, string& msg);
	bool cmd_finish(vector<string>& args, string& msg);
};