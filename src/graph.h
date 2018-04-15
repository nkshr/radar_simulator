#pragma once

class Vertex {
	virtual  bool run();
};

class Edge {
	virtual bool run();
};

class Graph {
public:
	void run();
	void add();
	void remove();

private:
	Vertex * vertexes;
	Edge * Edges;
};