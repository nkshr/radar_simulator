//#define DEBUG_CLOCK
#include <string>
#include "clock.hpp"

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

Clock::Clock() : m_stop(false), m_strick(false), m_num_clocks(0), m_num_procs(0),
m_num_expired(0),
m_cf(10), m_target_time(0), m_delta(0), m_sum_diff(0), m_ref_time(0ll){
	m_ref_time = steady_clock::now().time_since_epoch().count();
	m_base_time = system_clock::now().time_since_epoch().count()*100;
	time_t t = system_clock::to_time_t(system_clock::now());
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
	m_target_time = get_time();
}


void Clock::stop() {
	m_proc_rate = static_cast<double>(m_num_procs) / static_cast<double>(m_num_clocks);
}

void Clock::update() {
	m_num_clocks++;
	m_num_procs++;
	
	//long long diff = m_target_time - m_time_after_sleep;
	//m_sum_diff += abs(diff);

	//if(m_strick)
	//	m_delta = m_delta * 0.9 + diff * 0.1;
	//else
	//	m_delta = 0;
	m_delta = 0;

	long long time_before_sleep = get_time();

	m_target_time += m_time_per_clock;


	if (time_before_sleep > m_target_time){
		long long tmp = (time_before_sleep - m_target_time) / m_time_per_clock;
		if (tmp > 0)
			tmp += 1;

		m_num_clocks += tmp;

		m_num_expired += tmp;
		const long long rem = time_before_sleep % m_time_per_clock;
		m_sleep_time = m_time_per_clock - rem + m_delta;
		m_target_time = time_before_sleep + m_sleep_time;
	}
	else {
		m_sleep_time = m_target_time - time_before_sleep + m_delta;
	}

	//sleep_for(nanoseconds(sleep_time));

	//m_time_after_sleep = get_time();
	//return sleep_time;
#ifdef DEBUG_CLOCK
	cout << "sleep time : " << sleep_time << endl;	
	cout << "target time : " << m_target_time << endl;
	cout << "time after sleep : " << m_time_after_sleep << endl;
	cout << "sum of difference : " << m_sum_diff << endl;
	cout << "difference avgerage : " << (double)m_sum_diff / (double)m_num_clock << endl;
#endif
}

long long Clock::get_sleep_time() {
	return m_sleep_time;
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

string Clock::get_info_str() {
	lock_guard<mutex> lock(m_lock);
	m_proc_rate = static_cast<double>(m_num_procs) / static_cast<double>(m_num_clocks);
	string str = "proc rate : " + to_string(m_num_procs) + "/" + to_string(m_num_clocks) 
		+ "(" + to_string(m_proc_rate*100) + "\%)";
	return str;
}