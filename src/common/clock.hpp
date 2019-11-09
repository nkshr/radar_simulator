#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>
#include <mutex>
#include <list>
#include <string>

using std::chrono::steady_clock;
using std::mutex;
using std::list;
using std::string;

class Clock {

public:
	Clock();
	
	void init();
	
	void set_time(long long t);

	void start();
	void stop();
	void update();
	long long get_sleep_time();
	
	long long get_time() const;
	long long get_time_per_clock() const;
	double get_clock_freq() const;

	void set_clock_freq(double cf);

	Clock* clone();

	void lock();
	void unlock();

	string get_info_str();

private:
	bool m_stop;
	bool m_strick;

	unsigned int m_cf; //clock frequency

	long long m_num_clocks;
	long long m_num_procs;
	long long m_num_expired;
	long long m_time_per_clock;
	long long m_target_time;
	long long m_delta;
	//long long m_time_after_sleep;
	long long m_sum_diff;

	double m_proc_rate;

	long long m_ref_time;
	long long m_base_time;
	long long m_sleep_time;

	mutex m_lock;

	list<Clock*> m_clocks;
};