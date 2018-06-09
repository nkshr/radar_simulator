#include "module.hpp"

MemType Memory::get_type() const {
	return m_mem_type;
}

void Memory::enable_rom(bool rom) {
	m_brom = rom;
}

bool MemInt::set_value(const string& value) {
	return true;
}

bool Memory::is_rom() const {
	return m_brom;
}

void MemInt::set_value(int value) {
	m_lock.lock();
	m_value = value;
	m_lock.unlock();
}

int MemInt::get_value() {
	Mutex lock(&m_lock);
	return m_value;
}

//string& Port::get_name() {
//	return m_name;
//}
//
//int PortInt::get_value() {
//	return m_mem.get_value();
//}
//
//void PortInt::set_value(int value) {
//	m_mem.set_value(value);
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

void Module::start() {
	m_th = thread(&Module::processing_loop, this);
}

void Module::join() {
	m_th.join();
}

void Module::processing_loop() {
	m_clock.start();
	
	while (true) {
		if (!m_brun)
			break;

		if (!process()) {
			break;
		}

		m_clock.adjust();
	}
}

void Module::stop() {
	m_brun = false;
}


void Module::register_port(const string& name, const string& disc,
	MemType mem_type, Memory** mem) {
	Port* port = new Port;
	port->name = name;
	port->disc = disc;
	port->mem_type = mem_type;
	port->mem = mem;
	m_ports.insert(pair<const string, Port*>(port->name, port));
}

Port* Module::get_port(const string& name) {
	return m_ports[name];
}

bool Module::connect_port(const string& port_name, const string& mem_name) {
	PortMap::iterator pm_it  = m_ports.find(port_name);
	if (pm_it == m_ports.end())
		return false;
	Port* port = pm_it->second;
	
	MemMap::iterator mm_it = m_mems.find(mem_name);
	if (mm_it == m_mems.end())
		return false;
	Memory* mem = mm_it->second;

	switch (port->mem_type) {
	case MT_INT:
		if (MT_INT != mem->get_type())
			return false;
		*port->mem = mem;
		return true;
	default:
		return false;
	}
}

bool Module::set_port(const string& name, const string& value) {
	PortMap::iterator pm_it = m_ports.find(name);
	if (pm_it == m_ports.end())
		return false;
	Memory*& mem = *(pm_it->second->mem);

	if (mem != nullptr && mem->is_rom()) {
		delete mem;
	}

	switch (pm_it->second->mem_type) {
	case MT_INT:
		mem = (Memory*)(new MemInt());
		break;
	default:
		return false;
	}

	if (!mem->set_value(value)) {
		delete mem;
		return false;
	}
	mem->enable_rom(true);
	return true;
}

void Module::lock() {
	//m_lock.lock();
}

void Module::unlock() {
	//m_lock.unlock();
}
