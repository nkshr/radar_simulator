cmake_minimum_required(VERSION 2.8.4)

project(radar_simulator)

set(SOURCE_FILES
	src/clock.cpp
	src/graph.cpp
	src/main.cpp
	src/miscel.cpp
	src/modulator.cpp
	src/object.cpp
	src/simulator.cpp
)

set(HEADER_FILES
	src/clock.h
	src/config.h
	src/graph.h
	src/math.h
	src/miscel.h
	src/object.h
	src/simulator.h
)

add_executable(radar_simulator src/main.cpp ${SOURCE_FILES} ${HEADER_FILES})