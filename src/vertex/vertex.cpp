#include "vertex.hpp"

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
