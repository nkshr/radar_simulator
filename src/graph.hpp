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

typedef void(Graph::*vcreator)(const string&);
typedef void(*ecreator)(const string&);

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
	
	bool create_vertex(const string& vtype, const string& vname);
	bool create_edge(const string& etype, const string& ename);

	bool start(const string& vname);
	void start_all();
	
	//void run(const vector<char*>& vertetxes);

	void stop_all();
	bool stop(const string& vname);

	void lock();
	void unlock();

private:
	mutex m_lock;

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

	CmdServer m_cmd_server;
};