#include <vector>

#include "../board.hpp"

using namespace std;

class Signal {
public:
protected:
	Board* m_board;
};

class RadarSignal : protected Signal {
public:
	RadarSignal(int buf_size);
	~RadarSignal();

	void set_signal(double d, long long t);

private:
	int m_idx;
	int m_buf_size;
	double* m_buf;
	long long* m_times;
};