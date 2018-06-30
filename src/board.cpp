#include <thread>
//#include <mutex>
#include <iostream>
#include <string>
#include  <map>

#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

//#include "module/simulator.hpp"
#include "module/module.hpp"
#include "common/miscel.hpp"

#include "board.hpp"

#include "module/data_server.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::map;
using std::string;
using std::pair;

CmdProcess::CmdProcess(Board* board) : m_board(board) {

}

const string& CmdProcess::get_msg() const {
	return m_msg;
}

const string& CmdProcess::get_name() const {
	return m_name;
}

CmdModule::CmdModule(Board* board) : CmdProcess(board) {
}
bool CmdModule::process(vector<string> args) {
	if (args.size() < 1) {
		m_msg = "Too few arguments.\n";
		m_msg += "module <type> <name0> <name1>...";
		return false;
	}

	if (args[0] == "--help") {
		vector<string> types = m_board->get_module_types();

		m_msg = "";
		for (int i = 0; i < types.size(); ++i) {
			m_msg += types[i] + '\n';
		}
		m_msg[m_msg.size()-1] = '\0';
		return true;
	}

	string& type = args[0];
	string& name = args[1];
	if (!m_board->create_module(type, name)) {
		m_msg = "Couldn't create module " + type + " " + name + ".";
		return false;
	}

	return true;
}

bool CmdSet::process(vector<string> args) {
	if (args.size() != 3) {
		m_msg = "Too few argument\n";
		m_msg += "set <module> <port> <data>";
		return false;
	}

	string& module = args[0];
	string& port = args[1];
	string& data = args[2];
	if (!m_board->set_data(module, port, data)) {
		m_msg = "Couldn't set " +  data + " to " + port + " of " + module + ".";
		return false;
	}
	return true;
}

bool CmdGet::process(vector<string> args) {
	if (args.size() != 2) {
		m_msg = "Too few argument\n";
		m_msg += "get <module> <port>";
		return false;
	}

	string& module = args[0];
	string& port = args[1];
	if (m_board->get_data(module, port, m_msg)) {
		m_msg = "Couldn't find " + port + " of " + module + ".";
		return false;
	}
	return true;
}

bool CmdLsMod::process(vector<string> args) {
	m_msg = "";
	vector<string> names = m_board->get_module_names();

	if (names.size() == 0) {
		return true;
	}

	m_msg = "";
	for (int i = 0; i < names.size(); ++i) {
		m_msg += names[i] + '\n';
	}
	m_msg[m_msg.size() - 1] = '\0';
	return true;
}

bool CmdLsPort::process(vector<string> args) {
	m_msg = "";
	if (args.size() != 1) {
		m_msg = "Too few arguments.\n";
		m_msg += "rsim lsport <module>";
		return false;
	}

	string& mname = args[0];
	vector<string> names;
	if (!m_board->get_port_names(mname, names)) {
		m_msg = "Couldn't find " + mname + ".";
		return false;
	}

	for (int i = 0; i < names.size(); ++i) {
		m_msg += names[i] + "\n";
	}
	m_msg[m_msg.size() - 1] = '\0';
	return true;
}

bool CmdFinish::process(vector<string> args) {
	m_board->stop();
	return true;
}

bool CmdPing::process(vector<string> args) {
	m_msg = "rsim running";
	return true;
}

Board::Board() {
	memset(&m_myself, 0, sizeof(m_myself));
	m_myself.sin_family = AF_INET;
	m_myself.sin_addr.s_addr = inet_addr("127.0.0.1");
	m_myself.sin_port = htons(8080);

	register_cmd_proc<CmdLsMod>("lsmod");
	register_cmd_proc<CmdLsPort>("lsport");
	register_cmd_proc<CmdFinish>("finish");
	register_cmd_proc<CmdPing>("ping");
	register_cmd_proc<CmdModule>("module");
	register_cmd_proc<CmdSet>("set");
	register_cmd_proc<CmdGet>("get");

	register_module<DataServer>("data_server");
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

		string& cmd = toks[0];

		vector<string> args;
		if (toks.size() > 1) {
			args.assign(toks.begin() + 1, toks.end());
		}

		CmdProcMap::iterator cpm_it = m_cmd_procs.find(cmd);
		if (cpm_it == m_cmd_procs.end()) {
			res = send(target_sock, cmd_err_str.c_str(), cmd_err_str.size(), 0);
			if (res != cmd_err_str.size()) {
				cerr << "send failed with error : " << WSAGetLastError() << endl;
				break;
			}

			string msg = "Invalid command " +  cmd + " received.";
			res = send(target_sock, msg.c_str(), msg.size(), 0);
			if (res != msg.size()) {
				cerr << "send failed with error : " << WSAGetLastError() << endl;
				break;
			}

		}
		else {
			CmdProcess* cmd_process = cpm_it->second;
			if (cmd_process->process(args)) {
				res = send(target_sock, cmd_suc_str.c_str(), cmd_suc_str.size(), 0);
			}
			else {
				res = send(target_sock, cmd_err_str.c_str(), cmd_err_str.size(), 0);
			}

			if (res != cmd_suc_str.size()) {
				cerr << "send failed with error : " << WSAGetLastError() << endl;
				break;
			}

			const string& msg = cmd_process->get_msg();
			res = res = send(target_sock, msg.c_str(), msg.size(), 0);
			if (res != msg.size()) {
				cerr << "send failed with error : " << WSAGetLastError() << endl;
			}
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

void Board::stop() {
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

bool Board::create_module(const string& type, const string& name) {
	for (ModCreatorMap::iterator it = m_mcreators.begin(); it != m_mcreators.end(); ++it) {
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


template <typename T>
void Board::register_module(const string& type) {
	m_mcreators.insert(pair<const string, ModCreator>(type, &Board::create_module<T>));
}

void foo(const char* s) {}

bool Board::connect(const string& out_mod_name , const string& out_port_name,
	const string& in_mod_name, const string& in_port_name) {
	return true;
}

vector<string> Board::get_module_names() const {
	vector<string> names;
	names.reserve(m_modules.size());

	for each(pair<string, Module*> module in m_modules) {
		names.push_back(module.first);
	}

	return names;	
}

vector<string> Board::get_module_types() const {
	vector<string> types;
	types.reserve(m_mcreators.size());

	for each(pair<string, ModCreator> mcreator in m_mcreators) {
		types.push_back(mcreator.first);
	}
	return types;
}

bool Board::get_port_names(const string& mname, vector<string>& names) const {
	ModMap::const_iterator mm_it = m_modules.find(mname);
	if (mm_it == m_modules.end()) {
		return false;
	}
	mm_it->second->get_port_names(names);
	return true;
}

template <typename T>
void Board::register_cmd_proc(const string& name) {
	CmdProcess* cmd_proc = dynamic_cast<CmdProcess*>(new T(this));
	m_cmd_procs.insert(pair<string, CmdProcess*>(name, cmd_proc));
}