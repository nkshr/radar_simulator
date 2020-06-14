#include "module_base.hpp"

void ModuleBase::register_bool(const string& name, const  string& disc,
	bool init_status, bool* status, BoolSetCallback sc, BoolGetCallback gc) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->type = Port::TYPE::BOOL;

	*status = init_status;
	port->data.b = status;
	port->bsc = sc;
	port->bgc = gc;

	m_ports.insert(pair<const string, Port*>(port->name, port));
}

void ModuleBase::register_int(const string& name, const string& disc,
	int init_val, int* val, IntSetCallback sc, IntGetCallback gc) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->type = Port::TYPE::INT;

	*val = init_val;
	port->data.i = val;

	port->isc = sc;
	port->igc = gc;

	m_ports.insert(pair<const string, Port*>(port->name, port));
}

void ModuleBase::register_double(const string& name, const string& disc,
	double init_val, double* val, DoubleSetCallback sc, DoubleGetCallback gc) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->type = Port::TYPE::DOUBLE;

	*val = init_val;
	port->data.d = val;

	port->dsc = sc;
	port->dgc = gc;

	m_ports.insert(pair<const string, Port*>(port->name, port));
}

void ModuleBase::register_string(const string& name, const string& disc,
	string init_str, string* str, StringSetCallback sc, StringGetCallback gc) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->type = Port::TYPE::STRING;

	*str = init_str;
	port->data.s = str;

	port->ssc = sc;
	port->sgc = gc;

	m_ports.insert(pair<const string, Port*>(port->name, port));
}

void ModuleBase::lock() {
	m_lock.lock();
}

void ModuleBase::unlock() {
	m_lock.unlock();
}
