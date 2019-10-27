#include <iomanip>
#include <iostream>

#include "module.hpp"

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;


template class function<double()>;
template class function<void(double)>;
static unsigned module_id = 0;

#define FLAG_TURNED_ON 1
#define FLAG_INITIALIZED 2
#define FLAG_FINISHED 4


Module::Module(Board * board) : m_board(board), m_bdebug(false),
m_command(STOP), m_status(0x00),
m_module_id(module_id++){
	m_clock = m_board->get_clock();
	register_bool("debug", "debug flag(default no).", false, &m_bdebug);

	/*register_double_callback("cf", "clock frequency(default 10.0).",
		[&](double cf) {m_clock->set_clock_freq(cf); return true; },
		[&]() {return m_clock->get_clock_freq(); });*/

	PortSetCallback psc_cf = [&](const string& cf) {m_clock->set_clock_freq(stod(cf)); return true; };
	PortGetCallback pgc_cf = [&]() {return to_string(m_clock->get_clock_freq()); };
	register_callback("cf", "clock frequency(default 10.0)",
		psc_cf, pgc_cf);

}

void Module::init() {
	lock_guard<mutex> lock(m_lock);
	m_command = Module::COMMAND::INIT;
}

void Module::run() {
	lock_guard<mutex> lock(m_lock);
	m_command = Module::COMMAND::RUN;
}

void Module::turn_on() {
	lock_guard<mutex> lock(m_lock);
	m_th = thread(&Module::processing_loop, this);
	m_status |= FLAG_TURNED_ON;
}

void Module::turn_off() {
	unique_lock<mutex> lock(m_lock);
	m_command = TURN_OFF;
	lock.unlock();

	m_th.join();
	m_status = m_status & ~FLAG_TURNED_ON;
}

void Module::finish() {
	unique_lock<mutex> lock(m_lock);
	m_command = Module::COMMAND::FINISH;
}

void Module::processing_loop() {
	unique_lock<mutex> lock(m_lock);
	m_clock->start();
	lock.unlock();

	while (true) {
		long long sleep_time;
		{
			unique_lock<mutex> _lock(m_lock);
			
			switch (m_command) {
			case Module::COMMAND::INIT:
				if(init_process())
					m_status |= FLAG_INITIALIZED;
				m_cv.notify_one();
				m_command = Module::COMMAND::STOP;
				break;
			case Module::COMMAND::RUN:
				if (m_status & FLAG_INITIALIZED) {
					if (!main_process())
						m_command = Module::COMMAND::STOP;
				}
				break;
			case Module::COMMAND::STOP:
				break;
			case Module::COMMAND::FINISH:
				if (finish_process())
					m_status |= FLAG_FINISHED;

				m_command = Module::COMMAND::STOP;
				m_cv.notify_one();
				break;
			case Module::COMMAND::TURN_OFF:
				return;
			};

			m_clock->update();
			sleep_time = m_clock->get_sleep_time();
		}

		sleep_for(nanoseconds(sleep_time));
	}
}

void Module::stop() {
	unique_lock<mutex>(m_lock);
	m_command = Module::COMMAND::STOP;
}

void Module::wait_init_reply() {
	unique_lock<mutex> lock(m_lock);
	m_cv.wait(lock, [&] {return !(m_command == Module::COMMAND::INIT); });
}

void Module::wait_finish_reply() {
	unique_lock<mutex> lock(m_lock);
	m_cv.wait(lock, [&] {return !(m_command == Module::COMMAND::FINISH); });
}

bool Module::get_init_status() {
	lock_guard<mutex> lock(m_lock);
	return m_status & FLAG_INITIALIZED;
}

bool Module::get_finish_status() {
	lock_guard<mutex> lock(m_lock);
	return m_status & FLAG_FINISHED;
}

bool Module::get_main_sw_status() {
	lock_guard<mutex> lock(m_lock);
	return m_status & FLAG_TURNED_ON;
}

unsigned Module::get_id() {
	return m_module_id;
}

void Module::print(const string& str) {
	lock_guard<mutex> lock(m_board->get_lock_print());
	cout << str;
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
	port->bsc = bsc;
	port->bgc = bgc;
	port->type = Port::TYPE::BOOL_CALLBACK;
	m_ports.insert(pair<const string, Port*>(port->name, port));
}

void Module::register_int_callback(const string& name, const string& disc,
	IntSetCallback isc, IntGetCallback igc) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->isc = isc;
	port->igc = igc;
	port->type = Port::TYPE::INT_CALLBACK;
	m_ports.insert(pair<const string, Port*>(port->name, port));
}

void Module::register_double_callback(const string& name, const string& disc,
	DoubleSetCallback dsc, DoubleGetCallback dgc){
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->dsc = dsc;
	port->dgc = dgc;
	port->type = Port::TYPE::DOUBLE_CALLBACK;
	m_ports.insert(pair<const string, Port*>(port->name, port));
}

void Module::register_string_callback(const string& name, const string& disc,
	StringSetCallback ssc, StringGetCallback sgc) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->ssc = ssc;
	port->sgc = sgc;
	port->type = Port::TYPE::STRING_CALLBACK;
	m_ports.insert(pair<const string, Port*>(port->name, port));
}

void Module::register_callback(const string& name, const string& disc,
	PortSetCallback psc, PortGetCallback pgc) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->psc = psc;
	port->pgc = pgc;
	port->type = Port::TYPE::CALLBACK_FUNC;
	m_ports.insert(pair<const string, Port*>(port->name, port));
}


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
			(port->bsc)(true);
		}
		else if ("no" == data || "n" == data) {
			(port->bsc)(false);
		}
		else {
			return false;
		}
		return true;
	case Port::TYPE::INT_CALLBACK:
		return (port->isc)(stoi(data));
	case  Port::TYPE::DOUBLE_CALLBACK:
		return (port->dsc)(stod(data));
	case Port::TYPE::STRING_CALLBACK:
		return (port->ssc)(data);
	case Port::TYPE::CALLBACK_FUNC:
		return port->psc(data);
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
		data = bool_to_str((port->bgc)());
		return true;
	case Port::TYPE::INT_CALLBACK:
		data = to_string((port->igc)());
		return true;
	case Port::TYPE::DOUBLE_CALLBACK:
		data = to_string((port->dgc)());
		return true;
	case Port::TYPE::STRING_CALLBACK:
		data = (port->sgc)();
		return true;
	case Port::TYPE::CALLBACK_FUNC:
		data = (port->pgc)();
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

void Module::set_time(long long t) {
	//m_board->lock();
	//m_board->set_time(t);
	//m_board->unlock();
}

long long Module::get_time() {
	return m_clock->get_time();
}

string Module::get_time_as_string() {
	return to_time_string(get_time());
}
