#define DEBUG_CLOCK
#include "clock.hpp"

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

steady_clock::time_point Clock::m_start_time;
long long Clock::m_base_time;
mutex Clock::m_lock;

Clock::Clock(const double cf) : m_stop(false), m_strick(false), m_num_clock(0), m_num_proc(0), m_num_excess(0),
m_cf(cf), m_target_time(0), m_delta(0), m_time_after_sleep(0), m_sum_diff(0){
}

void Clock::init() {
	unique_lock<mutex> lock(m_lock);
	m_start_time = steady_clock::now();
	m_base_time = 0;
}

void Clock::set_system_time(long long t) {
	m_base_time = t;
}

void Clock::start() {
	m_time_per_clock = static_cast<long long>(round(1.0e9 / m_cf));
	m_target_time = m_time_after_sleep = get_steady_time();
}


void Clock::stop() {
	m_proc_rate = static_cast<double>(m_num_proc) / static_cast<double>(m_num_clock);
}

void Clock::adjust() {
	m_num_clock++;
	m_num_proc++;
	
	long long diff = m_target_time - m_time_after_sleep;
	m_sum_diff += abs(diff);

	if(m_strick)
		m_delta = m_delta * 0.9 + diff * 0.1;
	else
	m_delta = 0;

	long long time_before_sleep = get_steady_time();

	m_base_time += time_before_sleep;

	m_target_time += m_time_per_clock;


	long long sleep_time;

	if (time_before_sleep > m_target_time){
		m_num_excess += (time_before_sleep - m_target_time) / m_time_per_clock + 1;
		const long long rem = time_before_sleep % m_time_per_clock;
		sleep_time = m_time_per_clock - rem + m_delta;
		m_target_time = time_before_sleep + sleep_time;
	}
	else {
		sleep_time = m_target_time - time_before_sleep + m_delta;
	}

	sleep_for(nanoseconds(sleep_time));

	m_time_after_sleep = get_steady_time();

#ifdef DEBUG_CLOCK
	cout << "sleep time : " << sleep_time << endl;	
	cout << "target time : " << m_target_time << endl;
	cout << "time after sleep : " << m_time_after_sleep << endl;
	cout << "sum of difference : " << m_sum_diff << endl;
	cout << "difference avgerage : " << (double)m_sum_diff / (double)m_num_clock << endl;
#endif
}



long long Clock::get_steady_time() const{
	return nanoseconds((steady_clock::now() - m_start_time)).count();
}

long long Clock::get_system_time() const {
	unique_lock<mutex> lock(m_lock);
	return get_steady_time() + m_base_time;
}

long long Clock::get_time_per_clock() const {
	return m_time_per_clock;
}

double Clock::get_clock_freq() const {
	return m_cf;
}

void Clock::set_clock_freq(double cf){
	m_cf = cf;
	m_time_per_clock = static_cast<long long>(round(1.0e9 / m_cf));
}