#include <thread>
#include <future>

#include "graph.h"

bool Vertex::run() {
	m_clock.start();

	while (true) {
		if (process())
			break;
		
		m_clock.adjust();
	}
}

void Graph::run() {
	for (int i = 0; i < m_num_vertexes; ++i) {
	}
}