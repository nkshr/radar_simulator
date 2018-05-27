#include "../graph.hpp"

class Edge {
public:
protected:
	Vertex* m_vertex0;
	Vertex* m_vertex1;
	Graph* m_graph;
};

class RadarSignal : protected Edge {
public:
	RadarSignal(int buf_size);
	~RadarSignal();

	void set_signal(double d, long long t);

private:
	int m_idx;
	int m_buf_size;
	double* m_buf;
	long long* m_times;
};