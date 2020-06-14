#pragma once

#include <thread>
#include <queue>
#include <condition_variable>
#include <complex>

#include "../module_base.hpp"
#include "../common/miscel.hpp"
#include "../memory/memory.hpp"
#include "../board.hpp"

using std::thread;
using std::queue;
using std::condition_variable;
using std::unique_lock;
using std::pair;

class Module : public ModuleBase{
public:
	enum COMMAND {
		INIT,
		RUN,
		STOP,
		BE_IDLE,
		FINISH,
		TURN_OFF,
		OPERATION_END
	};

	Module(const string& name, Board * board);
	virtual ~Module() {};

//////////////////thread safe functions///////////////
	void init();
	void run();
	void turn_on();
	void turn_off();
	void stop();
	void finish();

	//Port* get_port(const string& name);

	bool connect_memory(Memory* memory, const string& port);
	bool set_data(const string& name, const string& data);
	bool get_data(const string& name, string& data);

	void get_port_names_and_discs(vector<pair<string, string> >& names_and_discs);

	void wait_init_reply();
	void wait_finish_reply();

	bool get_init_status();
	bool get_finish_status();
	bool get_main_sw_status();

	unsigned get_id();

	void print(const string& str);

///////////////////////////////////////////////

private:
	COMMAND m_command;

	void processing_loop();

	unsigned m_module_id;

	string m_name;

protected:

	bool m_bdebug;

	unsigned char m_status;

	double m_cf;

	thread m_th;

	condition_variable m_cv;

	Clock * m_clock;

	Board *m_board;

	MemMap m_mems;


	void register_memory(const string& name, const string& disc,
		Memory** mem);

	void register_callback(const string& name, const string& disc,
		PortSetCallback psc, PortGetCallback pgc);

	void set_time(long long t);
	
	long long get_time();

	string get_time_as_string();
};


