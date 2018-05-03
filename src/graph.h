#pragma once
#include <map>
#include <set>

#include "clock.h"
#include "miscel.h"

using namespace std;

class Graph;
class Vertex;
class Edge;

class Vertex {
public:
	Vertex(const char* vname);

	void run();
	void join();
	void processing_loop();
	void stop();

	virtual bool process() = 0;

protected:
	char m_vname[1024];
	bool m_brun;

	thread m_th;

	Clock m_clock;
	Graph *m_graph;
};

void foo(const char*s);

class Edge {
public:
protected:
	char m_vname[1024];
	Vertex* m_to;
	Vertex* m_from;
};

typedef void(Graph::*vcreator)(const char*);
typedef void(*ecreator)(const char*);

typedef map<const char*, Vertex*, string_comparator> vmap;
typedef map<const char*, Edge*, string_comparator> emap;

typedef map<const char*, vcreator, string_comparator> vcmap;
typedef map<const char*, ecreator, string_comparator> ecmap;

class Graph {
public:
	void init();
	void run();

	void remove();
	void listen();
	
	void set_port(int port);
	
	bool create_vertex(const char* vtype, const char* vname);
	bool create_edge(const char* etype, const char* ename);

	void run_all();
	void run(const vector<char*>& vertetxes);

	void stop_all();
	bool stop(const char* vname);

private:
	char m_cmd_buf[config::buf_size];

	vmap m_vertexes;
	emap m_edges;

	vcmap m_vcreators;
	ecmap m_ecreators;

	template <typename T>
	void create_vertex(const char* vname);
	template <typename T>
	void create_edge(const char* ename);

	template <typename T>
	void register_vertex(const char* vtype) {
		m_vcreators.insert(pair<const char*, vcreator>(vtype, &Graph::create_vertex<T>));
	}

	UDP m_udp;
	CmdParser m_cp;
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
	CmdTerminal(const char* vname) : Vertex(vname){};
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