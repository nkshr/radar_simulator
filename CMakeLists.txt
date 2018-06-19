cmake_minimum_required(VERSION 2.8.4)

project(radar_simulator)

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

set(MODULE_CPP_FILES
	src/module/module.cpp
	src/module/cmd_receiver.cpp
	src/module/data_server.cpp
	src/module/modulator.cpp
	src/module/simulator.cpp
)

set(MODULE_HPP_FILES
	src/module/module.hpp
	src/module/cmd_receiver.hpp
	src/module/data_server.hpp
	src/module/simulator.hpp
)

set(SIGNAL_CPP_FILES
	src/signal/signal.cpp
)

set(SIGNAL_HPP_FILES
	src/signal/signal.hpp
)

set(CPP_FILES
	${COMMON_CPP_FILES}
	${MODULE_CPP_FILES}
	${SIGNAL_CPP_FILES}
	src/board.cpp
)

set(HPP_FILES
	${COMMON_HPP_FILES}
	${MODULE_HPP_FILES}
	${SIGNAL_HPP_FILES}
	src/board.hpp
)

add_executable(rsim_server src/server.cpp ${CPP_FILES} ${HPP_FILES})

add_executable(rsim src/client.cpp ${CPP_FILES} ${HPP_FILES})
