#pragma once
#include <map>
#include <set>
#include <string>
#include <mutex>

#include "common/clock.hpp"
#include "common/miscel.hpp"

using namespace std;

class Graph;
class Vertex;
class Edge;


void foo(const char*s);

typedef Vertex* (Graph::*vcreator)();
typedef Edge* (Graph::*ecreator)();

typedef map<const string, Vertex*> vmap;
typedef map<const string, Edge*> emap;


typedef map<const string, vcreator> vcmap;
typedef map<const string, ecreator> ecmap;

class Graph {
public:
	Graph();

	bool init();
	void run();

	void remove();
	void listen();
	
	void set_port(int port);

	bool set_variable(const string& vname, const string& var, void* value);

	bool create_vertex(const string& vtype, const string& vname);
	bool create_edge(const string& etype, const string& ename);

	bool start(const string& vname);
	void start_all();
	
	//void run(const vector<char*>& vertetxes);

	void stop_all();
	bool stop(const string& vname);

	void lock();
	void unlock();

	const Edge* get_edge(const string& ename);

private:
	mutex m_lock;

	char m_cmd_buf[config::buf_size];

	vmap m_vertexes;
	emap m_edges;

	vcmap m_vcreators;
	ecmap m_ecreators;

	template<typename T>
	Vertex* create_vertex();
	template<typename T>
	Edge* create_edge();

	template<typename T>
	void register_vertex(const string& vtype);
	template<typename T>
	void register_edge(const string& etype);

	CmdServer m_cmd_server;
};