#pragma once


#include <Eigen/Dense>

#include "miscel.hpp"

using namespace Eigen;

void get_proj(const float fx, const float fy,
	const float cx, const float cy, Matrix3f &P);

void get_frustum(const float l, const float r,
	const float b, const float t,
	const float n, const float f, Matrix4f &P);

void get_ortho(const float l, const float r,
	const float b, const float t,
	const float n, const float f, Matrix4f &P);

void get_perspective(const float fov, const float aspect,
	const float near, const float far, Matrix4f &P);

void get_Rx(const float ang, Matrix3f &R);

void get_Ry(const float ang, Matrix3f &R);

void get_Rz(const float ang, Matrix3f &R);

void get_Sim3(const float s, const float xang, const float yang, const float zang,
	const float x, const float y, const float z, Matrix4f &Sim3);

void get_Sim3(const float s, const Matrix3f& R, const Vector3f& t, Matrix4f &Sim3);

void get_SE3_inv(const float xang, const float yang, const float zang, const float x, const float y, const float z, Matrix4f &SE3);

float adjustment(const float color, const float  factor, const float gamma);

void calc_epi_line(const float x, const float y, const Matrix3f& E, Vector3f& epi_line);

void get_cross_product(const Vector3f& v, Matrix3f& cp);

void calc_essential_matrix(const Matrix3f& R, const Vector3f& t,
	Matrix3f& E);

void estimate_R_and_t_between_images(
	const Matrix3f& prev_R, const Vector3f& prev_t,
	const Image& prev_img, const Image img,
	Matrix3f& R, Vector3f& t);

struct Info3D {
	Matrix3f R;
	Vector3f t;
};