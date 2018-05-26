cmake_minimum_required(VERSION 2.8.4)

project(radar_simulator)

set(VERTEX_CPP_FILES
	src/vertex/data_server.cpp
	src/vertex/modulator.cpp
	src/vertex/simulator.cpp
)

set(VERTEX_HPP_FILES
	src/vertex/data_server.hpp
	src/vertex/simulator.hpp
)

set(COMMON_CPP_FILES
	src/common/clock.cpp
	src/common/miscel.cpp
	src/common/object.cpp
	src/common/tcp.cpp
	src/common/udp.cpp
)

set(COMMON_HPP_FILES
	src/common/clock.hpp
	src/common/math.hpp
	src/common/miscel.hpp
	src/common/object.hpp
	src/common/tcp.hpp
	src/common/udp.hpp
)
	
set(CPP_FILES
	${COMMON_CPP_FILES}
	${VERTEX_CPP_FILES}
	src/graph.cpp
)

set(HPP_FILES
	${COMMON_HPP_FILES}
	${VERTEX_HPP_FILES}
	src/graph.h
)

add_executable(rsim_server src/server.cpp ${CPP_FILES} ${HPP_FILES})

add_executable(rsim src/client.cpp ${CPP_FILES} ${HPP_FILES})
