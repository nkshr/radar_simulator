#include <thread>
#include <future>
#include <mutex>
#include <iostream>
#include <string>
#include <sstream>
#include  <map>

#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

#include "vertex/simulator.hpp"
#include "common/miscel.hpp"

#include "graph.hpp"

using namespace std;

Graph::Graph() {
	m_cmd_server.set_server("", 8080);
}

bool Graph::init() {
	if(!m_cmd_server.init())
		return false;

	register_vertex<Simulator>("simulator");

	return true;
}

void Graph::run() {
	start_all();
	listen();
	
	for (vmap::iterator it = m_vertexes.begin(); it != m_vertexes.end(); ++it) {
		it->second->join();
	}
}

bool Graph::start(const string& vname) {
	for (vmap::iterator it = m_vertexes.begin(); it != m_vertexes.end(); ++it) {
		if (it->first == vname) {
			it->second->start();
			return true;
		}
	}

	return false;
}
void Graph::start_all() {
	for (vmap::iterator it = m_vertexes.begin(); it != m_vertexes.end(); ++it) {
		it->second->start();
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

void Graph::lock() {
	m_lock.lock();
}

void Graph::unlock() {
	m_lock.unlock();
}

void Graph::listen() {
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

		if (cmd == "vertex") {
			if (args.size() > 2) {
				const string& vname = args[0];
				const string& vtype = args[1];
				if (!create_vertex(vname, vtype)) {
					stringstream ss;
					ss << "Couldn't create a vertex : " << vname << " " << vtype;
					smsg = ss.str();
				}
				else {
					success = true;
				}
			}
			else {
				smsg = "Too few arguments for vertex command.";
				m_cmd_server.send_error(smsg);
			}
		}
		else if (cmd == "edge") {
			if (args.size() > 2) {
				const string& etype = args[0];
				const string& ename = args[1];

				if (!create_edge(args[0], args[1])) {
					stringstream ss;
					ss << "Couldn't create a edge : " << etype << " " << ename;
					smsg = ss.str();
				}
				else {
					success = true;
				}
			}
			else {
				smsg = "Too few arguments for edge command.";
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
						smsg = "Couldn't find vertex " + args[i] + ".";
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
			else if (args[0] == "vertex") {
				for each(pair<const string, Vertex*> vertex in m_vertexes) {
					smsg += vertex.first + "\n";
				}
				success = true;
			}
			else if (args[0] == "edge") {
				for each(pair<const string, Edge*> edge in m_edges) {
					smsg += edge.first + "\n";
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

