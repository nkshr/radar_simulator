#pragma once
#include <map>
#include <set>
#include <string>
#include <mutex>

#include "common/clock.hpp"
#include "common/miscel.hpp"

using namespace std;

class Board;
class Module;

void foo(const char*s);

typedef Module* (Board::*ModCreator)();

typedef map<const string, Module*> ModMap;

typedef map<const string, ModCreator> ModCreatorMap;

class Board {
public:
	Board();

	bool init();
	void run();

	void remove();
	void listen();
	
	void set_port(int port);

	bool set_data_to_port(const string& module, const string& port, const string& value);

	bool create_module(const string& type, const string& name);
	
	bool init_all();

	bool start(const string& vname);
	void start_all();
	
	//void run(const vector<char*>& vertetxes);

	void stop_all();
	bool stop(const string& vname);

	void lock();
	void unlock();


	bool connect(const string& mname0, const string& pname0,
		const string& mname1, const string& pname1);

	vector<string> get_mod_names() const;

private:
	mutex m_lock;

	char m_cmd_buf[config::buf_size];

	ModMap m_modules;

	ModCreatorMap m_mcreators;

	template<typename T>
	Module* create_module();

	template<typename T>
	void register_module(const string& type);

	//CmdServer m_cmd_server;
};