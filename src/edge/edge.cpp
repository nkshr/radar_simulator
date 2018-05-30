#include "edge.hpp"

RadarSignal::RadarSignal(int buf_size) : m_idx(0), m_buf_size(buf_size) {
	m_buf = new double[m_buf_size];
	m_times = new long long[m_buf_size];
}

RadarSignal::~RadarSignal() {
	delete[] m_buf;
	delete[] m_times;
}

void RadarSignal::set_signal(double d, long long t) {
	m_graph->lock();

	m_buf[m_idx] = d;
	m_times[m_idx] = t;

	if (++m_idx == m_buf_size) {
		m_idx = 0;
	}

	m_graph->unlock();
}

