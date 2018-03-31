#include <iostream>
#include <fstream>

#include "simulator.h"
#include  "math.h"

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
	//const Complex test[] = { 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0 };
	//CArray data(test, 8);

	//// forward fft
	//fft(data);

	//std::cout << "fft" << std::endl;
	//for (int i = 0; i < 8; ++i)
	//{
	//	std::cout << data[i] << std::endl;
	//}

	//// inverse fft
	//ifft(data);

	//std::cout << std::endl << "ifft" << std::endl;
	//for (int i = 0; i < 8; ++i)
	//{
	//	std::cout << data[i] << std::endl;
	//}

	Radar radar;
	radar.x = 0;
	radar.bandwidth = 10e6;
	radar.pulse_width = 10e-6;
	radar.pulse_interval = 1000e-6;
	radar.gain = 1;

	std::vector<Object> objects;
	objects.push_back(Object(15));
	objects.push_back(Object(30, 0.5));
	objects.push_back(Object(100, 2));

	SConfig sconfig;
	sconfig.num_samples = config::num_samples;
	sconfig.sample_time = config::sample_time;

	Simulator simulator(sconfig);
	simulator.simulate( radar, objects);

	CArray rx = simulator.get_rx();
	write_data("rx.csv", rx, config::sample_time);
	
	CArray tx = simulator.get_tx();
	write_data("tx.csv", tx, config::sample_time);

	write_data("tx.csv", tx, 1024);

	tx = dft(tx);
	write_data("tx_fft.csv", tx, 1e-6, AMP);

	rx = dft(rx);
	write_data("rx_fft.csv", rx, 1e-6, REAL_AMP);
	return 0;
}