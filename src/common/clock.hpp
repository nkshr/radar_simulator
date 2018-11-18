#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>
#include <mutex>
#include <list>

using std::chrono::steady_clock;
using std::mutex;
using std::list;

class Clock {

public:
	Clock();
	
	void init();
	
	void set_system_time(long long t);

	void start();
	void stop();
	void adjust();
	
	long long get_steady_time() const;
	long long get_system_time();
	long long get_time_per_clock() const;
	double get_clock_freq() const;

	void set_clock_freq(double cf);

	Clock* clone();

	void lock();
	void unlock();

private:
	bool m_stop;
	bool m_strick;

	unsigned int m_cf; //clock frequency

	long long m_num_clock;
	long long m_num_proc;
	long long m_num_excess;
	long long m_time_per_clock;
	long long m_target_time;
	long long m_delta;
	long long m_time_after_sleep;
	long long m_sum_diff;

	double m_proc_rate;

	long long m_ref_ste_time;

	long long m_ref_sys_time;

	mutex m_lock;

	list<Clock*> m_clocks;
};