#pragma once
#include <map>
#include <string>
#include <mutex>

#include "common/clock.hpp"
#include "common/miscel.hpp"

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
	CmdLsMod(Board* board) : CmdProcess(board) {};
	virtual bool process(vector<string> args);
};

class CmdFinish : public CmdProcess {
public:
	virtual bool process(vector<string> args);
};

class CmdPing : public CmdProcess {
public:
	virtual bool process(vector<string> args);
};

class Board {
public:
	Board();

	bool init();
	void run();

	void remove();
	//void listen();
	
	void set_port(int port);

	bool set_data_to_port(const string& module, const string& port, const string& value);

	bool create_module(const string& type, const string& name);
	
	bool init_all();

	bool run_module(const string& vname);
	void run_all_modules();
	
	//void run(const vector<char*>& vertetxes);

	void stop_all_modules();
	bool stop_module(const string& name);

	void stop();

	void lock();
	void unlock();


	bool connect(const string& mname0, const string& pname0,
		const string& mname1, const string& pname1);

	vector<string> get_module_names() const;
	vector<string> get_module_types() const;
private:
	bool m_brun;

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
};