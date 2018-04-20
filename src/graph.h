#pragma once
#include "clock.h"

class Vertex {
public:
	void run();
	void join();
	void processing_loop();

private:
	bool m_brun;

	thread m_th;
	Clock m_clock;

	virtual bool process() = 0;
};

class Edge {
};

class Graph {
public:
	void run();
	void add();
	void remove();
	template<typename T>
	void set_edges(const char * vertex_name, const char * variable_name, T value);
	void listen();

private:
	int m_num_vertexes;
	int m_num_edges;

	Vertex * m_vertexes;
	Edge * m_edges;

};

class RadarSignal : protected Edge{
public:
	RadarSignal(int buf_size);
	~RadarSignal();

	void set_signal(double d, long long t);

private:
	int m_idx;
	int m_buf_size;
	double * m_buf;
	long long * m_times;
};