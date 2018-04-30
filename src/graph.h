#pragma once
#include "clock.h"
#include "miscel.h"

class Graph;

class Vertex {
public:
	void run();
	void join();
	void processing_loop();

	virtual bool process() = 0;

protected:
	bool m_brun;

	thread m_th;

	Clock m_clock;
	Graph *m_graph;
};

class Edge {
};

class Graph {
public:
	void run();
	void add_vertex(Vertex* v);

	void remove();
	void listen();

	void set_port(int port);
	
	bool create_vertex(const char* vtype, const char* vname);
	bool create_edge(const char* etype, const char* ename);

private:
	char m_cmd_buf[config::buf_size];

	vector<Vertex*> m_vertexes;
	vector<char*> m_vertex_types;

	vector<Edge*> m_edges;
	vector<char*> m_edge_types;
};

class CmdReceiver : public Vertex {
public:
	bool process();

	void set_port(int port);

private:
	CmdParser m_cp;
	UDP m_udp;
	char m_err_msg[config::buf_size];
};

class CmdTerminal : public Vertex{
public:
	bool process();
	
private:
	char m_buf[config::buf_size];
	char m_rep[config::buf_size];

	CmdParser m_cmd_parser;
	UDP m_udp;
};

class RadarSignal : protected Edge{
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