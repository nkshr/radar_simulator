#pragma once
//#include <map>
#include <set>
#include <string>
#include <typeinfo>
#include <mutex>

#include "../common/clock.hpp"
#include "../common/miscel.hpp"

#include "../board.hpp"
#include "../signal/signal.hpp"

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

	bool set_data(bool value);
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

union MemPtr {
	MemInt* mem_int;
};


struct Port {
	string name;
	string disc;
	//MemPtr mem;
	MEM_TYPE mem_type;
	Memory** mem;
};

struct SignalPort : public Port {

};

typedef map<const string, Port*> PortMap;
typedef map<const string, Memory*> MemMap;

class Module {
public:

	Module();
	void start();
	void join();
	void processing_loop();
	void stop();

	virtual bool init() = 0;
	virtual bool process() = 0;

	//Port* get_port(const string& name);

	bool connect_port(const string& port_name, const string& mem_name);

protected:
	bool m_brun;

	thread m_th;

	Clock m_clock;

	Board *m_board;

	PortMap m_ports;
	MemMap m_mems;
	void register_port(const string& name, const string& disc,
		const string& data, MEM_TYPE mem_type, Memory** mem);
	void register_port(const string& name, const string& disc,
		int value, MemInt** mem);
	void register_port(const string& name, const string& disc,
		const string& str, MemString** mem);

	Port* get_port(const string& name);
	bool set_data_to_port(const string& name, const string& value);

	void lock();
	void unlock();

private:
	mutex m_lock;
};


