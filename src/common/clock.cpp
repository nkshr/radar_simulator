#define DEBUG_CLOCK
#include "clock.hpp"

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

steady_clock::time_point Clock::m_start_time;
long long Clock::m_base_time;
mutex Clock::m_lock;

Clock::Clock(const double cf) : m_stop(false), m_num_clock(0), m_num_proc(0), m_num_excess(0),
m_cf(cf), m_target_time(0) {
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
	m_target_time = get_steady_time();
}


void Clock::stop() {
	m_proc_rate = static_cast<double>(m_num_proc) / static_cast<double>(m_num_clock);
}

void Clock::adjust() {
	m_num_clock++;
	m_num_proc++;
	
	const long long cur_time = get_steady_time();
	m_base_time += cur_time;

	m_target_time += m_time_per_clock;

	long long sleep_time;

	if (cur_time > m_target_time){
		m_num_excess += (cur_time - m_target_time) / m_time_per_clock + 1;
		const long long rem = cur_time % m_time_per_clock;
		sleep_time = m_time_per_clock - rem;
		m_target_time = cur_time + sleep_time;
	}
	else {
		sleep_time = m_target_time - cur_time;
	}

#ifdef DEBUG_CLOCK
	cout << "sleep time : " << duration_cast<milliseconds>(nanoseconds(sleep_time)).count() << endl;
	sleep_for(nanoseconds(sleep_time));
	
	cout << "target time : " << 
		duration_cast<milliseconds>(nanoseconds(m_target_time)).count() << endl;

	cout << "current time : " << duration_cast<milliseconds>(nanoseconds(get_steady_time())).count() << endl;
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