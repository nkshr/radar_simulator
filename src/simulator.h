#pragma once

#include <vector>

#include "config.h"
#include "math.h"

using namespace std;

struct Object;

struct Intersection {
	bool hit;
	double dist;
	long long t;
	Object *obj;

	Intersection():hit(true), dist(DBL_MAX), t(0), obj(nullptr) {
	}
};

struct  Object {
	Object(double  _x, double _reflectance = 1.0) : x(_x), reflectance(_reflectance) {};
	double x;
	double reflectance;

	//Vec3d x;

	Intersection check_intersection(const Vec3d &start, const Vec3d &dir) {
		//Vec3d d = (x - start).normalize();
		Intersection isct;
		return isct;
	}

};

struct datum {
	double x, t;
	double delay;
};

struct Radar {
	double pulse_width; //pulse width
	double bandwidth; //bandwidth
	double prf; //pulse repetition frequency
	double gain;
	double sampling_rate; //sampling rate
	double anntena_rpm; //anntena revolution per minutes

	Vec3d pos;
	Vec3d dir;

	Radar(): pulse_width(1.0), bandwidth(1.0), prf(10.0), gain(1.0), sampling_rate(100.0), 
		anntena_rpm(24.0), pos(Vec3d::zeros()), dir(Vec3d::e0()){
	
	}

};

struct Signal {
	long long t;
	double *data;
};

struct SConfig {
	int max_depth;

	Radar radar;
	vector<Object> objects;
};

class Simulator {
	//Radar m_radar;
	//std::vector<Object> m_objects;
	bool m_stop;

	CArray m_rx;
	CArray m_tx;

	SConfig m_sconfig;

public:
	Simulator(const SConfig &sconfig);
	void init();
	void simulate(const Radar &radar, std::vector<Object> &objects);
	double simulate(const Vec3d &start, const Vec3d &dir,
		const int depth, const long long stime, const long long etime, const long long t);
	double simulate(const long long t);
	void update();
	void set_signal(const long long t, const double s);

	Intersection check_intersection(const Vec3d &start, const Vec3d &dir);

	CArray get_rx();
	CArray get_tx();


};
