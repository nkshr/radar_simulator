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

	register_cmd_proc<CmdLsMod>("lsmod");
	register_cmd_proc<CmdLsPort>("lsport");
	register_cmd_proc<CmdFinish>("finish");
	register_cmd_proc<CmdPing>("ping");
	register_cmd_proc<CmdModule>("module");
	register_cmd_proc<CmdSet>("set");
	register_cmd_proc<CmdGet>("get");
	register_cmd_proc<CmdRun>("run");

	register_module<RsimTest>("rsim_test");
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


		string cmd_str;
		if (toks.size() != 0) {
			cmd_str = toks[0];
		}
		else {
			cmd_str = "";
		}

		CMD cmd;
		for (int i = 0; i < CMD_END; ++i) {
			if (cmd_str == cmd_strs[i]) {
				cmd = static_cast<Board::CMD>(i);
			}
		}

		vector<string> args;
		if (toks.size() > 1) {
			args.assign(toks.begin() + 1, toks.end());
		}

		string msg;
		bool cmd_suc = false;
		switch (cmd) {
		case Board::CMD::MODULE:
			cmd_suc = cmd_module(args, msg);
			break;
		case Board::CMD::SET:
			cmd_suc = cmd_set(args, msg);
			break;
		case Board::CMD::GET:
			cmd_suc = cmd_get(args, msg);
			break;
		case Board::CMD::LSMOD:
			cmd_suc = cmd_lsmod(args, msg);
			break;
		case Board::CMD::LSPORT:
			cmd_suc = cmd_lsport(args, msg);
			break;
		case Board::CMD::RUN:
			cmd_suc = cmd_run(args, msg);
			break;
		case Board::CMD::FINISH:
			cmd_suc = cmd_finish(args, msg);
			break;
		case Board::CMD::PING:
			break;
		default:
			cmd_suc = false;
			 msg = "Invalid command " +  cmd_str + " received.";
			break;
		}

		if (cmd_suc) {
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

ModMap& Board::get_modules(){
	return m_modules;
}

template <typename T>
void Board::register_cmd_proc(const string& name) {
	CmdProcess* cmd_proc = dynamic_cast<CmdProcess*>(new T(this));
	m_cmd_procs.insert(pair<string, CmdProcess*>(name, cmd_proc));
}

bool Board::cmd_module(vector<string>& args, string &msg) {
	if (args.size() < 2) {
		msg = "Too few arguments.\n";
		msg += "module <type> <name0> <name1>...";
		return false;
	}

	if (args[0] == "--help") {
		for (ModCreatorMap::iterator it = m_mcreators.begin(); it != m_mcreators.end(); ++it) {
			msg += it->first + '\n';
		}
		msg[msg.size() - 1] = '\0';
		return true;
	}

	string& type = args[0];
	string& name = args[1];
	if (!create_module(type, name)) {
		msg = "Couldn't create module " + type + " " + name + ".";
		return false;
	}

	for (int i = 0; i < args.size() - 1; ++i) {
		msg = type + " " + args[i + 1] + " created.";
		if (i != args.size() - 2) {
			msg += "\n";
		}
	}
	return true;
}

bool Board::cmd_set(vector<string>& args, string &msg) {
	if (args.size() != 3) {
		msg = "Too few argument\n";
		msg += "set <module> <port> <data>";
		return false;
	}

	string& module = args[0];
	string& port = args[1];
	string& data = args[2];
	if (!set_data(module, port, data)) {
		msg = "Couldn't set " + data + " to " + port + " in " + module + ".";
		return false;
	}
	msg = data + " -> " + port;
	return true;
}

bool Board::cmd_get(vector<string>& args, string &msg) {
	msg = "";

	if (args.size() != 2) {
		msg = "Too few argument\n";
		msg += "get <module> <port>";
		return false;
	}

	string& module = args[0];
	string& port = args[1];
	if (!get_data(module, port, msg)) {
		msg = "Couldn't find " + port + " in " + module + ".";
		return false;
	}

	msg += '\0';
	return true;
}

bool Board::cmd_lsmod(vector<string>& args, string& msg) {
	msg = "";
	vector<string> names = get_module_names();

	if (names.size() == 0) {
		return true;
	}

	msg = "";
	for (int i = 0; i < names.size(); ++i) {
		msg += names[i] + '\n';
	}
	msg[msg.size() - 1] = '\0';
	return true;
}

bool Board::cmd_lsport(vector<string>& args, string&msg) {
	msg = "";
	if (args.size() != 1) {
		msg = "Too few arguments.\n";
		msg += "rsim lsport <module>";
		return false;
	}

	string& mname = args[0];
	ModMap::const_iterator mm_it = m_modules.find(mname);
	if (mm_it == m_modules.end()) {
		msg = "Couldn't find " + mname + ".";
		return false;
	}

	vector<pair<string, string>> names_and_discs;
	mm_it->second->get_port_names_and_discs(names_and_discs);

	for (int i = 0; i < names_and_discs.size(); ++i) {
		string& name =  names_and_discs[i].first;
		string& discs = names_and_discs[i].second;
		msg += name + " : " + discs + "\n";
	}
	if(msg.size())
		msg[msg.size() - 1] = '\0';
	return true;
}

bool Board::cmd_run(vector<string>& args, string& msg) {
	if (!args.size()) {
		msg = "";

		for (ModMap::iterator it = m_modules.begin(); it != m_modules.end(); ++it) {
			const string &name = it->first;
			Module *module = it->second;
			module->run();
			msg += name + " is  runninng.";
			if (it != --m_modules.end()) {
				msg += "\n";
			}
		}
		return true;
	}
	return false;

}

bool Board::cmd_finish(vector<string>& args, string& msg) {
	if (!args.size()) {
		msg = "";

		for (ModMap::iterator it = m_modules.begin(); it != m_modules.end(); ++it) {
			const string &name = it->first;
			Module *module = it->second;
			module->lock();
			module->stop();
			module->unlock();
		}

		for (ModMap::iterator it = m_modules.begin(); it != m_modules.end(); ++it) {
			const string &name = it->first;
			Module *module = it->second;
			module->join();
		}

		for (ModMap::iterator it = m_modules.begin(); it != m_modules.end(); ++it) {
			const string &name = it->first;
			Module *module = it->second;
			if (!module->finish()) {
				msg += name + " finished abnormally.";
			}
			else {
				msg += name + " fineshed.";
			}

			if (it != --m_modules.end()) {
				msg += "\n";
			}

		}

		return true;
	}
	return false;

}