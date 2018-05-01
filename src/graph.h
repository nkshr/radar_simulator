#pragma once
#include "clock.h"
#include "miscel.h"

class Graph;

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

class Edge {
public:
protected:
	char m_vname[1024];
	Vertex* m_to;
	Vertex* m_from;
};

typedef void(*vcreator)(const char*);
typedef void(*ecreator)(const char*);

class Graph {
public:
	void run();
	void add_vertex(Vertex* v);

	void remove();
	void listen();

	void set_port(int port);
	
	bool create_vertex(const char* vtype, const char* vname);
	bool create_edge(const char* etype, const char* ename);

	void run_all();
	void run(const vector<char*>& vertetxes);

	void stop_all();
	void stop(const vector<char*>& vertexes);


private:
	char m_cmd_buf[config::buf_size];

	vector<Vertex*> m_vertexes;
	vector<char*> m_vertex_types;

	vector<Edge*> m_edges;
	vector<char*> m_edge_types;

	template <typename T>
	void create_vertex(const char* vname);
	template <typename T>
	void create_edge(const char* ename);

	vector<vcreator> m_vcreators;
	vector<ecreator> m_ecreators;
	
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