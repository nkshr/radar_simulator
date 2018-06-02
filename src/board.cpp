#include <thread>
#include <future>
#include <mutex>
#include <iostream>
#include <string>
#include <sstream>
#include  <map>

#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

#include "module/simulator.hpp"
#include "common/miscel.hpp"

#include "board.hpp"

using namespace std;

Board::Board() {
	m_cmd_server.set_server("", 8080);
}

bool Board::init() {
	if(!m_cmd_server.init())
		return false;

	register_module<Simulator>("simulator");

	return true;
}

void Board::run() {
	start_all();
	listen();
	
	for (mmap::iterator it = m_modules.begin(); it != m_modules.end(); ++it) {
		it->second->join();
	}
}

bool Board::start(const string& vname) {
	for (mmap::iterator it = m_modules.begin(); it != m_modules.end(); ++it) {
		if (it->first == vname) {
			it->second->start();
			return true;
		}
	}

	return false;
}
void Board::start_all() {
	for (mmap::iterator it = m_modules.begin(); it != m_modules.end(); ++it) {
		it->second->start();
	}
}

void Board::stop_all() {
	for (mmap::iterator it = m_modules.begin(); it != m_modules.end(); ++it) {
		it->second->stop();
	}
}

bool Board::stop(const string& vname) {
	for (mmap::iterator it = m_modules.begin(); it != m_modules.end(); ++it) {
		if (it->first == vname) {
			it->second->stop();
			return true;
		}
	}
	return false;
}

void Board::lock() {
	m_lock.lock();
}

void Board::unlock() {
	m_lock.unlock();
}

void Board::listen() {
	while (true) {
		string smsg;
		char rmsg[config::buf_size];
		bool success = false;

		if (!m_cmd_server.listen()) {
			continue;
		}
		
		const string& cmd = m_cmd_server.get_cmd();
		const vector<string>& args = m_cmd_server.get_args();

		smsg.clear();

		if (cmd == "module") {
			if (args.size() > 2) {
				const string& vname = args[0];
				const string& vtype = args[1];
				if (!create_module(vname, vtype)) {
					stringstream ss;
					ss << "Couldn't create a module : " << vname << " " << vtype;
					smsg = ss.str();
				}
				else {
					success = true;
				}
			}
			else {
				smsg = "Too few arguments for module command.";
				m_cmd_server.send_error(smsg);
			}
		}
		else if (cmd == "signal") {
			if (args.size() > 2) {
				const string& etype = args[0];
				const string& ename = args[1];

				if (!create_signal(args[0], args[1])) {
					stringstream ss;
					ss << "Couldn't create a signal : " << etype << " " << ename;
					smsg = ss.str();
				}
				else {
					success = true;
				}
			}
			else {
				smsg = "Too few arguments for signal command.";
			}
		}
		else if (cmd == "stop") {
			if (!args.size()) {
				stop_all();
				success = true;
			}
			else {
				for (int i = 0; i < args.size(); ++i) {

					if (!stop(args[i])) {
						smsg = "Couldn't find module " + args[i] + ".";
					}
				}
			}
		}
		else if (cmd == "close") {
			stop_all();
			success = true;
		}
		else if (cmd == "ls") {
			if (args.size() == 0) {
				smsg = "Too few arguments for ls.";
			}
			else if (args[0] == "module") {
				for each(pair<const string, Module*> module in m_modules) {
					smsg += module.first + "\n";
				}
				success = true;
			}
			else if (args[0] == "signal") {
				for each(pair<const string, Signal*> signal in m_signals) {
					smsg += signal.first + "\n";
				}
				success = true;
			}
			else {
				smsg = "Invalid arguent for ls. : " + args[0];
			}
		}
		else if (cmd == "start") {
			if (args.size() == 0) {
				start_all();
			}
			else {
				for (int i = 0; i < args.size(); ++i) {
					if (!start(args[i])) {
						break;
					}
				}
			}
		}
		else {
			smsg = "Invalid command. : " + args[0];
		}

		smsg += '\0';

		if (success)
			m_cmd_server.send_success(smsg);
		else
			m_cmd_server.send_error(smsg);
	}

}


void Board::set_port(int port) {
	m_cmd_server.set_server("128.0.0.1", 8080);
}

bool Board::set_port(const string& module, const string& port, void* value) {
	return true;
}

bool Board::create_module(const string& type, const string& name) {
	for (mcmap::iterator it = m_mcreators.begin(); it != m_mcreators.end(); ++it) {
		if (it->first == type) {
			Module* m = (this->*(it->second))();
			m_modules.insert(pair<const string, Module*>(name, m));
			return true;
		}
	}
	return false;
}

bool Board::create_signal(const string& type, const string& name) {
	for (scmap::iterator it = m_screators.begin(); it != m_screators.end(); ++it) {
		if (it->first == type) {
			Signal* s = (this->*(it->second))();
			m_signals.insert(pair<const string, Signal*>(name, s));
			return true;
		}
	}
	return false;
}

template <typename T>
Module* Board::create_module() {
	return dynamic_cast<Module*>(new T);
}

template <typename T>
Signal* Board::create_signal() {
	return dynamice_cast<Signal*>(new T);
}

template <typename T>
void Board::register_module(const string& type) {
	m_mcreators.insert(pair<const string, mcreator>(type, &Board::create_module<T>));
}

template <typename T>
void Board::register_signal(const string& type) {
	m_screators.insert(pair<const string, screator>(type, &create_signal<T>));
}
void foo(const char* s) {}

bool Board::connect(const string& out_mod_name , const string& out_port_name,
	const string& in_mod_name, const string& in_port_name) {
	OutPort* out_port = m_modules[out_mod_name]->get_out_port(out_port_name);
	InPort* in_port = m_modules[in_mod_name]->get_in_port(in_port_name);
	return out_port->connect(in_port);
}