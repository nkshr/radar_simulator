#include <thread>
//#include <mutex>
#include <iostream>
#include <string>
#include  <map>

#include <WinSock2.h>

#include "module/module.hpp"
#include "common/miscel.hpp"

#include "command.hpp"

#include "board.hpp"

#include "module/rsim_test.hpp"
#include "module/time_sync.hpp"
#include "module/glfw_window.hpp"
#include "module/image_reader.hpp";

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;


SubProcess::SubProcess(Board * board):m_board(board), m_recv_msg_size(sizeof(m_recv_msg)),
m_bfinish(false){
	memset(&m_myself, 0, sizeof(m_myself));
	m_myself.sin_family = AF_INET;
	m_myself.sin_addr.s_addr = inet_addr("127.0.0.1");
	m_myself.sin_port = htons(8080);

	register_module<TimeSyncServer>("time_sync_server");
	register_module<TimeSyncClient>("time_sync_client");
	register_module<RsimTest>("rsim_test");
	register_module<GLFWWindow>("window");
	register_module<ImageReader>("image_reader");
	register_module<SLAMViewer>("slam_viewer");

	register_memory<MemBool>("bool");
	register_memory<MemImages>("image");

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

bool SubProcess::init() {
	WSADATA wsa;
	if (WSAStartup(WINSOCK_VERSION, &wsa) != 0) {
		cerr << " Windows Socket initialization error : " << WSAGetLastError() << endl;
		return false;
	}


	m_myself_sock = socket(m_myself.sin_family, SOCK_STREAM, 0);
	if (m_myself_sock == INVALID_SOCKET) {
		cerr << "Socket creation error : " << WSAGetLastError() << endl;
		closesocket(m_myself_sock);
		WSACleanup();
		return false;
	}

	int res = ::bind(m_myself_sock, (sockaddr*)&m_myself, sizeof(m_myself));
	if (res == SOCKET_ERROR) {
		cerr << "bind failed with error : " << WSAGetLastError() << endl;
		return false;
	}

	return true;
}


bool SubProcess::set_data(const string& mname, const string& pname, const string& data) {
	ModMap::iterator mm_it = m_modules.find(mname);
	if (mm_it == m_modules.end()) {
		return false;
	}

	Module* module = mm_it->second;
	return module->set_data(pname, data);
}

bool SubProcess::get_data(const string& mname, const string& pname, string& data) {
	ModMap::iterator mm_it = m_modules.find(mname);
	if (mm_it == m_modules.end()) {
		return false;
	}

	Module* module = mm_it->second;
	return module->get_data(pname, data);
}


bool SubProcess::create_module(const string& type, const string& name) {
	for (ModCreatorMap::iterator it = m_mod_creators.begin(); it != m_mod_creators.end(); ++it) {
		if (it->first == type) {
			Module* m = (this->*(it->second))(name, m_board);
			m_modules.insert(pair<const string, Module*>(name, m));
			return true;
		}
	}
	return false;
}

template <typename T>
Module* SubProcess::create_module(const string& name, Board * board){
	return dynamic_cast<Module*>(new T(name, board));
}

bool SubProcess::create_memory(const string& type, const string& name) {
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
Memory* SubProcess::create_memory() {
	return dynamic_cast<Memory*>(new T);
}

template <typename T>
void SubProcess::register_module(const string& type) {
	m_mod_creators.insert(pair<const string, ModCreator>(type, &SubProcess::create_module<T>));
}

template <typename T>
void SubProcess::register_memory(const string& type) {
	m_mem_creators.insert(pair<const string, MemCreator>(type, &SubProcess::create_memory<T>));
}

void foo(const char* s) {}

template <typename T>
void SubProcess::register_cmd_proc(const string& name) {
	CmdProcess* cmd_proc = dynamic_cast<CmdProcess*>(new T(this));
	m_cmd_procs.insert(pair<string, CmdProcess*>(name, cmd_proc));
}

void SubProcess::process() {
	while (true) {
		lock_guard<mutex> lock(m_lock);

		if (m_bfinish)
			break;

		int	res = listen(m_myself_sock, SOMAXCONN);
		if (res == SOCKET_ERROR) {
			cerr << "listen failed with error : " << WSAGetLastError() << endl;
			break;
		}

		SOCKET target_sock = accept(m_myself_sock, NULL, NULL);
		if (target_sock == INVALID_SOCKET) {
			cerr << "accept failed with error : " << WSAGetLastError() << endl;
			break;
		}

		res = recv(target_sock, m_recv_msg, m_recv_msg_size, 0);
		if (res == SOCKET_ERROR) {
			cerr << "recv failed with error : " << WSAGetLastError() << endl;
			break;
		}

		cout << m_recv_msg << " was received." << endl;

		vector<string> toks;
		split(m_recv_msg, " \n", toks);


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
			msg = "Invalid command " + cmd + " was received.";
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

Board::Board() : m_cf(1.0), m_sub_proc(this) , m_bfinish_main(false){
	m_clock.set_clock_freq(m_cf);
}

bool Board::init() {
	if (!m_sub_proc.init())
		return false;

	return true;
}

void Board::finish() {
}

mutex& Board::get_lock_print() {
	return m_lock_print;
}


void Board::run() {
	thread th = thread(&SubProcess::process, &m_sub_proc);
	main_process();
	th.join();
}

void Board::finish_main_proc() {
	lock_guard<mutex> lock(m_lock);
	m_bfinish_main = true;
}

void Board::main_process() {
	unique_lock<mutex> _lock(m_lock);
	m_clock.start();
	_lock.unlock();

	while (true) {
		long long sleep_time;
		{
			lock_guard<mutex> lock(m_lock);
			if (m_bfinish_main)
				break;
			//////////////////////////////////////////
			/*Processes are written, here which must be in main thread.*/


			//////////////////////////////////////////
			m_clock.update();
			sleep_time = m_clock.get_sleep_time();

			lock_guard<mutex> lock_print(m_lock_print);
			cout << "Board clock : " << to_time_string(m_clock.get_time()) << endl;
		}

		sleep_for(nanoseconds(sleep_time));
	}
}
void Board::set_time(long long t) {
	lock_guard<mutex> lock(m_lock);
	m_clock.set_time(t);

}

long long Board::get_time() {
	lock_guard<mutex> lock(m_lock);
	return m_clock.get_time();
}

Clock * Board::get_clock() {
	lock_guard<mutex> lock(m_lock);
	return m_clock.clone();
}
