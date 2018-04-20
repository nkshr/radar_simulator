#include <thread>
#include <future>
#include <mutex>

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
		mtx.lock();

		mtx.unlock();
		if (!m_brun)
			break;

		if (!process())
			break;
		

		m_clock.adjust();
	}
}



void Graph::run() {

	for (int i = 0; i < m_num_vertexes; ++i) {
		m_vertexes[i].run();
	}

	listen();
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
	m_buf[m_idx] = d;
	m_times[m_idx] = t;

	if (++m_idx == m_buf_size) {
		m_idx = 0;
	}
}
