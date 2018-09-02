#pragma once
//#include <map>
#include <string>
#include <thread>
#include <queue>
#include <condition_variable>

#include "../common/miscel.hpp"
#include "../memory/memory.hpp"
#include "../board.hpp"

using std::thread;
using std::string;
using std::queue;
using std::condition_variable;
using std::unique_lock;
using std::pair;

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
		TYPE_END
	};

	string name;
	string disc;
	//MemPtr mem;
	Memory** mem;
	union Ptr {
		bool* b;
		int* i;
		double* d;
		string* s;
	}pdata;

	Port::TYPE type;
};

struct SignalPort : public Port {

};

typedef map<const string, Port*> PortMap;

class Module {
public:

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

	bool connect_port(const string& port_name, const string& mem_name);

	bool set_data(const string& name, const string& data);
	bool get_data(const string& name, string& data);

	void get_port_names_and_discs(vector<pair<string, string> >& names_and_discs);

	//Port* get_port(const string& name);

	//const PortMap* get_ports();

	void lock();
	void unlock();

protected:
	bool m_brun;
	bool m_bwait;
	bool m_bfinish;
	thread m_th;

	condition_variable m_run;

	Clock m_clock;

	Board *m_board;

	PortMap m_ports;

	MemMap m_mems;
	//void register_port(const string& name, const string& disc,
	//	const string& data, MEM_TYPE mem_type, Memory** mem);
	//void register_port(const string& name, const string& disc,
	//	bool status, MemBool** mem);
	//void register_port(const string& name, const string& disc,
	//	int value, MemInt** mem);
	//void register_port(const string& name, const string& disc,
	//	const string& str, MemString** mem);
	void register_port(const string& name, const string& disc,
		bool init_status, bool* status);
	void register_port(const string& name, const string& disc,
		int init_val, int* val);
	void register_port(const string& name, const string& disc,
		double init_val, double* val);
	void register_port(const string& name, const string& disc,
		string init_str, string* str);
	void register_port(const string& name, const string& disc,
		Memory** mem);
private:
	mutex m_lock;
};


