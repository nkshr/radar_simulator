#pragma once

#include <vector>

#include "config.h"
#include "math.h"
#include "object.h"
#include "graph.h"

using namespace std;

struct datum {
	double x, t;
	double delay;
};

struct Signal {
	long long t;
	double *data;
};

struct SConfig {
	int max_depth;

	Radar radar;
	vector<Object*> objects;
};

class Simulator : public Vertex{
private:
	long long m_last_pulse_time;

	double m_pulse_interval;
	
	bool m_stop;

	CArray m_rx;
	CArray m_tx;

	SConfig m_sconfig;

	RadarSignal *m_rs;

public:
	Simulator(const SConfig &sconfig);
	void init();

	virtual bool process();

	void simulate(const Radar &radar, std::vector<Object> &objects);
	double simulate(const Vec3d &start, const Vec3d &dir,
		int depth, long long stime, long long etime);
	void set_signal(const long long t, const double s);

	Intersection check_intersection(const Vec3d &start, const Vec3d &dir);

	CArray get_rx();
	CArray get_tx();


};
