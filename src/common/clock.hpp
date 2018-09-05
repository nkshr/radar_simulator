#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>
#include <mutex>

using std::chrono::steady_clock;
using std::mutex;

class Clock {

public:
	Clock(const double cf = 10.0);
	
	static void init();
	
	static void set_system_time(long long t);

	void start();
	void stop();
	void adjust();
	
	long long get_steady_time() const;
	long long get_system_time() const;
	long long get_time_per_clock() const;

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

	static steady_clock::time_point m_start_time;
	static long long m_base_time;
	static mutex m_lock;
};