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

Graph::Graph() {
	m_udp.set_myself("127.0.0.1", 5000);
}

void Graph::init() {
	UDP::init_win_sock();
	m_udp.init();

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
	string smsg;

	char rmsg[config::buf_size];

	while (true) {
		m_udp.receive(rmsg, config::buf_size);

		m_cp.parse(rmsg);
		
		if (m_cp.get_cmd() == Cmd::INVALID)
			m_udp.send_back(cmd_error_str.c_str(), cmd_error_str.size());

		const vector<string> args = m_cp.get_args();

		smsg.clear();

		switch (m_cp.get_cmd()) {
		case Cmd::VERTEX:
			if (args.size() > 2) {
				const string& vname = args[0];
				const string& vtype = args[1];
				if (!create_vertex(vname, vtype)) {
					m_udp.send_back(cmd_error_str.c_str(), cmd_error_str.size());

					stringstream ss;
					ss << "Couldn't create a vertex : " << vname << " " << vtype;
					smsg = ss.str();
					m_udp.send_back(smsg.c_str(), smsg.size());
				}
				else {
					m_udp.send_back(cmd_success_str.c_str(), cmd_success_str.size());
				}
			}
			else {
				m_udp.send_back(cmd_error_str.c_str(), cmd_error_str.size());
				smsg =  "Too few arguments for vertex command.";
				m_udp.send_back(smsg.c_str(), smsg.size());
			}
			break;
		case Cmd::EDGE:
			if (args.size() > 2) {
				const string& etype = args[0];
				const string& ename = args[1];

				if (!create_edge(args[0], args[1])) {
					m_udp.send_back(cmd_error_str.c_str(), cmd_error_str.size());

					stringstream ss;
					ss << "Couldn't create a edge : " << etype << " " << ename;
					smsg = ss.str();
					m_udp.send_back(smsg.c_str(), smsg.size());
				}
				else {
					m_udp.send_back(cmd_success_str.c_str(), cmd_success_str.size());
				}
			}
			else {
				m_udp.send_back(cmd_error_str.c_str(), cmd_error_str.size());
				smsg = "Too few arguments for edge command.";
				m_udp.send_back(smsg.c_str(), smsg.size());
			}
			break;
		case Cmd::STOP:
			if (!args.size()) {
				m_udp.send_back(cmd_success_str.c_str(), cmd_success_str.size());
				stop_all();
			}
			else {
				for (int i = 0; i < args.size(); ++i) {
					if (!stop(args[i])) {
						m_udp.send_back(cmd_success_str.c_str(), cmd_success_str.size());
						cerr << "Couldn't find vertex " << args[i] << "." << endl;
					}
				}
			}
			break;
		case Cmd::CLOSE:
			stop_all();
			return;
		case Cmd::LS:
			if (args.size() == 0) {
				cerr << "Too few arguments for ls" << endl;
				break;
			}
			else if (args[0] == "vertex") {
				for (vcmap::iterator it = m_vcreators.begin(); it != m_vcreators.end(); ++it) {
					smsg += it->first + "\n";
				}
			}
			else if(args[0] == "edge"){
				for (ecmap::iterator it = m_ecreators.begin(); it != m_ecreators.end(); ++it) {
					smsg += it->first + "\n";
				}
			}
			else {
				smsg = "Invalid arguent for ls. : " + args[0];
			}
			m_udp.send(smsg.c_str(), smsg.size());
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

	m_udp.send("success", 8);

}

void Graph::set_port(int port) {
	//m_udp.set_port(port);
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


RadarSignal::RadarSignal(int buf_size) : m_idx(0), m_buf_size(buf_size){
	m_buf = new double[m_buf_size];
	m_times = new long long[m_buf_size];
}

RadarSignal::~RadarSignal() {
	delete[] m_buf;
	delete[] m_times;
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

