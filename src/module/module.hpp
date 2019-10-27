#pragma once
#include <string>
#include <thread>
#include <queue>
#include <condition_variable>
#include <complex>

#include <functional>

#include "../common/miscel.hpp"
#include "../memory/memory.hpp"
#include "../board.hpp"

using std::thread;
using std::string;
using std::queue;
using std::condition_variable;
using std::unique_lock;
using std::pair;
using std::function;

typedef function<bool(const string&)> PortSetCallback;
typedef function<bool(bool)> BoolSetCallback;
typedef function<bool(int)> IntSetCallback;
typedef function<bool(double)> DoubleSetCallback;
typedef function<bool(const string&)> StringSetCallback;

typedef function<string()> PortGetCallback;
typedef function<bool()> BoolGetCallback;
typedef function<int()> IntGetCallback;
typedef function<double()> DoubleGetCallback;
typedef function<string()> StringGetCallback;

struct Port {
	enum TYPE {
		BOOL,
		INT,
		DOUBLE,
		STRING,
		MEMORY,
		BOOL_CALLBACK,
		INT_CALLBACK,
		DOUBLE_CALLBACK,
		STRING_CALLBACK,
		CALLBACK_FUNC,
		TYPE_END
	};

	string name;
	string disc;

	union Data {
		bool* b;
		int* i;
		double* d;
		string* s;
	}data;
	
	Memory** mem;

	Port::TYPE type;
	
	PortSetCallback psc;
	BoolSetCallback bsc;
	IntSetCallback isc;
	DoubleSetCallback dsc;
	StringSetCallback ssc;
	
	PortGetCallback pgc;
	BoolGetCallback bgc;
	IntGetCallback igc;
	DoubleGetCallback dgc;
	StringGetCallback sgc;
};

typedef map<const string, Port*> PortMap;

class Module {
public:
	enum COMMAND {
		INIT,
		RUN,
		STOP,
		FINISH,
		TURN_OFF,
		OPERATION_END
	};

	Module(Board * board);
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

	void lock();
	void unlock();

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


protected:

	bool m_bdebug;

	unsigned char m_status;

	double m_cf;

	thread m_th;

	condition_variable m_cv;

	Clock * m_clock;

	Board *m_board;

	PortMap m_ports;

	MemMap m_mems;

	virtual bool init_process() {
		return true;
	};
	virtual bool main_process() {
		return true;
	};
	virtual bool finish_process() {
		return true;
	};

	void register_bool(const string& name, const string& disc,
		bool init_status, bool* status);
	void register_int(const string& name, const string& disc,
		int init_val, int* val);
	void register_double(const string& name, const string& disc,
		double init_val, double* val);
	void register_string(const string& name, const string& disc,
		string init_str, string* str);
	void register_memory(const string& name, const string& disc,
		Memory** mem);

///////////Remove after few week/////////////////	
	void register_bool_callback(const string& name, const string& disc,
		BoolSetCallback bsc, BoolGetCallback bgc);
	void register_int_callback(const string& name, const string& disc,
		IntSetCallback isc, IntGetCallback igc);
	void register_double_callback(const string& name, const string& disc,
		DoubleSetCallback isc, DoubleGetCallback dgc);
	void register_string_callback(const string& name, const string& disc,
		StringSetCallback ssc, StringGetCallback sgc);
//////////////////////////////////////////////////

	void register_callback(const string& name, const string& disc,
		PortSetCallback psc, PortGetCallback pgc);

	void set_time(long long t);
	
	long long get_time();

	string get_time_as_string();
private:
	mutex m_lock;
};


