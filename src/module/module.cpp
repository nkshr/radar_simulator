#include "module.hpp"

Module::Port::Port(){
}

void Module::Port::lock() {
	m_lock.lock();
}

void Module::Port::unlock() {
	m_lock.unlock();
}

const string& Module::Port::get_discription() const{
	return m_disc;
}

bool InInt::set_value(const string& value) {
	lock();
	m_value = stoi(value);
	unlock();
	return true;
}

void InInt::set_value(const int value) {
	lock();
	m_value = value;
	unlock();
}

int InInt::get_value() {
	return m_value;
}


bool OutInt::connect(InPort* port) {
	m_to = dynamic_cast<InInt*>(port);
	if (m_to)
		return true;
	else
		return false;
}

void OutInt::set_value(const int value) {
	m_to->set_value(value);
}

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

InPort* Module::get_in_port(const string& name) {
	return m_in_ports[name];
}

OutPort* Module::get_out_port(const  string& name) {
	return m_out_ports[name];
}


bool Module::set_port(const string& name, const string& value) {
	return  m_in_ports[name]->set_value(value);
}

//bool Module::register_port(const string& name, const string& disc, const type_info& tf, void* value) {
//	if (tf == typeid(int)) {
//		Port* p = dynamic_cast<Port*>(new IntPort(disc, value));
//		m_ports.insert(pair<const string, Port*>(name, p));
//	}
//	else {
//		return false;
//	}
//	return true;
//}

//void Module::lock() {
//	m_lock.lock();
//}
//
//void Module::unlock() {
//	m_lock.unlock();
//}

void Module::register_port(const string& name, Port* p) {
	m_vars.insert(pair<const string, Port*>(name, p));
}