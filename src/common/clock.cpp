#define DEBUG_CLOCK
#include "clock.hpp"

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

Clock::Clock() : m_stop(false), m_strick(false), m_num_clock(0), m_num_proc(0), m_num_excess(0),
m_cf(10), m_target_time(0), m_delta(0), m_time_after_sleep(0), m_sum_diff(0), m_ref_time(0ll){
	m_ref_time = steady_clock::now().time_since_epoch().count();
	m_base_time = system_clock::now().time_since_epoch().count()*100;
	time_t t = system_clock::to_time_t(system_clock::now());
	cout << ctime(&t) << endl;
}

void Clock::set_time(long long t) {
	m_ref_time = steady_clock::now().time_since_epoch().count();
	m_base_time = t;
	for (list<Clock*>::iterator it = m_clocks.begin(); it != m_clocks.end(); it++) {
		Clock * clock = (*it);
		clock->lock();
		clock->set_time(t);
		clock->unlock();
	}
}

void Clock::start() {
	m_time_per_clock = static_cast<long long>(round(1.0e9 / m_cf));
	m_target_time = m_time_after_sleep = get_time();
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

	long long time_before_sleep = get_time();

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

	m_time_after_sleep = get_time();

#ifdef DEBUG_CLOCK
	cout << "sleep time : " << sleep_time << endl;	
	cout << "target time : " << m_target_time << endl;
	cout << "time after sleep : " << m_time_after_sleep << endl;
	cout << "sum of difference : " << m_sum_diff << endl;
	cout << "difference avgerage : " << (double)m_sum_diff / (double)m_num_clock << endl;
#endif
}

Clock* Clock::clone() {
	Clock * clock = new Clock();
	m_clocks.push_back(clock);
	clock->set_time(get_time());
	return clock;
}

long long Clock::get_time() const{
	return steady_clock::now().time_since_epoch().count() - m_ref_time + m_base_time;
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

void Clock::lock() {
	m_lock.lock();
}

void Clock::unlock() {
	m_lock.unlock();
}