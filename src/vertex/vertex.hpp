#pragma once
#include <map>
#include <set>
#include <string>
#include <typeinfo>
#include "../common/clock.hpp"
#include "../common/miscel.hpp"

#include "../graph.hpp"
#include "../edge/edge.hpp"

class Vertex {
public:
	class Variable {
	public:
		Variable(const string& name, void* value, Vertex* v) {
			v->m_vars.insert(pair<const string, Variable*>(name, dynamic_cast<Variable*>(this)));
		}
		virtual bool set_value(const string& value) = 0;
	};

	class IntVar : public Variable {
	public:
		virtual bool set_valuel(const string& value) {
			return true;
		}
	};

	class DoubleVar : public Variable {
	public:
		DoubleVar(const string& name, void* value, Vertex* v) : Variable(name, value, v){
		}

		double m_var;
		virtual bool set_value(const string& value) {
			m_var = atof(value.c_str());
			return true;
		}
	};

	class EdgeVar : public Variable {
	public:
		virtual bool set_value(const string& value);
	};

	Vertex();
	void start();
	void join();
	void processing_loop();
	void stop();

	virtual bool process() = 0;

	bool set_variable(const string& name, const string& value);

protected:
	map<const string, Variable*> m_vars;

	bool m_brun;

	thread m_th;

	Clock m_clock;
	Graph *m_graph;

	struct variable {
		string name;
		void* value;
		size_t hash;
	};


	bool register_variable(const string& name, const void* value, size_t hash) {

	}
	//template <typename T>
	//void register_variable(const string& name, T* var) {
	//	m_vars.insert(pair<const string, Variable*>(name, 
	//		dynamic_cast<Variable*>(new Variable<T>(var))));
	//}
};
