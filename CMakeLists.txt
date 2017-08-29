cmake_minimum_required(VERSION 2.8.4)

set(SOURCE_FILES
	src/main.cpp
)

set(HEADER_FILES
	src/math.h
)

add_executable(radar_simulator src/main.cpp ${SOURCE_FILES} ${HEADER_FILES})