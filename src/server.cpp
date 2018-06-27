#include <iostream>
#include <fstream>

#include  "common/math.hpp"
#include "board.hpp"

#define REAL 0
#define IMAG 1
#define AMP 2
#define REAL_AMP 3
#define IMAG_AMP 4

inline bool write_data(const char * fname, CArray &carray,
	const double step_time, const int type = REAL) {
	std::ofstream ofs(fname);
	if (!ofs.good()) {
		return false;
	}

	double half_time = step_time * (carray.size() / 2);

	switch (type) {
	case REAL:
		for (int i = 0; i < carray.size(); ++i) {
			double t = i;
			ofs << carray[i].real() << ", " << i * step_time - half_time << "\n";
		}
		break;

	case IMAG:
		for (int i = 0; i < carray.size(); ++i) {
			double t = i;
			ofs << carray[i].imag() << ", " << i * step_time - half_time << "\n";
		}
		break;

	case AMP:
	{
		const double s = 1.0 / carray.size();
		for (int i = 0; i < carray.size(); ++i) {
			double t = i;
			ofs << sqrt(pow(carray[i].real(), 2) + pow(carray[i].imag(), 2)) * s << ", " << i * step_time - half_time << "\n";
		}
	}
		break;

	case REAL_AMP:
	{
		const double s = 1.0 / carray.size();
		for (int i = 0; i < carray.size(); ++i) {
			double t = i;
			ofs << sqrt(pow(carray[i].real(), 2)) * s << ", " << i * step_time - half_time << "\n";
		}
	}
	break;

	case IMAG_AMP:
	{
		const double s = 1.0 / carray.size();
		for (int i = 0; i < carray.size(); ++i) {
			double t = i;
			ofs << sqrt(pow(carray[i].imag(), 2)) * s << ", " << i * step_time - half_time << "\n";
		}
	}
	break;

	}
	return true;
}

inline CArray expand(const CArray &carray, const int new_size) {
	CArray ecarray(carray.size() + new_size);

	const int diff = carray.size();
	for (int i = 0; i < carray.size(); ++i) {
		ecarray[i] = carray[i];
	}

	for (int i = carray.size(); i < new_size; ++i) {
		ecarray[i] = Complex(0, 0);
	}
	return ecarray;
}
int main()
{

	Board board;
	board.run();

	return 0;
}