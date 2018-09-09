#pragma once
//#include <map>
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

typedef function<void(bool)> BoolSetCallback;
typedef function<void(int)> IntSetCallback;
typedef function<void(double)> DoubleSetCallback;
typedef function<void(string)> StringSetCallback;

typedef function<bool()> BoolGetCallback;
typedef function<int()> IntGetCallback;
typedef function<double()> DoubleGetCallback;
typedef function<string()> StringGetCallback;

struct LatLon {
	double deg;
	double min;
	double sec;
};

struct Ship {
	int mmsi;
	double x, y, z;
	LatLon lat, lon;
};


union MemPtr {
	MemInt* mem_int;
};


struct Port {
	enum TYPE{
		BOOL,
		INT,
		DOUBLE,
		STRING,
		MEMORY,
		BOOL_CALLBACK,
		INT_CALLBACK,
		DOUBLE_CALLBACK,
		STRING_CALLBACK,
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
	
	union SetCallback {
		BoolSetCallback* b;
		IntSetCallback* i;
		DoubleSetCallback* d;
		StringSetCallback* s;
	}sc;

	union GetCallback {
		BoolGetCallback* b;
		IntGetCallback* i;
		DoubleGetCallback* d;
		StringGetCallback* s;
	}gc;

	Memory** mem;

	Port::TYPE type;
	

};

struct SignalPort : public Port {

};

typedef map<const string, Port*> PortMap;

class Module {
public:
	enum STATUS {
		CREATED,
		INITIALIZED,
		RUNNING,
		STOPPING,
		STOPPED,
		FINISHED,
		STATUS_END
	};

	Module();
	virtual ~Module() {};
	void run();
	void join();
	void processing_loop();
	void stop();

	virtual bool init() { 
		return true; 
	};
	virtual bool process() {
		return true;
	};
	virtual bool finish() { 
		return true;
	};
	//Port* get_port(const string& name);

	bool connect_memory(Memory* memory, const string& port);
	bool set_data(const string& name, const string& data);
	bool get_data(const string& name, string& data);

	void get_port_names_and_discs(vector<pair<string, string> >& names_and_discs);

	//Port* get_port(const string& name);

	//const PortMap* get_ports();

	void lock();
	void unlock();

	STATUS get_status();
	void set_status(STATUS);

protected:
	bool m_brun;

	STATUS m_status;

	double m_cf;

	thread m_th;

	Clock m_clock;

	Board *m_board;

	PortMap m_ports;

	MemMap m_mems;

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
	void register_bool_callback(const string& name, const string& disc,
		BoolSetCallback bsc, BoolGetCallback bgc);
	void register_int_callback(const string& name, const string& disc,
		IntSetCallback isc, IntGetCallback igc);
	void register_double_callback(const string& name, const string& disc,
		DoubleSetCallback isc, DoubleGetCallback dgc);
	void register_string_callback(const string& name, const string& disc,
		StringSetCallback ssc, StringGetCallback sgc);


private:
	mutex m_lock;
};


