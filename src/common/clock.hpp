#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>

using std::chrono::steady_clock;

class Clock {

public:
	Clock(const double cf = 10.0);
	
	static void init();
	
	void start();
	void stop();
	void adjust();
	
	long long get_cur_time() const;
	long long get_time_per_clock() const;

private:
	bool m_stop;
	unsigned int m_cf;
	long long m_num_clock;
	long long m_num_proc;
	long long m_num_excess;
	long long m_time_per_clock;
	long long m_target_time;

	double m_proc_rate;

	static steady_clock::time_point m_start_time;
	
};