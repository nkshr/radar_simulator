#include "module.hpp"

using namespace std;

template class function<double()>;
template class function<void(double)>;

Module::Module() :  m_brun(true), m_bdebug(false), m_status(Module::STATUS::CREATED) {
	m_clock = m_board->get_clock();
	register_bool("debug", "debug flag(default no).", false, &m_bdebug);
	register_double_callback("cf", "clock frequency(default 10.0).",
		[&](double cf) {m_clock->set_clock_freq(cf); return true; },
		[&]() {return m_clock->get_clock_freq(); });
}

void Module::run() {
	m_th = thread(&Module::processing_loop, this);

}

void Module::join() {
	m_th.join();
}

void Module::processing_loop() {
	m_clock->start();

	while (true) {

		{
			unique_lock<mutex> lock(m_lock);
			if (!(process() && m_brun))
				break;
		}

		m_clock->adjust();
	}

	m_clock->stop();
}

void Module::stop() {
	unique_lock<mutex>(m_lock);
	m_brun = false;
}


void Module::register_bool(const string& name, const  string& disc,
	bool init_status, bool* status) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->type = Port::TYPE::BOOL;

	*status = init_status;
	port->data.b = status;
	
	m_ports.insert(pair<const string, Port*>(port->name, port));
}

void Module::register_int(const string& name, const string& disc,
	int init_val, int* val) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->type = Port::TYPE::INT;

	*val = init_val;
	port->data.i = val;

	m_ports.insert(pair<const string, Port*>(port->name, port));
}

void Module::register_double(const string& name, const string& disc,
	double init_val, double* val) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->type = Port::TYPE::DOUBLE;

	*val = init_val;
	port->data.d = val;

	m_ports.insert(pair<const string, Port*>(port->name, port));
}

void Module::register_string(const string& name, const string& disc,
	string init_str, string* str) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->type = Port::TYPE::STRING;

	*str = init_str;
	port->data.s = str;

	m_ports.insert(pair<const string, Port*>(port->name, port));
}

void Module::register_memory(const string& name, const string& disc, Memory** mem) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->mem = mem;
	port->type = Port::TYPE::MEMORY;
	m_ports.insert(pair<const string, Port*>(port->name, port));
}

void Module::register_bool_callback(const string& name, const string& disc,
	BoolSetCallback bsc, BoolGetCallback bgc) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->sc.b = new BoolSetCallback(bsc);
	port->gc.b = new BoolGetCallback(bgc);
	port->type = Port::TYPE::BOOL_CALLBACK;
	m_ports.insert(pair<const string, Port*>(port->name, port));
}

void Module::register_int_callback(const string& name, const string& disc,
	IntSetCallback isc, IntGetCallback igc) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->sc.i = new IntSetCallback(isc);
	port->gc.i = new IntGetCallback(igc);
	port->type = Port::TYPE::INT_CALLBACK;
	m_ports.insert(pair<const string, Port*>(port->name, port));
}

void Module::register_double_callback(const string& name, const string& disc,
	DoubleSetCallback dsc, DoubleGetCallback dgc){
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->sc.d = new DoubleSetCallback(dsc);
	port->gc.d = new DoubleGetCallback(dgc);
	port->type = Port::TYPE::DOUBLE_CALLBACK;
	m_ports.insert(pair<const string, Port*>(port->name, port));
}

void Module::register_string_callback(const string& name, const string& disc,
	StringSetCallback ssc, StringGetCallback sgc) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->sc.s = new StringSetCallback(ssc);
	port->gc.s = new StringGetCallback(sgc);
	port->type = Port::TYPE::STRING_CALLBACK;
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
		if ("yes" == data || "y" == data) {
			*port->data.b = true;
		}
		else if ("no" == data || "n" == data) {
			*port->data.b = false;
		}
		else {
			return false;
		}
		return true;
	case Port::TYPE::INT:
		(*port->data.i) = stoi(data);
		return true;
	case Port::TYPE::DOUBLE:
		(*port->data.d) = stod(data);
		return true;
	case Port::TYPE::STRING:
		(*port->data.s) = data;
		return true;
	case Port::TYPE::BOOL_CALLBACK:
		if ("yes" == data || "y" == data) {
			(*port->sc.b)(true);
		}
		else if ("no" == data || "n" == data) {
			(*port->sc.b)(false);
		}
		else {
			return false;
		}
		return true;
	case Port::TYPE::INT_CALLBACK:
		return (*port->sc.i)(stoi(data));
	case  Port::TYPE::DOUBLE_CALLBACK:
		return (*port->sc.d)(stod(data));
	case Port::TYPE::STRING_CALLBACK:
		return (*port->sc.s)(data);
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
		data = bool_to_str((*port->data.b));
		return true;
	case Port::TYPE::INT:
		data = to_string((*port->data.i));
		return true;
	case Port::TYPE::DOUBLE:
		data = to_string((*port->data.d));
		return true;
	case Port::TYPE::STRING:
		data = (*port->data.s);
		return true;
	case Port::TYPE::BOOL_CALLBACK:
		data = bool_to_str((*port->gc.b)());
		return true;
	case Port::TYPE::INT_CALLBACK:
		data = to_string((*port->gc.i)());
		return true;
	case Port::TYPE::DOUBLE_CALLBACK:
		data = to_string((*port->gc.d)());
		return true;
	case Port::TYPE::STRING_CALLBACK:
		data = (*port->gc.s)();
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

Module::STATUS Module::get_status() {
	return m_status;
}

void Module::set_status(STATUS status) {
	m_status = status;
}

void Module::set_time(long long t) {
	m_board->lock();
	m_board->set_time(t);
	m_board->unlock();
}

long long Module::get_time() {
	return m_clock->get_system_time();
}