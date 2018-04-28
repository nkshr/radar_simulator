#pragma once
#include "math.h"

struct Object;

struct Intersection {
	bool hit;
	double dist;
	long long time;
	Object *obj;

	Intersection() :hit(true), dist(DBL_MAX), time(0), obj(nullptr) {
	}
};

struct  Object {
	Object(const Vec3d &_pos, double _reflectance) : pos(_pos), reflectance(_reflectance) {};
	double reflectance;

	Vec3d pos;

	virtual Intersection check_intersection(const Vec3d &start, const Vec3d &dir) = 0;
};

struct Sphere : public Object {
	double radius;

	Sphere(const Vec3d &_pos, double _reflectance, double _radius);
	virtual Intersection check_intersection(const Vec3d & start, const Vec3d & dir);
};

struct Radar {
	double pulse_width; //pulse width
	double bandwidth; //bandwidth
	double prf; //pulse repetition frequency
	double gain;
	double sampling_rate; //sampling rate
	double anntena_rpm; //anntena revolution per minutes
	double pulse_interval;

	Vec3d pos;
	Vec3d dir;

	Radar() : pulse_width(1.0), bandwidth(1.0), prf(10.0), gain(1.0), sampling_rate(100.0),
		anntena_rpm(24.0), pos(Vec3d::zeros()), dir(Vec3d::e0()) {}
};