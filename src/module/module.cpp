#include "module.hpp"

using namespace std;

//string& Port::get_name() {
//	return m_name;
//}
//
//int PortInt::get_value() {
//	return m_mem.get_value();
//}
//
//void PortInt::set_data(int value) {
//	m_mem.set_data(value);
//}
//
//void Port::add_connection(Port* port) {
//	m_dests.push_back(port);
//	m_lock = port->m_lock;
//}
//
//void Port::lock() {
//	m_lock->lock();
//}
//
//void Port::unlock() {
//	m_lock->unlock();
//}

Module::Module() : m_brun(false) {
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

//void Module::register_port(const string& name, const string& disc,
//	bool status, MemBool** mem) {
//	Port* port = new Port;
//	port->name = name;
//	port->disc = disc;
//	port->mem = (Memory**)mem;
//	
//	MemBool* tmp = new MemBool();
//	tmp->set_status(status);
//	*port->mem = tmp;
//
//	m_ports.insert(pair<const string, Port*>(port->name, port));
//}


//void Module::register_port(const string& name, const string& disc,
//	const string& value, MEM_TYPE mem_type, Memory** mem) {
//	Port* port = new Port;
//	port->name = name;
//	port->disc = disc;
//	port->mem_type = mem_type;
//	port->mem = mem;
//	m_ports.insert(pair<const string, Port*>(port->name, port));
//	set_data_to_port(port->name, value);
//}

//void Module::register_port(const string& name, const string& disc,
//	int value, MemInt** mem) {
//	Port* port = new Port;
//	port->name = name;
//	port->disc = disc;
//	port->mem = (Memory**)(mem);
//	MemInt* tmp = new MemInt();
//	tmp->set_value(value);
//	*port->mem = tmp;
//	m_ports.insert(pair<const string, Port*>(port->name, port));
//}
//
//void Module::register_port(const string& name, const string& disc,
//	const string& str, MemString** mem) {
//	Port* port = new Port;
//	port->name = name;
//	port->disc = disc;
//	port->mem = (Memory**)(mem);
//	MemString* tmp = new MemString();
//	tmp->set_string(str);
//	*port->mem = tmp;
//	m_ports.insert(pair<const string, Port*>(port->name, port));
//}

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

bool Module::connect_port(const string& port_name, const string& mem_name) {
	PortMap::iterator pm_it  = m_ports.find(port_name);
	if (pm_it == m_ports.end())
		return false;
	Port* port = pm_it->second;
	
	MemMap::iterator mm_it = m_mems.find(mem_name);
	if (mm_it == m_mems.end())
		return false;
	Memory* mem = mm_it->second;

	port->mem = &mem;
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

	//PortMap::iterator pm_it = m_ports.find(name);
	//if (pm_it == m_ports.end())
	//	return false;
	//Memory*& mem = *(pm_it->second->mem);

	//if (mem != nullptr && mem->is_rom()) {
	//	delete mem;
	//}

	//switch (pm_it->second->mem_type) {
	//case MT_INT:
	//	mem = (Memory*)(new MemInt());
	//	break;
	//case MT_BOOL:
	//	mem = (Memory*)(new MemBool());
	//	break;
	//default:
	//	return false;
	//}
	//if (!mem->set_data(value)) {
	//	delete mem;
	//	return false;
	//}
	//mem->enable_rom(true);
	//return true;
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
