#include "module.hpp"

using namespace std;

Module::Module() :  m_brun(false) {
}

void Module::run() {
	m_brun = true;
	m_th = thread(&Module::processing_loop, this);
}

void Module::join() {
	m_th.join();
}

void Module::processing_loop() {
	m_clock.start();

	while (true) {

		{
			unique_lock<mutex> lock(m_lock);
			if (!(process() && m_brun))
				break;
		}

		m_clock.adjust();
	}

	m_clock.stop();
}

void Module::stop() {
	unique_lock<mutex>(m_lock);
	m_brun = false;
}


void Module::register_port(const string& name, const  string& disc,
	bool init_status, bool* status) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->type = Port::TYPE::BOOL;

	*status = init_status;
	port->pdata.b = status;
	
	m_ports.insert(pair<const string, Port*>(port->name, port));
}

void Module::register_port(const string& name, const string& disc,
	int init_val, int* val) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->type = Port::TYPE::INT;

	*val = init_val;
	port->pdata.i = val;

	m_ports.insert(pair<const string, Port*>(port->name, port));
}

void Module::register_port(const string& name, const string& disc,
	double init_val, double* val) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->type = Port::TYPE::DOUBLE;

	*val = init_val;
	port->pdata.d = val;

	m_ports.insert(pair<const string, Port*>(port->name, port));
}

void Module::register_port(const string& name, const string& disc,
	string init_str, string* str) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->type = Port::TYPE::STRING;

	*str = init_str;
	port->pdata.s = str;

	m_ports.insert(pair<const string, Port*>(port->name, port));
}

void Module::register_port(const string& name, const string& disc, Memory** mem) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->mem = mem;
	port->type = Port::TYPE::MEMORY;
	m_ports.insert(pair<const string, Port*>(port->name, port));
}


//Port* Module::get_port(const string& name) {
//	return m_ports[name];
//}

bool Module::connect_memory(Memory* memory, const string& port_name) {
	unique_lock<mutex> lock(m_lock);

	PortMap::iterator pm_it = m_ports.find(port_name);
	if (pm_it == m_ports.end())
		return false;

	Port* port = pm_it->second;
	if (port->type != Port::TYPE::MEMORY)
		return false;

	*(port->mem) = memory;
	return true;
}

bool Module::set_data(const string& name, const string& data) {
	unique_lock<mutex> lock(m_lock);

	PortMap::iterator pm_it = m_ports.find(name);
	if (pm_it == m_ports.end())
		return false;

	Port* port = pm_it->second;
	switch (port->type) {
	case Port::TYPE::BOOL:
		return str_to_bool(data, *(port->pdata.b));
	case Port::TYPE::INT:
		(*port->pdata.i) = stoi(data);
		return true;
	case Port::TYPE::DOUBLE:
		(*port->pdata.d) = stod(data);
		return true;
	case Port::TYPE::STRING:
		(*port->pdata.s) = data;
		return true;
	default:
		return false;
	}
}

bool Module::get_data(const string& name, string& data) {
	unique_lock<mutex> lock(m_lock);

	PortMap::iterator pm_it = m_ports.find(name);
	if (pm_it == m_ports.end())
		return false;

	Port* port = pm_it->second;
	switch (port->type) {
	case Port::TYPE::BOOL:
		data = bool_to_str((*port->pdata.b));
		return true;
	case Port::TYPE::INT:
		data = to_string((*port->pdata.i));
		return true;
	case Port::TYPE::DOUBLE:
		data = to_string((*port->pdata.d));
		return true;
	case Port::TYPE::STRING:
		data = (*port->pdata.s);
		return true;
	default:
		return false;
	}
}

void Module::get_port_names_and_discs(vector<pair<string, string> >& names_and_discs) {
	unique_lock<mutex> lock(m_lock);

	names_and_discs.reserve(m_ports.size());

	for each(pair<string, Port*> port in m_ports){
		names_and_discs.push_back(pair<string, string>(port.second->name, port.second->disc));
	}
}

void Module::lock() {
	m_lock.lock();
}

void Module::unlock() {
	m_lock.unlock();
}

bool Module::is_ready() {
	return m_ready;
}