#pragma once
#include <map>
#include <set>
#include <string>

#include "clock.h"
#include "miscel.h"

using namespace std;

class Graph;
class Vertex;
class Edge;

class Vertex {
public:
	Vertex();
	void run();
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

void foo(const char*s);

class Edge {
public:
protected:
	Vertex* m_to;
	Vertex* m_from;
};

typedef void(Graph::*vcreator)(const string&);
typedef void(*ecreator)(const string&);

typedef map<const string, Vertex*> vmap;
typedef map<const string, Edge*> emap;

typedef map<const string, vcreator> vcmap;
typedef map<const string, ecreator> ecmap;

class Graph {
public:
	void init();
	void run();

	void remove();
	void listen();
	
	void set_port(int port);
	
	bool create_vertex(const string& vtype, const string& vname);
	bool create_edge(const string& etype, const string& ename);

	void run(const string& vname);
	void run_all();
	
	//void run(const vector<char*>& vertetxes);

	void stop_all();
	bool stop(const string& vname);

private:
	char m_cmd_buf[config::buf_size];

	vmap m_vertexes;
	emap m_edges;

	vcmap m_vcreators;
	ecmap m_ecreators;

	template <typename T>
	void create_vertex(const string& vname);
	template <typename T>
	void create_edge(const string& ename);

	template <typename T>
	void register_vertex(const string& vtype) {
		m_vcreators.insert(pair<const string, vcreator>(vtype, &Graph::create_vertex<T>));
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
	CmdTerminal();
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