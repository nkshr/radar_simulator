#pragma once

#include "miscel.h"
#include "graph.h"

using namespace std;

class DataServer : public Vertex {
private:
	UDP m_udp;

public:
	DataServer();
	
	virtual bool process();
};