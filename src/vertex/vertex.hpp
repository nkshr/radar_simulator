#pragma once
#include <map>
#include <set>
#include <string>

#include "../common/clock.hpp"
#include "../common/miscel.hpp"

#include "../graph.hpp"

class Vertex {
public:
	Vertex();
	void start();
	void join();
	void processing_loop();
	void stop();

	virtual bool process() = 0;

protected:
	bool m_brun;

	thread m_th;

	Clock m_clock;
	Graph *m_graph;
};
