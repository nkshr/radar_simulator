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
class Signal;


void foo(const char*s);

typedef Module* (Board::*mcreator)();
typedef Signal* (Board::*screator)();

typedef map<const string, Module*> mmap;
typedef map<const string, Signal*> smap;


typedef map<const string, mcreator> mcmap;
typedef map<const string, screator> scmap;

class Board {
public:
	Board();

	bool init();
	void run();

	void remove();
	void listen();
	
	void set_port(int port);

	bool set_port(const string& module, const string& port, void* value);

	bool create_module(const string& type, const string& name);
	bool create_signal(const string& type, const string& name);

	bool start(const string& vname);
	void start_all();
	
	//void run(const vector<char*>& vertetxes);

	void stop_all();
	bool stop(const string& vname);

	void lock();
	void unlock();

	const Signal* get_signal(const string& name);

	bool connect(const string& mname0, const string& pname0,
		const string& mname1, const string& pname1);

private:
	mutex m_lock;

	char m_cmd_buf[config::buf_size];

	mmap m_modules;
	smap m_signals;

	mcmap m_mcreators;
	scmap m_screators;

	template<typename T>
	Module* create_module();
	template<typename T>
	Signal* create_signal();

	template<typename T>
	void register_module(const string& type);
	template<typename T>
	void register_signal(const string& type);

	CmdServer m_cmd_server;
};