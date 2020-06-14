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
#define FLAG_STOPPED 8

bool smpl_bsc(const string& in, bool* out) {
	return str_to_bool(in, *out);
}

bool smpl_isc(const string& in, int* out) {
	*out = stoi(in);
	return true;
}

bool smpl_dsc(const string& in, double* out) {
	*out = stod(in);
	return true;
}

bool smpl_ssc(const string& in, string* out) {
	*out = in;
	return  true;
}

bool smpl_esc(const string& in, const vector<string>&  strs, void* out) {
	vector<string>::const_iterator it = find(strs.begin(), strs.end(),in);
	if (it != strs.end()) {
		int index = distance(strs.begin(), it);
		*(static_cast<int*>(out)) = index;
		return true;
	}
	
	return false;
}

string smpl_bgc(const bool* in) {
	return bool_to_str(*in);
}

string smpl_igc(const int* in) {
	return to_string(*in);
}

string smpl_dgc(const double* in) {
	return to_string(*in);
}

string smpl_sgc(const string* in) {
	return *in;
}

string smpl_egc(const vector<string>& strs, const void* in) {
	int index = *static_cast<const int*>(in);
	return strs[index];
}

Module::Module(const string&name, Board * board) : m_name(name), m_board(board), m_bdebug(false),
m_command(BE_IDLE), m_status(0x00),
m_module_id(module_id++){
	m_clock = m_board->get_clock();
	register_bool("debug", "debug flag(default no).", false, &m_bdebug);

	PortSetCallback psc_cf = [&](const string& cf) {m_clock->set_clock_freq(stod(cf)); return true; };
	PortGetCallback pgc_cf = [&]() {return to_string(m_clock->get_clock_freq()); };
	register_callback("cf", "clock frequency(default 10.0)",
		psc_cf, pgc_cf);

	PortGetCallback pgc_pr= [&]() {
		return m_clock->get_info_str();
	};
	register_callback("proc_rate", "processing rate(read only)",
		nullptr, pgc_pr);
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
				m_command = Module::COMMAND::BE_IDLE;
				break;
			case Module::COMMAND::RUN:
				
				if (m_status & FLAG_INITIALIZED) {
					if (!main_process())
						m_command = Module::COMMAND::BE_IDLE;
				}
				break;
			case Module::COMMAND::STOP:
				m_status |= FLAG_STOPPED;
				print(m_name + " " + m_clock->get_info_str() + "\n");
				m_command = Module::COMMAND::BE_IDLE;
			case Module::COMMAND::BE_IDLE:
				break;
			case Module::COMMAND::FINISH:
				if (finish_process())
					m_status |= FLAG_FINISHED;

				print(m_name + " " + m_clock->get_info_str() + "\n");
				m_command = Module::COMMAND::BE_IDLE;
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


void Module::register_memory(const string& name, const string& disc, Memory** mem) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->mem = mem;
	port->type = Port::TYPE::MEMORY;
	m_ports.insert(pair<const string, Port*>(port->name, port));
}

//template <typename T>
//void Module::register_enum(const string& name, const string& disc,
//	int init_val, void* val, const vector<string>& strs, EnumSetCallback sc, EnumGetCallback gc) {
//	Port* port = new Port;
//	port->name = name;
//	port->disc = disc;
//	port->data.e = val;
//	*static_cast<int*>(val) = init_val;
//	port->type = Port::TYPE::ENUM;
//	port->esc = sc;
//	port->egc = gc;
//	port->strs = strs;
//	m_ports.insert(pair<const string, Port*>(port->name, port));
//
//}

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
		return port->bsc(data, port->data.b);
	case Port::TYPE::INT:
		return port->isc(data, port->data.i);
	case Port::TYPE::DOUBLE:
		return port->dsc(data, port->data.d);
	case Port::TYPE::STRING:
		return port->ssc(data, port->data.s);
	case Port::TYPE::ENUM:
		return port->esc(data, port->strs, port->data.e);
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
		data = port->bgc(port->data.b);
		return true;
	case Port::TYPE::INT:
		data = port->igc((port->data.i));
		return true;
	case Port::TYPE::DOUBLE:
		data = port->dgc((port->data.d));
		return true;
	case Port::TYPE::STRING:
		data = port->sgc(port->data.s);
		return true;
	case Port::TYPE::ENUM:
		data = (port->egc(port->strs, port->data.e));
		return true;
	case Port::TYPE::CALLBACK_FUNC:
		data = (port->pgc());
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
