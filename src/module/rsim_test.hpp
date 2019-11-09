#pragma once

#include <ctime>

#include "module.hpp"

using std::cout;
using std::endl;
using std::to_string;

class RsimTest : public Module {
protected:
	bool m_bprint;
public:
	RsimTest(const string name, Board * board) : Module(name, board){
		register_bool("print", "boolean value for printing time.(default y)", true, &m_bprint);
	}

	virtual bool init_process() {
		print(to_string(get_id()) + " : init_process is runnning.\n");
		return true;
	};

	virtual bool main_process() {

		if (m_bprint) {			
			string str =  to_string(get_id()) + ": " + get_time_as_string()+ "\n";
			print(str);
		}
		return true;
	};

	virtual bool finish_process() {
		print(to_string(get_id()) + " : finish_process is running.\n");
		return true;
	};

};