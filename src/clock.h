#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

class Clock {

public:
	Clock(const unsigned int cf);
	void start();
	void stop();
	void adjust();
	bool is_behind();
	steady_clock::duration get_cur_time();

private:
	bool m_stop;
	unsigned int m_cf;
	long long m_num_clock;
	long long m_num_proc;
	long long m_num_excess;
	double m_proc_rate;

	nanoseconds m_time_per_clock;
	steady_clock::time_point m_start_time;
	steady_clock::duration m_target_time;
};