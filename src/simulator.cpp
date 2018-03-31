#include <complex>

#include "simulator.h"

Simulator::Simulator(const SConfig &sconfig) {
	m_sconfig = sconfig;
}

void Simulator::init(){

}

//void Simulator::simulate(const Radar &radar, std::vector<Object> &objects) {
//	const double chirp_rate = radar.bandwidth / radar.pulse_width;
//	m_rx.resize(m_num_samples);
//	m_tx.resize(m_num_samples);
//
//	for (int i = 0; i < m_num_samples; ++i) {
//		m_rx[i] = m_tx[i] = Complex(0, 0);
//	}
//
//	int num_pings = 0;
//	double prev_ping_time = 0;
//	const double half_time = (m_num_samples /2) * m_sample_time;
//	const double half_pulse_width = radar.pulse_width * 0.5;
//	bool ping = false;
//	for (int i = 0; i < m_num_samples; ++i) {
//		const double cur_time = i * m_sample_time;// -half_time;
//
//		const double diff = cur_time - prev_ping_time;
//		if (diff < m_sample_time + DBL_EPSILON && !(diff <0)) {
//			ping = true;
//			prev_ping_time = cur_time;
//		}
//
//		if (cur_time - prev_ping_time > radar.pulse_width) {
//			ping = false;
//		}
//
//		//generate chirp
//		if (ping) {
//			const double theta = config::pi * chirp_rate * cur_time * cur_time;
//			m_tx[i] = radar.gain * exp(Complex(0, theta));
//		}
//		else {
//			m_tx[i] = Complex(0, 0);
//		}
//
//		for (int j = 0; j < objects.size(); ++j) {
//			int idiff = (int)((objects[j].x - radar.x) * 0.5 / (config::c * m_sample_time));
//			int rindex = i + idiff;
//
//			if (rindex >= m_num_samples) {
//				rindex -= m_num_samples;
//				//m_rx[rindex] += m_tx[i]*objects[i].reflectance;
//			}
//			else {
//				m_rx[rindex] += m_tx[i] * objects[j].reflectance;
//			}
//		}
//	}
//
//	for (int i = 0; i < m_num_samples; ++i) {
//		m_rx[i] = Complex(m_tx[i].real() * m_rx[i].real(), 0);
//	}
//}

void Simulator::simulate(const Radar &radar, std::vector<Object> & objsects) {
	const double chirp_rate = radar.bandwidth / radar.pulse_width;

}

void Simulator::simulate(const Vec2d &start, const Vec3d &dir, const int depth) {
	if (depth > m_sconfig.max_depth)
		return;
	
	Vec2d next_start;
	Vec3d next_dir;

	simulate(next_start, next_dir, depth+1);
}

void Simulator::update() {

}

CArray Simulator::get_rx() {
	return m_rx;
}

CArray Simulator::get_tx() {
	return m_tx;
}

