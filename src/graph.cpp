#include <thread>
#include <future>
#include <mutex>
#include <iostream>
#include <string>
#include <sstream>
#include  <map>

#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

#include "graph.h"
#include "simulator.h"
#include "miscel.h"

using namespace std;

mutex mtx;

Vertex::Vertex() : m_brun(false){
}

void Vertex::run() {
	m_th = thread(&Vertex::run, this);
}

void Vertex::join() {
	m_th.join();
}

void Vertex::processing_loop() {
	m_clock.start();
	
	while (true) {
		if (!m_brun)
			break;

		if (!process())
			break;
		

		m_clock.adjust();
	}
}

void Vertex::stop() {
	m_brun = false;
}

void Graph::init() {
	UDP::init_win_sock();
	m_udp.init();

	register_vertex<CmdTerminal>("cmd_termial");
	register_vertex<Simulator>("simulator");
}

void Graph::run() {
	run_all();
	listen();

	for (vmap::iterator it = m_vertexes.begin(); it != m_vertexes.end(); ++it) {
		it->second->join();
	}
}

void Graph::run(const string& vname) {
	for (vmap::iterator it = m_vertexes.begin(); it != m_vertexes.end(); ++it) {
		if (it->first == vname) {
			it->second->run();
			return;
		}
	}
}
void Graph::run_all() {
	for (vmap::iterator it = m_vertexes.begin(); it != m_vertexes.end(); ++it) {
		it->second->run();
	}
}

void Graph::stop_all() {
	for (vmap::iterator it = m_vertexes.begin(); it != m_vertexes.end(); ++it) {
		it->second->stop();
	}
}

bool Graph::stop(const string& vname) {
	for (vmap::iterator it = m_vertexes.begin(); it != m_vertexes.end(); ++it) {
		if (it->first == vname) {
			it->second->stop();
			return true;
		}
	}
	return false;
}

void Graph::listen() {
	char err_msg[config::buf_size];

	while (true) {
		m_udp.receive(m_cp.get_buf(), m_cp.get_buf_size());

		if (!m_cp.parse()) {
			char emsg[config::buf_size];
			sprintf(emsg, "Invaliid command : %s", m_cp.get_buf());
			m_udp.send(emsg, config::buf_size);
		}

		m_udp.send("success", 8);

		const vector<char*> args = m_cp.get_args();

		switch (m_cp.get_cmd()) {
		case Cmd::VERTEX:
			if (args.size() > 2) {
				const char* vname = args[0];
				const char* vtype = args[1];
				if (!create_vertex(args[0], args[1])) {
					sprintf(err_msg, "Couldn't create a vertex : %s %s\n", vname, vtype);
					m_udp.send(err_msg, config::buf_size);
				}
			}
			else {
				cerr << "Too few arguments for vertex command." << endl;
			}
			break;
		case Cmd::EDGE:
			if (args.size() > 2) {
				const char* etype = args[0];
				const char* ename = args[1];

				if (!create_edge(args[0], args[1])) {
					sprintf(err_msg, "Couldn't create a edge : %s %s\n", args[0], args[1]);
					m_udp.send(err_msg, config::buf_size);
				}
			}
			else {
				cerr << "Too few arguments for edge command." << endl;
			}
			break;
		case Cmd::STOP:
			if (!args.size()) {
				stop_all();
			}
			else {
				for (int i = 0; i < args.size(); ++i) {
					if (!stop(args[i])) {
						cerr << "Couldn't find vertex " << args[i] << "." << endl;
					}
				}
			}
			break;
		case Cmd::CLOSE:
			stop_all();
			return;
		case Cmd::LS:
			if (!args.size() >= 1) {
				cerr << "Too few arguments for ls command." << endl;
			}
			else {
				for (int i = 0; i < args.size(); ++i) {
					const char* arg = args[i];
					string msg;
					if (strcmp(arg, "vertex") == 0) {
						for (vcmap::iterator it = m_vcreators.begin(); it != m_vcreators.end(); ++it) {
							msg += it->first+"\n";
						}
						m_udp.send(msg.c_str(), msg.size());
					}
					else if (strcmp(arg, "edge")==0) {
						for (ecmap::iterator it = m_ecreators.begin(); it != m_ecreators.end(); ++it) {
							cout << it->first << endl;
						}
					}
					else {
						cerr << arg << "  is invalid argument for ls command." << endl;
					}
				}
			}
			break;
		case Cmd::RUN:
			if (args.size() == 0) {
				run_all();
			}
			else {
				for (int i = 0; i < args.size(); ++i) {
					run(args[i]);
				}
			}
		default:
			break;
		}
	}
}

void Graph::set_port(int port) {
	m_udp.set_port(port);
}

bool Graph::create_vertex(const string& vtype, const string& vname) {
	for (vcmap::iterator it = m_vcreators.begin(); it != m_vcreators.end(); ++it) {
		if (it->first == vtype) {
			//it->second(vname);
			return true;
		}
	}
	return false;
}

template <typename T>
void Graph::create_vertex(const string& vname) {
	m_vertexes.insert(pair<const string, Vertex*>(vname, dynamic_cast<Vertex*>(new T())));
}

bool Graph::create_edge(const string& etype, const string& ename) {
	//for (int i = 0; i < m_edge_types.size(); ++i) {
	//	if (strcmp(m_edge_types[i], etype) == 0) {
	//		m_ecreators[i](ename);
	//		return true;
	//	}
	//}
	return false;
}

template <typename T>
void Graph::create_edge(const string& ename) {
	//m_edges.push_back(dynamic_cast<T>(new T(name)));
}
void foo(const char* s) {}

//template <typename T>
//void Graph::register_vertex(const char* vtype) {
//	m_vcreators.insert(vtype, foo);
//}



//void Graph::run(const vector<char*>& vertexes) {
//	for (int i = 0; i < m_vertexes.size(); ++i) {
//		
//	}
//}
//
//void Graph::stop_all() {
//	for (int i = 0; i < m_vertexes.size(); ++i){
//		m_vertexes[i]->stop();
//	}
//}

bool CmdReceiver::process() {
	const long long tpc = static_cast<int>(round(m_clock.get_time_per_clock() * 0.001));

	m_udp.set_timeout(0, tpc);
	m_udp.receive(m_cp.get_buf(), m_cp.get_buf_size());
	
	if (!m_cp.parse()) {
		char emsg[config::buf_size];
		sprintf(emsg, "Invaliid command : %s", m_cp.get_buf());
		m_udp.send(emsg, config::buf_size);
	}

	m_udp.send("success", 8);

	const vector<char*> args = m_cp.get_args();

	switch (m_cp.get_cmd()) {
	case Cmd::VERTEX:
		if (args.size() > 2) {
			const char* vname = args[0];
			const char* vtype = args[1];
			if (!m_graph->create_vertex(args[0], args[1])) {
				sprintf(m_err_msg, "Couldn't create a vertex : %s %s\n", vname, vtype);
				m_udp.send(m_err_msg, config::buf_size);
				return true;
			}
		}
		else {
			cerr << "Too few arguments for vertex command." << endl;
		}

		break;
	case Cmd::EDGE:
		if (args.size() > 2) {
			const char* etype = args[0];
			const char* ename = args[1];

			if (!m_graph->create_edge(args[0], args[1])) {
				sprintf(m_err_msg, "Couldn't create a edge : %s %s\n", args[0], args[1]);
				m_udp.send(m_err_msg, config::buf_size);
				return true;
			}
		}
		else {
			cerr << "Too few arguments for edge command." << endl;
		}

		break;
	case Cmd::STOP:
		if (!args.size()) {
			m_graph->stop_all();
		}
	default:
		break;
	}

	
	return true;
}

void CmdReceiver::set_port(int port) {
	m_udp.set_port(port);
}

RadarSignal::RadarSignal(int buf_size) : m_idx(0), m_buf_size(buf_size){
	m_buf = new double[m_buf_size];
	m_times = new long long[m_buf_size];
}

RadarSignal::~RadarSignal() {
	delete[] m_buf;
	delete[] m_times;
}

CmdTerminal::CmdTerminal() {
}

bool CmdTerminal::process() {
	cin.getline(m_buf, config::buf_size);
	
	m_udp.send(m_buf, config::buf_size);

	m_udp.receive(m_rep, config::buf_size);

	if (!strcmp(m_rep, "success")) {
		cerr << m_rep << endl;
	}
	
	return true;
}

void RadarSignal::set_signal(double d, long long t) {
	mtx.lock();
	m_buf[m_idx] = d;
	m_times[m_idx] = t;

	if (++m_idx == m_buf_size) {
		m_idx = 0;
	}
	mtx.unlock();
}

