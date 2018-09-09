#include <thread>
//#include <mutex>
#include <iostream>
#include <string>
#include  <map>

#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

#include "module/rsim_test.hpp"
#include "module/module.hpp"
#include "common/miscel.hpp"

#include "command.hpp"

#include "board.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::map;
using std::string;
using std::pair;


Board::Board() :m_brun(true), m_bdebug(true) {
	memset(&m_myself, 0, sizeof(m_myself));
	m_myself.sin_family = AF_INET;
	m_myself.sin_addr.s_addr = inet_addr("127.0.0.1");
	m_myself.sin_port = htons(8080);

	register_module<RsimTest>("rsim_test");

	register_memory<MemBool>("bool");

	//relate command to proccess.
	register_cmd_proc<CmdLsMod>("lsmod");
	register_cmd_proc<CmdLsPort>("lsport");
	register_cmd_proc<CmdFinish>("finish");
	register_cmd_proc<CmdPing>("ping");
	register_cmd_proc<CmdModule>("module");
	register_cmd_proc<CmdSet>("set");
	register_cmd_proc<CmdGet>("get");
	register_cmd_proc<CmdRun>("run");
	register_cmd_proc<CmdMemory>("memory");
	register_cmd_proc<CmdShutdown>("shutdown");
	register_cmd_proc<CmdLsMem>("lsmem");	
	register_cmd_proc<CmdConnect>("connect");
	register_cmd_proc<CmdStop>("stop");
}

bool Board::init() {
	//if(!m_cmd_server.init())
	//	return false;

	//register_module<Simulator>("simulator");

	return true;
}

bool Board::init_all() {

	return true;
}

void Board::run() {
	WSADATA wsa;
	if (WSAStartup(WINSOCK_VERSION, &wsa) != 0) {
		cerr << " Windows Socket initialization error : " << WSAGetLastError() << endl;
		return;
	}

	char recv_msg[1024];
	int recv_msg_size = sizeof(recv_msg);

	m_myself_sock = socket(m_myself.sin_family, SOCK_STREAM, 0);
	if (m_myself_sock == INVALID_SOCKET) {
		cerr << "Socket creation error : " << WSAGetLastError() << endl;
		closesocket(m_myself_sock);
		WSACleanup();
		return;
	}

	int res = bind(m_myself_sock, (sockaddr*)&m_myself, sizeof(m_myself));
	if (res == SOCKET_ERROR) {
		cerr << "bind failed with error : " << WSAGetLastError() << endl;
		return;
	}

	while (m_brun) {
		res = listen(m_myself_sock, SOMAXCONN);
		if (res == SOCKET_ERROR) {
			cerr << "listen failed with error : " << WSAGetLastError() << endl;
			break;
		}

		SOCKET target_sock = accept(m_myself_sock, NULL, NULL);
		if (target_sock == INVALID_SOCKET) {
			cerr << "accept failed with error : " << WSAGetLastError() << endl;
			break;
		}

		res = recv(target_sock, recv_msg, recv_msg_size, 0);
		if (res == SOCKET_ERROR) {
			cerr << "recv failed with error : " << WSAGetLastError() << endl;
			break;
		}

		cout << recv_msg << " received." << endl;

		vector<string> toks;
		split(recv_msg, " \n", toks);


		string cmd;
		if (toks.size() != 0) {
			cmd = toks[0];
		}
		else {
			cmd = "";
		}

		vector<string> args;
		if (toks.size() > 1) {
			args.assign(toks.begin() + 1, toks.end());
		}

		string msg;

		CmdProcMap::iterator it = m_cmd_procs.find(cmd);
		bool cmd_result;
		if (it == m_cmd_procs.end()) {
			msg = "Invalid command " + cmd + " received.";
			cmd_result = false;
		}
		else {
			cmd_result = it->second->process(args);
			msg = it->second->msg;
		}

		if (cmd_result) {
			res = send(target_sock, cmd_suc_str.c_str(), cmd_suc_str.size(), 0);
		}
		else {
			res = send(target_sock, cmd_err_str.c_str(), cmd_err_str.size(), 0);
		}

		if (res != cmd_suc_str.size()) {
			cerr << "send failed with error : " << WSAGetLastError() << endl;
		}

		res = send(target_sock, msg.c_str(), msg.size(), 0);
		if (res != msg.size()) {
			cerr << "send failed with error : " << WSAGetLastError() << endl;
		}

		closesocket(target_sock);
	}

	closesocket(m_myself_sock);
	WSACleanup();
}

bool Board::run_module(const string& name) {
	for (ModMap::iterator it = m_modules.begin(); it != m_modules.end(); ++it) {
		if (it->first == name) {
			it->second->run();
			return true;
		}
	}

	return false;
}
void Board::run_all_modules() {
	for (ModMap::iterator it = m_modules.begin(); it != m_modules.end(); ++it) {
		it->second->run();
	}
}

void Board::stop_all_modules() {
	for (ModMap::iterator it = m_modules.begin(); it != m_modules.end(); ++it) {
		it->second->stop();
	}
}

bool Board::stop_module(const string& name) {
	for (ModMap::iterator it = m_modules.begin(); it != m_modules.end(); ++it) {
		if (it->first == name) {
			it->second->stop();
			return true;
		}
	}
	return false;
}

void Board::finish() {
	stop_all_modules();
	m_brun = false;
}

void Board::lock() {
	m_lock.lock();
}

void Board::unlock() {
	m_lock.unlock();
}

bool Board::set_data(const string& mname, const string& pname, const string& data) {
	ModMap::iterator mm_it = m_modules.find(mname);
	if (mm_it == m_modules.end()) {
		return false;
	}

	Module* module = mm_it->second;
	return module->set_data(pname, data);
}

bool Board::get_data(const string& mname, const string& pname, string& data) {
	ModMap::iterator mm_it = m_modules.find(mname);
	if (mm_it == m_modules.end()) {
		return false;
	}

	Module* module = mm_it->second;
	return module->get_data(pname, data);
}

mutex* Board::get_lock() {
	return &m_lock;
}

bool Board::create_module(const string& type, const string& name) {
	for (ModCreatorMap::iterator it = m_mod_creators.begin(); it != m_mod_creators.end(); ++it) {
		if (it->first == type) {
			Module* m = (this->*(it->second))();
			m_modules.insert(pair<const string, Module*>(name, m));
			return true;
		}
	}
	return false;
}

template <typename T>
Module* Board::create_module() {
	return dynamic_cast<Module*>(new T);
}

bool Board::create_memory(const string& type, const string& name) {
	for (MemCreatorMap::iterator it = m_mem_creators.begin(); it != m_mem_creators.end(); ++it) {
		if (it->first == type) {
			Memory* m = (this->*(it->second))();
			m_memories.insert(pair<const string, Memory*>(name, m));
			return true;
		}
	}
	return false;
}

template <typename T>
Memory* Board::create_memory() {
	return dynamic_cast<Memory*>(new T);
}

template <typename T>
void Board::register_module(const string& type) {
	m_mod_creators.insert(pair<const string, ModCreator>(type, &Board::create_module<T>));
}

template <typename T>
void Board::register_memory(const string& type) {
	m_mem_creators.insert(pair<const string, MemCreator>(type, &Board::create_memory<T>));
}

void foo(const char* s) {}

bool Board::connect(const string& out_mod_name , const string& out_port_name,
	const string& in_mod_name, const string& in_port_name) {
	return true;
}

template <typename T>
void Board::register_cmd_proc(const string& name) {
	CmdProcess* cmd_proc = dynamic_cast<CmdProcess*>(new T(this));
	m_cmd_procs.insert(pair<string, CmdProcess*>(name, cmd_proc));
}
