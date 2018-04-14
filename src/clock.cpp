#include "clock.h"

Clock::Clock(const unsigned int cf) : m_stop(false), m_num_clock(0), m_num_proc(0), m_num_excess(0),
m_cf(cf), m_target_time(0) {
}

void Clock::start() {
	m_time_per_clock = 
		nanoseconds(static_cast<int>(round(1.0e9 / static_cast<double>(m_cf))));
	m_start_time = steady_clock::now();
	cout << m_start_time.time_since_epoch().count() << endl;
}


void Clock::stop() {
	m_proc_rate = static_cast<double>(m_num_proc) / static_cast<double>(m_num_clock);
}

void Clock::adjust() {
	m_num_clock++;
	m_num_proc++;
	steady_clock::duration cur_time = get_cur_time();
	m_target_time += m_time_per_clock;
	nanoseconds sleep_time;

	if (cur_time > m_target_time){
		m_num_excess += (cur_time - m_target_time).count() / m_cf + 1;
		const nanoseconds rem = nanoseconds(cur_time.count() % m_cf);
		sleep_time = m_time_per_clock - rem;
		m_target_time = cur_time + sleep_time;
	}
	else {
		sleep_time = m_target_time - cur_time;
	}

	cout << "sleep time : " << duration_cast<milliseconds>(sleep_time).count() << endl;
	sleep_for(sleep_time);

	cout << "target time : " << 
		duration_cast<milliseconds>(m_target_time).count() << endl;

	cout << "current time : " << duration_cast<milliseconds>(get_cur_time()).count() << endl;
}

bool Clock::is_behind() {
	return false;
}

steady_clock::duration Clock::get_cur_time() {
	steady_clock::time_point n = steady_clock::now();
	return  n - m_start_time;
}
