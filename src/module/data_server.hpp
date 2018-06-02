#pragma once

#include "../common/miscel.hpp"
#include "module.hpp"

using namespace std;

class DataServer : public Module {
private:
	UDP m_udp;

public:
	DataServer();
	
	virtual bool process();
};