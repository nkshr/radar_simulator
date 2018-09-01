#pragma once
//#include <map>
#include <string>
#include <thread>
#include <queue>
#include <condition_variable>

using std::thread;
using std::string;
using std::queue;
using std::condition_variable;
using std::unique_lock;
using std::pair;
//#include "../common/clock.hpp"
#include "../common/miscel.hpp"

#include "../board.hpp"

enum MEM_TYPE {
	MT_BOOL,
	MT_INT,
	MT_FLOAT,
	MT_DOUBLE,
	MT_STRING,

};

class Memory {
public:
	virtual bool set_data(const string& value) = 0;
	//virtual void write(char* buf, int buf_size);
	//virtual bool read(const char* buf, int buf_size);
	virtual string get_data();

	bool is_rom() const;
	MEM_TYPE get_type() const;
	void enable_rom(bool rom);

protected:
	mutex m_lock;

private:
	string m_name;
	bool m_brom;
	MEM_TYPE m_mem_type;
	
};

class MemInt : public Memory{
public:
	//MemInt(int vlaue);
	virtual bool set_data(const string& value);
	void set_value(int value);

	int get_value();
private:
	int m_value;
};

class MemBool : public Memory {
public:
	virtual bool set_data(const string& value);
	virtual string get_data();

	void set_status(bool status);
	bool get_status();
private:
	bool m_status;
};

class MemString : public Memory {
public:
	virtual bool set_data(const string& value); 
	void set_string(const string& str);
	string& get_string();
private:
	string m_string;
};

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
		MEM,
		TYPE_END
	};

	string name;
	string disc;
	//MemPtr mem;
	MEM_TYPE mem_type;
	Memory** mem;
	union Ptr {
		bool* b;
		int* i;
		double* d;
		string* s;
	}pdata;
	Port::TYPE pt;
};

struct SignalPort : public Port {

};

typedef map<const string, Port*> PortMap;
typedef map<const string, Memory*> MemMap;

class Module {
public:

	Module();
	void run();
	void join();
	void processing_loop();
	void stop();

	virtual bool init() = 0;
	virtual bool finish() {
		return true;
	};
	virtual bool process() = 0;
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
	void register_port(const string& name, const string& disc,
		bool status, MemBool** mem);
	void register_port(const string& name, const string& disc,
		int value, MemInt** mem);
	void register_port(const string& name, const string& disc,
		const string& str, MemString** mem);
	void register_port(const string& name, const string& disc,
		bool init_status, bool* status);
private:
	mutex m_lock;
};


