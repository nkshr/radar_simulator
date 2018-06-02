#pragma once
#include <map>
#include <set>
#include <string>
#include <typeinfo>
#include <mutex>

#include "../common/clock.hpp"
#include "../common/miscel.hpp"

#include "../board.hpp"
#include "../signal/signal.hpp"



//class DoublePort : public Port {
//public:
//
//	double m_var;
//	virtual bool set_value(const string& value) {
//		m_var = atof(value.c_str());
//		return true;
//	}
//};

//class SignalPort : public Port {
//public:
//	virtual bool set_value(const string& value);
//};

class InPort;
class OutPort;

class Module {
public:
	class Port;

	Module();
	void start();
	void join();
	void processing_loop();
	void stop();

	virtual bool process() = 0;

	Port* get_port();
	InPort* get_in_port(const string& name);
	OutPort* get_out_port(const string &name);

protected:
	bool m_brun;

	thread m_th;

	Clock m_clock;

	Board *m_board;

	map<const string, InPort*> m_in_ports;
	map<const string, OutPort*> m_out_ports;

	void register_port(const string& name, Port* p);

	bool set_port(const string& name, const string& value);

	//void lock();
	//void unlock();

private:
	mutex m_lock;
	map<const string, Port*> m_vars;
};

class Module::Port{
public:
	Port();
	Port(const string& disc, void* value);

	const string& get_discription() const;

private:
	mutex m_lock;

	string m_disc;

protected:
	void lock();
	void unlock();
};

class InPort : public Module::Port {
public:
	InPort();
	InPort(const string& disc, void* value);
	virtual bool set_value(const string& value) = 0;
};

class OutPort : public Module::Port {
public:
	OutPort();
	virtual bool connect(InPort* port);
};

class InInt : public InPort {
public:
	InInt(const string& disc, void* value);
	virtual bool set_value(const string& value);
	void set_value(const int value);

	int get_value();

private:
	int m_value;
};

class OutInt : public OutPort {
public:
	virtual bool connect(InPort* port);
	void set_value(const int value);
private:
	InInt* m_to;
};


