#pragma once
#pragma once

#include <vector>

#include "config.h"
#include "math.h"

struct  Object {
	Object(double  _x, double _reflectance = 1.0) : x(_x), reflectance(_reflectance) {};
	double x;
	double reflectance;
};

struct datum {
	double x, t;
	double delay;
};

struct Radar {
	double x;
	double pulse_width;//micro second
	double bandwidth;//MHz
	double pulse_interval;
	double gain;
};

struct SConfig {
	int num_samples;
	int max_depth;
	double sample_time;
};

class Simulator {
	//Radar m_radar;
	//std::vector<Object> m_objects;
	CArray m_rx;
	CArray m_tx;

	SConfig m_sconfig;

public:
	Simulator(const SConfig &sconfig);
	void init();
	void simulate(const Radar &radar, std::vector<Object> &objects);
	void simulate(const Vec2d &start, const Vec3d &dir, const int depth);
	void update();

	CArray get_rx();
	CArray get_tx();


};
