#include "clock.h"

Clock::Clock(const double cf) : m_stop(false), m_num_clock(0), m_num_proc(0), m_num_excess(0),
m_cf(cf), m_target_time(0) {
}

void Clock::init() {
	m_start_time = steady_clock::now();
}

void Clock::start() {
	m_time_per_clock =
		nanoseconds(static_cast<int>(round(1.0e9 / m_cf)));
	m_target_time = get_cur_time();
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
		m_num_excess += (cur_time - m_target_time).count() / m_time_per_clock.count() + 1;
		const nanoseconds rem = nanoseconds(cur_time.count() % m_time_per_clock.count());
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



steady_clock::duration Clock::get_cur_time() {
	return steady_clock::now() - m_start_time;
}
