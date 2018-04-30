#include <thread>
#include <future>
#include <mutex>
#include <iostream>
#include <string>
#include <sstream>

#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

#include "graph.h"

mutex mtx;

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



void Graph::run() {

	for (int i = 0; i < m_vertexes.size(); ++i) {
		m_vertexes[i]->run();
	}

	listen();
}

void Graph::add_vertex(Vertex* v) {
	m_vertexes.push_back(v);
}

void Graph::listen() {

}

void Graph::set_port(int port) {

}

bool Graph::create_vertex(const char* vtype, const char* vname) {
	for (int i = 0; i < m_vertex_types.size(); ++i) {
		if (strcmp(m_vertex_types[i], vtype) == 0) {
			
			Vertex * v = dynamic_cast<Vertex*>(new CmdReceiver());
			m_vertexes.push_back(v);
		}
	}
}

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
		const char* vtype = args[0];
		const char* vname = args[1];

		if (!m_graph->create_vertex(vtype, vname)) {
			sprintf(m_err_msg, "Couldn't create a vertex : %s %s\n", vtype, vname);
			m_udp.send(m_err_msg, config::buf_size);
			return true;
		}

		break;
	case Cmd::EDGE:
		const char* etype = args[0];
		const char* ename = args[1];

		if (!m_graph->create_edge(etype, ename)) {
			sprintf(m_err_msg, "Couldn't create a edge : %s %s\n", vtype, vname);
			m_udp.send(m_err_msg, config::buf_size);
			return true;
		}
		break;
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

