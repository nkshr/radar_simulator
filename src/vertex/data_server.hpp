#pragma once

#include "../common/miscel.hpp"
#include "vertex.hpp"

using namespace std;

class DataServer : public Vertex {
private:
	UDP m_udp;

public:
	DataServer();
	
	virtual bool process();
};