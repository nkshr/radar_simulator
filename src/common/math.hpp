#include <iostream>
#include <string>
#include <math.h>
#include <random>
#include <ctime>

#include <Eigen/Dense>
#include <Eigen/Geometry>


using namespace std;
using namespace Eigen;

void get_proj(const float fx, const float fy,
	const float cx, const float cy, Matrix4f &P) {
	P << fx, 0.f, cx, 0.f,
		0.f, fy, cy, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f;
}

//refer to http://www.songho.ca/opengl/gl_projectionmatrix.html
void get_frustum(const float l, const float r,
	const float b, const float t,
	const float n, const float f, Matrix4f &P) {
	float n2 = 2 * n;
	float irl = 1.f / (r - l);
	float itb = 1.f / (t - b);
	float ifn = 1.f / (f - n);

	P <<
		n2 * irl, 0.f, (r + l) * irl, 0.f,
		0.f, n2 * itb, (t + b) * itb, 0.f,
		0.f, 0.f, -(f + n) * ifn, -n2 * f * ifn,
		0.f, 0.f, -1.f, 0.f;
}

void get_ortho(const float l, const float r,
	const float b, const float t,
	const float n, const float f, Matrix4f &P) {
	const float irl = 1.f / (r - l);
	const float itb = 1.f / (t - b);
	const float ifn = 1.f / (f - n);
	P <<
		2.f * irl, 0.f, 0.f, -(r + l) * irl,
		0.f, 2.f * itb, 0.f, -(t + b) * itb,
		0.f, 0.f, -2.f * ifn, -(f + n) * ifn,
		0.f, 0.f, 0.f, 1.f;
}

void get_perspective(const float fov, const float aspect, const float near, const float far, Matrix4f &P) {
	float top = tan(fov / 2) * near;
	float bottom = -top;
	float right = top * aspect;
	float left = -right;
	get_frustum(left, right, bottom, top, near, far, P);
}

void get_Rx(const float ang, Matrix4f &R) {
	const float c = cosf(ang);
	const float s = sinf(ang);
	R <<
		1, 0, 0, 0,
		0, c, -s, 0,
		0, s, c, 0,
		0, 0, 0, 1;
}

void get_Ry(const float ang, Matrix4f &R) {
	const float c = cosf(ang);
	const float s = sinf(ang);
	R <<
		c, 0, -s, 0,
		0, 1, 0, 0,
		s, 0, c, 0,
		0, 0, 0, 1;
}

void get_Rz(const float ang, Matrix4f &R) {
	const float c = cosf(ang);
	const float s = sinf(ang);
	R <<
		c, -s, 0, 0,
		s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;
}


void get_Sim3(const float s, const float xang, const float yang, const float zang,
	const float x, const float y, const float z, Matrix4f &Sim3) {
	Matrix4f Rx, Ry, Rz;
	get_Rx(xang, Rx);
	get_Ry(yang, Ry);
	get_Rz(zang, Rz);

	float _s = 1.f + s;
	Matrix4f m = Rz * Ry * Rx;
	Sim3 <<
		_s * m(0, 0), m(0, 1), m(0, 2), x,
		m(1, 0), _s * m(1, 1), m(1, 2), y,
		m(2, 0), m(2, 1), _s * m(2, 2), z,
		0.f, 0.f, 0.f, 1.f;
}

void get_Sim3(const float s, const Matrix3f& R, const Vector3f& t, Matrix4f &Sim3) {
	float _s = 1.f + s;
	Sim3 <<
		_s * R(0, 0), R(0, 1), R(0, 2), t(0),
		R(1, 0), _s * R(1, 1), R(1, 2), t(1),
		R(2, 0), R(2, 1), _s * R(2, 2), t(2),
		0.f, 0.f, 0.f, 1.f;
}

void get_SE3_inv(const float xang, const float yang, const float zang, const float x, const float y, const float z, Matrix4f &SE3) {
	Matrix4f Rx, Ry, Rz;
	get_Rx(xang, Rx);
	get_Ry(yang, Ry);
	get_Rz(zang, Rz);

	Matrix4f m = Rz * Ry * Rx;
	const float _x = m(0, 0) * x + m(1, 0) * y + m(2, 0) * z;
	const float _y = m(0, 1) * x + m(1, 1) * y + m(2, 1) * z;
	const float _z = m(0, 2) * x + m(1, 2) * y + m(2, 2) * z;
	SE3 <<
		m(0, 0), m(1, 0), m(2, 0), -_x,
		m(0, 1), m(1, 1), m(2, 1), -_y,
		m(0, 2), m(1, 2), m(2, 2), -_z,
		0.f, 0.f, 0.f, 1.f;
}

float adjustment(const float color, const float  factor, const float gamma) {
	if (abs(color) < FLT_EPSILON) {
		return 0.f;
	}
	else {
		return powf(color * factor, gamma);
	}
}

float get_rand(const float max_val, const float min_val) {
	static default_random_engine generator(static_cast<unsigned int>(time(NULL)));
	uniform_real_distribution<float> distribution(0.f, 1.f);
	return distribution(generator);

}
