#pragma once

#include <vector>

#include "../common/config.hpp"
#include "../common/math.hpp"
#include "../common/object.hpp"
#include "module.hpp"


struct datum {
	double x, t;
	double delay;
};

struct SConfig {
	int max_depth;

	Radar radar;
	vector<Object*> objects;
};

class Simulator : public Module{
private:
	long long m_last_pulse_time;

	double m_pulse_interval;
	
	bool m_stop;
	
	CArray m_rx;
	CArray m_tx;

	SConfig m_sconfig;
	MemInt* m_max_depth;

	MemShip* m_onw_ship;
	//RadarSignal *m_rs;

public:
	Simulator();
	virtual bool init();

	virtual bool process();

	void simulate(const Radar &radar, std::vector<Object> &objects);
	double simulate(const Vec3d &start, const Vec3d &dir,
		int depth, long long stime, long long etime);
	void set_signal(const long long t, const double s);

	Intersection check_intersection(const Vec3d &start, const Vec3d &dir);

	CArray get_rx();
	CArray get_tx();


};
