#include <complex>
#include <iterator>
#include <chrono>
#include <thread>
#include <ctime>

#include "simulator.h"
#include "clock.h"

using namespace std;
using namespace std::chrono;

using namespace std::this_thread;

Simulator::Simulator(const SConfig &sconfig): m_stop(false) {
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
	const double chirp_rate = radar.bandwidth / radar.pulse_width; //chirp rate
	const long long sampling_interval = radar.sampling_rate; //sampling interval
	const long long pulse_interval = static_cast<long long>(round(1.0e9 / radar.prf));
		
	Clock clock(radar.sampling_rate);
	clock.start();

	long long last_pulse_time = 0;

	while (true) {		
		if (m_stop)
			break;

		const long long cur_time = clock.get_cur_time().count();
		if (pulse_interval > cur_time - last_pulse_time) {
			last_pulse_time = cur_time;
		}

		//const double s = simulate(radar.pos, radar.dir, 0, cur_time);
		double s;
		set_signal(cur_time, s);

		sleep_for(milliseconds(100));

		clock.adjust();
	}

	clock.stop();
}

double Simulator::simulate(const Vec3d &start, const Vec3d &dir,
	const int depth, const long long stime, const long long etime, const long long t0) {
	if (depth > m_sconfig.max_depth)
		return 0.0;
	
	double t;
	double reflectance;


	Intersection isct = check_intersection(start, dir);
	if (!isct.hit) {
		return 0.0;
	}

	long long t1;

	if (!is_in(stime, etime, t1)) {
		return 0.0;
	}

	double s;
	return s;
	//simulate(next_start, next_dir, depth+1);
}


void Simulator::update() {

}

void Simulator::set_signal(const long long t, const double s) {
}

Intersection Simulator::check_intersection(const Vec3d &start, const Vec3d &dir) {
	Intersection closest_isct;

	vector<Object>::iterator it = m_sconfig.objects.begin();
	vector<Object>::iterator end = m_sconfig.objects.end();

	for (; it != end; ++it) {
		Intersection isct = it->check_intersection(start, dir);
		if (isct.dist < closest_isct.dist)
			closest_isct = isct;
	}

	return closest_isct;
}

CArray Simulator::get_rx() {
	return m_rx;
}

CArray Simulator::get_tx() {
	return m_tx;
}

