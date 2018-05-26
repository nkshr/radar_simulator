#include "object.h"

Sphere::Sphere(const Vec3d &_pos, double _reflectance, double  _radius) :
	Object(_pos, _reflectance), radius(_radius)
{

}

Intersection Sphere::check_intersection(const Vec3d &start, const Vec3d &dir) {
	Intersection isct;

	const Vec3d v = start - pos;
	const double b = 2 * dir * v;
	const double c = v * v - radius * radius;
	const double d = b - 4 * c;

	if (d < 0) {
		isct.hit = false;
		return isct;
	}

	isct.hit = true;

	if (d < DBL_EPSILON) {
		isct.dist = -b;
		return isct;
	}

	const double e= sqrt(d);

	const double t0 = (-b + e * 0.5);
	const double t1 = (-b - e * 0.5);

	if (t0 < t1) {
		isct.dist = t0;
	}
	else {
		isct.dist = t1;
	}
	return isct;
}