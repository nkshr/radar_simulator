#include <thread>
#include <future>
#include <mutex>

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
