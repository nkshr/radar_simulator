#pragma once
#include "clock.h"

class Vertex {
public:
//	Vertex() {};

	void run();
	void join();
	void processing_loop();

	virtual bool process() = 0;

protected:
	bool m_brun;

	thread m_th;

	Clock m_clock;

};

class Edge {
};

class Graph {
public:
	void run();
	void add_vertex(Vertex* v);

	void remove();
	void listen();

private:
	vector<Vertex*> m_vertexes;
	vector<Edge*> m_edges;

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