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

enum MemType {
	MT_INT,
	MT_FLOAT,
	MT_DOUBLE,
	MT_STRING,
};

class Memory {
public:
	virtual bool set_value(const string& value) = 0;
	bool is_rom() const;
	MemType get_type() const;
	void enable_rom(bool rom);

protected:
	mutex m_lock;

private:
	string m_name;
	bool m_brom;
	MemType m_mem_type;
	
};

class MemInt : public Memory{
public:
	virtual bool set_value(const string& value);
	void set_value(int value);
	int get_value();
private:
	int m_value;
};

union MemPtr {
	MemInt* mem_int;
};

//class Port {
//public:
//	string& get_name();
//	virtual bool set_value(const string& value) = 0;
//	virtual void update();
//
//	void add_connection(Port* port);
//
//protected:
//	void lock();
//	void unlock();
//	string m_name;
//	vector<Port*> m_dests;
//
//private:
//	mutex* m_lock;
//};

//class PortInt : public Port {
//private:
//	int m_value;
//	MemInt m_mem;
//
//public:
//	int get_value();
//	//virtual void set_value(int value);
//	virtual void update();
//
//	void set_value(int value);
//};

struct Port {
	string name;
	string disc;
	//MemPtr mem;
	MemType mem_type;
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
		MemType mem_type, Memory** mem);

	Port* get_port(const string& name);
	bool set_port(const string& name, const string& value);

	void lock();
	void unlock();

private:
	mutex m_lock;
};


