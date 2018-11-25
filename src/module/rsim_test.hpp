#pragma once

#include <ctime>

#include "module.hpp"

using std::cout;
using std::endl;

class RsimTest : public Module {
protected:
	bool m_bprint;
public:
	RsimTest(Board * board) : Module(board){
		register_bool("print", "boolean value for printing time.(default y)", true, &m_bprint);
	}

	virtual bool init() {
		debug_msg dmsg("init");
		return true;
	};

	virtual bool process() {

		if (m_bprint) {			
			cout << get_time_by_string() << endl;
		}
		return true;
	};

	virtual bool finish() {
		debug_msg dmsg("finish");
		return true;
	};

};