#pragma once

#include <complex>
#include <iostream>
#include <valarray>

namespace constant {
	constexpr double pi = 3.141592653589793238460;
	constexpr double c = 3.0e8;
}


typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

// Cooley–Tukey FFT (in-place, divide-and-conquer)
// Higher memory requirements and redundancy although more intuitive
inline void fft(CArray& x)
{
	const size_t N = x.size();
	if (N <= 1) return;

	// divide
	CArray even = x[std::slice(0, N / 2, 2)];
	CArray  odd = x[std::slice(1, N / 2, 2)];

	// conquer
	fft(even);
	fft(odd);

	// combine
	for (size_t k = 0; k < N / 2; ++k)
	{
		Complex t = std::polar(1.0, -2 * constant::pi * k / N) * odd[k];
		x[k] = even[k] + t;
		x[k + N / 2] = even[k] - t;
	}
}

inline std::complex<double> W(int kn, int length)
{
	const std::complex<double> j = std::complex<double>(0.0, 1.0);
	return std::exp(-2.0 * constant::pi * j * double(kn) / double(length));
}

inline void FFT(CArray &x)
{
	if (x.size() <= 1) {
		return;
	}

	const size_t half = x.size() / 2;
	CArray even(half);
	CArray odd(half);

	for (int i = 0; i < half; ++i) {
		even[i] = x[i] + x[half + i];
		odd[i] = (x[i] - x[half + i]) * W(i, x.size());
	}

	FFT(even);
	FFT(odd);

	for (int i = 0; i < half; ++i) {
		const int ii = i * 2;
		x[ii] = even[i];
		x[ii + 1] = odd[i];
	}
}

inline CArray dft(const CArray &src) {
	const int N = src.size();
	CArray dst(N);
	for (int k = 0; k < N; ++k) {
		double real = 0;
		double imag = 0;
		for (int n = 0; n < N; ++n) {
			real += src[n].real() * (cos((2 * constant::pi / N) * k * n)) 
				+ src[n].imag() * (sin((2 * constant::pi / N) * k * n));
			imag += src[n].real() * (-sin((2 * constant::pi / N) * k * n)) 
				+ src[n].imag() * (cos((2 * constant::pi / N) * k * n));
		}
		dst[k] = Complex(real, imag);
	}

	return dst;
}

inline std::valarray<double> generate_power_spectrum(const CArray &carray) {
	std::valarray<double> ps(carray.size());
	for (int i = 0; i < ps.size(); ++i) {
		ps[i] = sqrt(pow(carray[i].real(), 2) + pow(carray[i].imag(), 2));
	}
}

inline CArray generate_wave(const double f, const double step_time, const int size) {
	CArray wave(size);
	for (int i = 0; i < size; ++i) {
		const double t = i * step_time;
		wave[i] =  exp(Complex(0, 2 * constant::pi * f * t));
	}

	return wave;
}

// Cooley-Tukey FFT (in-place, breadth-first, decimation-in-frequency)
// Better optimized but less intuitive
// !!! Warning : in some cases this code make result different from not optimased version above (need to fix bug)
// The bug is now fixed @2017/05/30 
//void fft(CArray &x)
//{
//	// DFT
//	unsigned int N = x.size(), k = N, n;
//	double thetaT = 3.14159265358979323846264338328L / N;
//	Complex phiT = Complex(cos(thetaT), -sin(thetaT)), T;
//	while (k > 1)
//	{
//		n = k;
//		k >>= 1;
//		phiT = phiT * phiT;
//		T = 1.0L;
//		for (unsigned int l = 0; l < k; l++)
//		{
//			for (unsigned int a = l; a < N; a += n)
//			{
//				unsigned int b = a + k;
//				Complex t = x[a] - x[b];
//				x[a] += x[b];
//				x[b] = t * T;
//			}
//			T *= phiT;
//		}
//	}
//	// Decimate
//	unsigned int m = (unsigned int)log2(N);
//	for (unsigned int a = 0; a < N; a++)
//	{
//		unsigned int b = a;
//		// Reverse bits
//		b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
//		b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
//		b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
//		b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
//		b = ((b >> 16) | (b << 16)) >> (32 - m);
//		if (b > a)
//		{
//			Complex t = x[a];
//			x[a] = x[b];
//			x[b] = t;
//		}
//	}
//	//// Normalize (This section make it not working correctly)
//	//Complex f = 1.0 / sqrt(N);
//	//for (unsigned int i = 0; i < N; i++)
//	//	x[i] *= f;
//}

// inverse fft (in-place)
inline void ifft(CArray& x)
{
	// conjugate the complex numbers
	x = x.apply(std::conj);

	// forward fft
	fft(x);

	// conjugate the complex numbers again
	x = x.apply(std::conj);

	// scale the numbers
	x /= (double)x.size();
}


//inline Complex exp(Complex a) {
//	Complex ret;
//	
//}

struct Vec2d {
	Vec2d(double _x = 0.0, double _y = 0.0) {
		x = _x;
		y = _y;
	}

	double x, y;

};

struct Vec3d {
	Vec3d(double _x = 0.0, double _y = 0.0, double _z = 0.0) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vec3d(const Vec3d& v) {
		x = v.x;
		y = v.y;
		z = v.z;
	}

	double x, y, z;

	Vec3d normalize() {
		const double recp_d = 1.0 / sqrt(x * x + y * y + z * z);
		Vec3d nv;
		nv.x = x * recp_d;
		nv.y = y * recp_d;
		nv.z = z * recp_d;
	}

	double norm() {
		return sqrt(x * x + y * y + z * z);
	}

	static Vec3d zeros() {
		return Vec3d();
	}

	static Vec3d e0() {
		return Vec3d(1.0, 0.0, 0.0);
	}
};


inline Vec2d operator+(const Vec2d &left, const Vec2d &right) {
	Vec2d ret;
	ret.x = left.x + right.x;
	ret.y = left.y + right.y;
	return ret;
}

inline Vec3d operator+(const Vec3d &left, const Vec3d &right) {
	Vec3d ret;
	ret.x = left.x + right.x;
	ret.y = left.y + right.y;
	ret.z = left.z + right.z;
	return ret;
}

inline Vec3d operator-(const Vec3d &left, const Vec3d &right) {
	Vec3d ret;
	ret.x = left.x - right.x;
	ret.y = left.y - right.y;
	ret.z = left.z - right.z;
	return ret;
}

inline double operator*(const Vec3d &left, const Vec3d &right) {
	return left.x * right.x + left.y * right.y + left.z * right.z;
}

inline bool is_in(const double min, const double max, const double x) {
	return min <= x ? x < max : false;
}