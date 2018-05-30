#include "vertex.hpp"

bool Vertex::EdgeVar::set_value(const string& value) {
	
	return true;
}

Vertex::Vertex() : m_brun(false) {
}

void Vertex::start() {
	m_th = thread(&Vertex::processing_loop, this);
}

void Vertex::join() {
	m_th.join();
}

void Vertex::processing_loop() {
	m_clock.start();
	
	while (true) {
		if (!m_brun)
			break;

		if (!process())
			break;


		m_clock.adjust();
	}
}

void Vertex::stop() {
	m_brun = false;
}

bool Vertex::set_variable(const string& name, const string& value) {
	return m_vars[name]->set_value(value);
}

 