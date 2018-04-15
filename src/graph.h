#pragma once
#include "clock.h"

class Vertex {
public:
	bool run();

private:
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

private:
	int m_num_vertexes;
	int m_num_edges;

	Vertex * vertexes;
	Edge * Edges;

};